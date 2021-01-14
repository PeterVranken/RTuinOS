/**
 * @file tc11_mutex.c
 * Test case 11 of RTuinOS. Test case 09 had demonstrated how to implement a safe resource
 * managment without the availability of true operating system provided synchronization
 * objects. In the second release of RTuinOS these objects became available. This test case
 * re-implements test case 09 using a true mutex instead of a pseudo-mutex.\n
 *   The mutex is applied to control the alternating use of a shared resource (Serial) by
 * several tasks. With release 1.0 true to-the-point waiting for a resource becomes
 * possible in RTuinOS.\n
 *   Observations:\n
 *   The test succeeds if the Arduino console shows correct text output. The tasks, which
 * gets the resource uses the shared resource (the global object Serial) to write one line
 * of text to the console. This is purposely done in several portions of output, which are
 * interrupted by task switches. A certain percentage of the task switches will also oocur
 * in the middle of a print command. The lines must nonetheless be always complete.\n
 *   It must be apparent, that the task of higher priority gets the resource if it is
 * concurrent with another one.
 *
 * Copyright (C) 2012-2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Module interface
 *   setup
 *   loop
 * Local functions
 *   getResource
 *   releaseResource
 *   taskC0
 *   taskEntryC0
 *   taskT0_C1
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "tc11_applEvents.h"

/*
 * Defines
 */

/** Pin 13 has an LED connected on most Arduino boards. */
#define LED 13

/** Stack size of all the tasks. */
#define STACK_SIZE 250

/** The number of system timer tics required to implement the time span given in Milli
    seconds. Consider to use an expression like \a TIME_IN_MS(10) as argument to the time
    related RTuinOS API functions in order to get readable code
      @remark
    The double operations are limited to the compile time if the argument of the macro is a
    literal. No double operation is then found in the machine code. Never use this macro
    with runtime expressions! */
#define TIME_IN_MS(tiInMs) ((uintTime_t)((double)(tiInMs)/RTOS_TIC_MS+0.5))



/*
 * Local type definitions
 */

     

/*
 * Local prototypes
 */

static RTOS_TRUE_FCT void getResource(void);
static void releaseResource(void);
static void taskC0(uint8_t idxTask);
static void taskEntryC0(uint16_t postedEventVec);
static void taskT0_C1(uint16_t postedEventVec);


/*
 * Data definitions
 */

static uint8_t _stackT0_C0[STACK_SIZE]
             , _stackT1_C0[STACK_SIZE]
             , _stackT2_C0[STACK_SIZE]
             , _stackT0_C1[STACK_SIZE];


/*
 * Function implementation
 */


/**
 * This function implements the pseudo mutex. It is called by all tasks, which demand to
 * use the shared resource.\n
 *   The function is of type void: When it returns the caller owns the resource and may use
 * it until he returns the resource (to anybody else) by invoking \a releaseResource.
 *   @see
 * void releaseResource(void)
 *   @remark
 * The implementation needs to be reentrant as several tasks will enter the function at the
 * same time.
 */ 

static RTOS_TRUE_FCT void getResource()
{
    /* The task which calls this routine suspends itself by waiting only for the mutex
       which indicates ownership of the shared resource. No timeout is specified.
       Therefore, testing the return code of the suspend command is superflous; any value
       other than the mutex event would be an error of RTuinOS.
         Don't be confused that the same function getResource can individually suspend
       different tasks all calling this function - even overlappingly in time. This is
       what reentrant actually means: The same function code operates individually and
       independently for each caller.
         Only to prove this statement we declared the functions as RTOS_TRUE_FCT. Otherwise
       gcc would simply inline it and there was no issue with reentrance at all. */
#ifdef DEBUG
    uint16_t postedEvtVec =
#endif
    rtos_waitForEvent(EVT_MUTEX_OWNING_RESOURCE, false, 0);
    ASSERT(postedEvtVec == EVT_MUTEX_OWNING_RESOURCE);

} /* End of getResource */




/**
 * The counterpart to getResource: A task, which got the resource should call this function
 * as soon as it no longer requires the resource.\n
 *   @see
 * void getResource(void)
 */ 

static void releaseResource()
{
    /* Signal the availability of the resource to possibly waiting tasks. */
    rtos_sendEvent(EVT_MUTEX_OWNING_RESOURCE);
    
} /* End of releaseResource */




/**
 * The functional code of all round robin tasks of priority class 0 in this application.
 * These tasks try to get the shared resource (Serial). When they got it they use it to do
 * some visible operations, which lasts clearly longer than their activation time.
 * Nonetheless, the operations need to succeed as the task owns the resource independent
 * of being suspended, due or active.
 *   @param idxTask
 * The index 0..2 of the task which executes the (shared) code.
 */ 

static void taskC0(uint8_t idxTask)
{
    uint32_t cnt = 0;
    
    while(true)  
    {
        /* Wait until we get the resource. */
        getResource();
    
        /* Being here, we can be sure to have the resource. Use it. */
        Serial.print("This is task "); Serial.print(idxTask);
        
        /* The ownership of the resource needs to be independent of the status of the
           tasks. To prove this we suspend the task deliberately in the middle of some
           output operation and we use the blocking Arduino function delay that long, that
           we have a high probability of running into a round robin task switch before
           delay returned. */
        Serial.print(": "); Serial.print(++cnt);
        Serial.print(" loops. Thi");
        rtos_delay(TIME_IN_MS(12));
        Serial.print("s line of console output is interrupted by seve");
        delay(31/*ms*/);
        Serial.print("ral task de-activations. ");
        Serial.print("Now the resource Serial is released by task ");
        Serial.println(idxTask);
        
        /* In the original test case tc09, demonstrating the implementation of a
           pseudo-mutex, we had written: "Give other tasks a chance to get the resource.
           After the call of release, we must not immediately cycle around, otherwise the
           chance is high, that we get it immediately again since most of the concurrent
           tasks have the same priority. Suspend deliberately (but as short as possible),
           so that other tasks of the same priority class can become active."
             Using a true mutex, this becomes obsolete. The call of releaseResource returns
           the mutex, which is in the same atomic instance passed on to one of the
           concurrent, waiting, suspended other tasks. If that is a task of same priority it
           still holds true, that it won't interrupt the running task, but two statements
           later, in its next loop, this running task will try to aqcuire the resource
           again but the mutex is not available and it'll then be suspended. */
        releaseResource();
        //rtos_delay(0);
    }
} /* End of taskC0 */





/**
 * The tasks of lowest priority (besides idle) all use the same task function, which thus
 * needs to be reentrant. This is the entry into the common task function.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */

static void taskEntryC0(uint16_t initCondition)
{
    /* A minor problem because of using one function for all tasks: The function needs to
       know, which specific task executes it. We start the task with task-dependent general
       purpose events ant let the function evaluate the parameter to find out. */
    uint8_t idxTask;
    if(initCondition == EVT_START_TASK_T0_C0)
        idxTask = 0;
    else if(initCondition == EVT_START_TASK_T1_C0)
        idxTask = 1;
    else    
    {
        ASSERT(initCondition == EVT_START_TASK_T2_C0);
        idxTask = 2;
    }

    /* Next problem: All tasks using this function use the round robin pattern in order to
       have permament unforeseen task switches. The idle task can set an event to start
       such a task, but then it'll never be activated again. It would fail to activate any
       of the remaining tasks. Therefore, we chain the initial activation: one task
       initiates the next one. */
    if(idxTask == 0)
        rtos_sendEvent(EVT_START_TASK_T1_C0);
    else if(idxTask == 1)
        rtos_sendEvent(EVT_START_TASK_T2_C0);
    
    /* Here, the actual task code begins. The next statement will never return. */
    taskC0(idxTask);
    
} /* End of taskEntryC0 */




/**
 * A task of higher priority does the same as the round robin tasks: It tries to get the
 * shared resource and the test application proves that it can safely use it then.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */
 
static void taskT0_C1(uint16_t initCondition)
{
#define TASK_TIME_T0_C1_MS  21

    uint32_t cnt = 0;
    
    /* The task inspects the results of the interrupt on a regular base. */
    do
    {
        /* Wait until we get the resource. */
        getResource();
    
        /* Being here, we can be sure to have the resource. Use it. */
        
        /* The ownership of the resource needs to be independent of the status of the
           tasks. To prove this we suspend the task deliberately in the middle of some
           output operation and we use the blocking Arduino function delay that long, that
           we have a high probability of running into a round robin task switch before
           delay returned. */
        Serial.print("This is task T0_C1");
        Serial.print(": "); Serial.print(++cnt);
        Serial.print(" loops. This line of conso");
        rtos_delay(TIME_IN_MS(7));
        Serial.print("le output is interr");
        delay(3/*ms*/);
        Serial.print("upted by several task de-activations");
        Serial.println(". Now the resource is released again");

        /* Give other tasks a chance to get the resource. */
        releaseResource();
        
        /* Here, no other task will already use the acquired resource: All concurrent tasks
           are of lower priority. One of them will have become due but not active yet. The
           while condition will now make this task inactive and the already due other one
           active. */
    }
    while(rtos_suspendTaskTillTime(TIME_IN_MS(TASK_TIME_T0_C1_MS)));

#undef TASK_TIME_T0_C1_MS
} /* End of taskT0_C1 */




/**
 * The initalization of the RTOS tasks and general board initialization.
 */

void setup(void)
{
    /* Start serial port at 9600 bps. */
    Serial.begin(9600);
    Serial.println("\n" RTOS_RTUINOS_STARTUP_MSG);

    /* Initialize the digital pin as an output. The LED is used for most basic feedback about
       operability of code. */
    pinMode(LED, OUTPUT);

    uint8_t idxTask = 0
          , idxClass = 0;
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskEntryC0
                       , /* prioClass */        idxClass
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   23
#endif
                       , /* pStackArea */       &_stackT0_C0[0]
                       , /* stackSize */        sizeof(_stackT0_C0)
                       , /* startEventMask */   EVT_START_TASK_T0_C0
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskEntryC0
                       , /* prioClass */        idxClass
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   29
#endif
                       , /* pStackArea */       &_stackT1_C0[0]
                       , /* stackSize */        sizeof(_stackT1_C0)
                       , /* startEventMask */   EVT_START_TASK_T1_C0
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskEntryC0
                       , /* prioClass */        idxClass
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   31
#endif
                       , /* pStackArea */       &_stackT2_C0[0]
                       , /* stackSize */        sizeof(_stackT2_C0)
                       , /* startEventMask */   EVT_START_TASK_T2_C0
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    
    /* Initialization of tasks of next priority class starts here. */
    ++ idxClass;
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C1
                       , /* prioClass */        idxClass
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   0
#endif
                       , /* pStackArea */       &_stackT0_C1[0]
                       , /* stackSize */        sizeof(_stackT0_C1)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    ASSERT(idxTask == RTOS_NO_TASKS  &&  idxClass+1 == RTOS_NO_PRIO_CLASSES);

} /* End of setup */




/**
 * The application owned part of the idle task. This routine is repeatedly called whenever
 * there's some execution time left. It's interrupted by any other task when it becomes
 * due.
 *   @remark
 * Different to all other tasks, the idle task routine may and should return. (The task as
 * such doesn't terminate). This has been designed in accordance with the meaning of the
 * original Arduino loop function.
 */

void loop(void)
{
    /* Idle is used only to start the first round robin task. */
    rtos_sendEvent(EVT_START_TASK_T0_C0);

    /* In test case tc09 of RTuinOS we had written: "Since we have a pseudo mutex only,
       which is implemented by polling (try and suspend until next try) there are minor
       gaps in time where all tasks are suspended. To not run into the sendEvent again, we
       place an empty loop here. Having a true mutex implementation, we would place an
       ASSERT(false) instead."
         Now we have re-implemented the test case using a true mutex but the statement
       turned out to be wrong, such an assertion fires occasionally. The reason is, that
       the tasks apply the other suspend command rtos_delay so that it might happen that
       all of them are suspended at the same time and idle becomes active. If all calls of
       rtos_delay are commented out the assertion is indeed safe. */
    //ASSERT(false);
    while(true)
        ;

} /* End of loop */




