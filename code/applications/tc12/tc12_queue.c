/**
 * @file tc12_queue.c
 *   Test case 12 of RTuinOS. Two tasks implement a producer-consumer system. The producer
 * computes samples of the sine function and files them in a queue. The second task, which
 * is of higher priority, waits for queued data and prints the values to the terminal
 * output. How to build queues on semaphores for safe and polling-free inter-task
 * communication is demonstrated by this code sample.\n
 *   Such an architecture basically leads to a simple pattern. The producer puts one sample
 * into the queue. The consumer gets immediately awaken as he has the higher priority. He
 * consumes the sample and goes sleeping; control returns to the consumer.\n
 *   To make this pattern somewhat more complex and to demonstrate the capability of
 * combining wait-for-event conditions to a more complex resume condition we have defined a
 * second phase of processing. After a predetermined number of the simple producer-consumer
 * cycles, the consumer extends its resume condition: It now waits for the semaphore event,
 * which continues to notify data-queued events and for an other, ordinary event. This
 * event is broadcasted asynchronously by the idle task. The consumer is triggered by this
 * event and reads all meanwhile queued elements en block.\n
 *   The pattern has been made even more complex by introducing a mutex for shared,
 * alternating access to the console output: Both tasks write their progress messages into
 * Serial. Caution, this is not an example of proper code design but just to make it more
 * complex and a better test case. The mutual exclusion from the serial output degrades the
 * accurate timing of the basically regular consumer task.
 *   @remark: This application produces a lot of screen output and requires a terminal Baud
 * rate higher then the standard setting. It'll produce a lot of trash in the Arduino
 * console window if you do not switch the Baud rate in Arduino's Serial Monitor to 115200
 * Baud. See setup() for more.
 *   @remark The output of the sine generator is printed with the printf command applying
 * its floating point formatting characters. This requires that the code is linked against
 * a special library, which supports the printf floating point features (and which
 * increases the code size by about 2k). If you see question marks instead of figures you
 * linked against the standard library. This sample makes use of the makefile "callback" in
 * order to let the right library be linked. Please refer to tc12.mk for details.
 *
 * Copyright (C) 2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
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
 *   blink
 *   taskT0C0_producer
 *   tC0C0
 *   taskT0C1_consumer
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "stdout.h"
#include "integerSineZ.h"
#include "gsl_systemLoad.h"
#include "itq_interTaskQueue.h"
#include "aev_applEvents.h"


/*
 * Defines
 */
 
/** Common stack size of tasks. */
#define STACK_SIZE   256
 
 
/** The number of system timer tics required to implement the time span given in Milli
    seconds. Consider to use an expression like \a TIME_IN_MS(10) as argument to the time
    related RTuinOS API functions in order to get readable code
      @remark
    The double operations are limited to the compile time if the argument of the macro is a
    literal. No double operation is then found in the machine code. Never use this macro
    with runtime expressions! */
#define TIME_IN_MS(tiInMs) ((uintTime_t)((double)(tiInMs)/RTOS_TIC_MS+0.5))


/** The indexes of the tasks are named to make index based API functions of RTuinOS safely
    usable. */
enum {_idxTaskT0C0, _idxTaskT0C1, _noTasks};


/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
static void taskT0C1_consumer(uint16_t initCondition);
static void tT0C0(uint16_t initCondition);
 
 
/*
 * Data definitions
 */
 
static uint8_t _taskStackT0C1[STACK_SIZE]
             , _taskStackT0C0[STACK_SIZE];
             
             
/** The CPU load as computed in the idle task. A shared global variable is used because it is
    reported in one of the other tasks, which may use the terminal - idle must not do so in
    this application! */
static volatile uint8_t _cpuLoad = 200;
 
 
/** The semaphore of type uint8_t counts the number of samples in the queue, which are
    already produced but not yet consumed. The start value needs to be null.
      @remark Although this variable is shared between tasks and although its value is
   shared by others tasks it must not be declared as volatile. Actually, no task will
   directly read or write to this variable, tasks do this only indirectly by calling the
   related RTuinOS API functions - and to the RTuinOS code the variable is not volatile. */
uintSemaphore_t rtos_semaphoreAry[RTOS_NO_SEMAPHORE_EVENTS] = {0};


/*
 * Function implementation
 */


/**
 * The function code of the producer task. This function code is regularly called. It
 * unconditionally computes a data sample and puts it into the queue.
 */ 

static void taskT0C0_producer()
{
    uint32_t tiNow = millis();
    
    static uint32_t cnt_ = 0
                  , tiLastCall_ = 0;
                  
    /* The producer wants to log its activities, so it needs to wait for the mutex related
       to Serial. We specify a timeout but if it ever elapsed it would be a failure in this
       test case. */
#ifdef DEBUG
    uint16_t gotEvents = 
#endif
    rtos_waitForEvent( EVT_MUTEX_SERIAL | RTOS_EVT_DELAY_TIMER
                     , /* all */ false
                     , /* timeout */ TIME_IN_MS(10)
                     );

    /* The assertion fires if we see a timeout. */
    ASSERT(gotEvents == EVT_MUTEX_SERIAL);

    /* Do some reporting. We own the mutex. */
    printf("Producer:\n  Time: %3lu\n  CPU load: %5.1f%%\n", tiNow-tiLastCall_, 0.5*_cpuLoad);
    tiLastCall_ = tiNow;
    
    /* Produce data. */
    integerSineZ_step();
    int16_t nextSampleSine = integerSineZ_Y.y;

    /* Queue the data. This step implicitly increments the related semaphore. A client of
       the queue gets the notification that a data element is available. In our specific
       test case, and in the first phase of the test, this will make the consumer shortly
       due and active: It will just invoke the command to wait for the mutex, which grants
       access to the Serial object, and then we are back here. */
    itq_writeElem(nextSampleSine);
    
    /* Do some more reporting after task switch hence and force. We still own the mutex. */
    printf("  Queued data sample %8lu = %.6f\n", cnt_++, nextSampleSine/32768.0);
    
    /* We need to release the mutex, so that the consumer can report its activities. */
    rtos_sendEvent(EVT_MUTEX_SERIAL);

} /* End of taskT0C0_producer */



/**
 * The producer task. It unconditionally computes a data sample and puts it into the queue.
 *   @param initCondition
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void tT0C0(uint16_t initCondition)

{
#define TASK_TIME  120  /* ms */

    /* Initialize the external sinus generator module. */
    integerSineZ_initialize();

    /* The basic pattern is to run the producer task function regularly. */
    do
    {
        taskT0C0_producer();       
        
        /* Any task may query the task overrun counter and this task is intended to be
           regular. So we can double-check the counter.
             Remark: Both tasks request and wait for the mutex, which synchronizes the
           access to the serial communication channel. The consumer task is awaken by the
           idle task at arbitrary times; if this happens shortly before this task, the
           producer, becomes due, it'll not become active at the expected time. The
           consumer has the higher priority and its many printf statements take a lot of
           time. The activation of the consumer is postponed accordingly. However, this is
           just a priority caused jitter and not (yet) a task overrun. */
        ASSERT(rtos_getTaskOverrunCounter(_idxTaskT0C0, /* doReset */ false) == 0);
    }
    while(rtos_waitForEvent( /* eventMask */ RTOS_EVT_ABSOLUTE_TIMER              
                           , /* all */       false
                           , /* timeout */   TIME_IN_MS(TASK_TIME)
                           )
         );
    
    /* Termination code of sinus generator module is actually never reached (and not
       required as we don't terminate). */
    //integerSineZ_terminate();

    /* A task function must never return; this would cause a reset. */
    ASSERT(false);
    
#undef TASK_TIME
} /* End of tT0C0 */





/**
 * The consumer task. It waits for produced data and reports it to the terminal.
 *   @param eventToWaitForVec
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void taskT0C1_consumer(uint16_t eventToWaitForVec)
{
    uint32_t cnt_ = 0;
    
    /* During phase 1 we get awake as soon as data is signalled by the semaphore associated
       with the queue. The condition will be extended in phase 2. */
    eventToWaitForVec = EVT_SEMAPHORE_ELEM_IN_QUEUE;
    
    while(rtos_waitForEvent( /* eventMask */ eventToWaitForVec
                           , /* all */       true
                           , /* timeout */   0
                           )
         )
    {
        uint8_t noElemGot = 0;
        
        /* Get temporary access to the global, shared communication stream. */
        rtos_waitForEvent(/* eventMask */ EVT_MUTEX_SERIAL, /* all */ false, /* timeout */ 0);
        
        printf("Consumer: wake up\n");
                
        /* Since we awaked because of the received semaphore event we can be sure to get at
           least one element from the queue. Then we have a loop to read all other elements
           which were possibly queued meanwhile: There's no guarantee, that this task got
           due and immediately active because of the first semaphore posted by the
           producer. */
        do
        {
            int16_t nextSampleSine = itq_readElem();
            ++ noElemGot;
            printf("  Received data sample %6lu = %.6f\n", cnt_, nextSampleSine/32768.0);
            ++ cnt_;

            /* The while condition of this loop necessarily needs to use a timeout: If data
               is available in the queue, rtos_waitForEvent will return immediately with
               the semaphore event, without suspending this task. If there's no data left,
               the task is shortly suspended but becomes due and active again after the
               timeout.
                 Please note, that RTuinOS doesn't have a special handling of timeout 0.
               This timeout value suspends until the next timer tic, which is 0..2ms ahead.
               0 does not mean not to suspend at all! */
        }
        while(rtos_waitForEvent
                    ( /* eventMask */ RTOS_EVT_DELAY_TIMER | EVT_SEMAPHORE_ELEM_IN_QUEUE
                    , /* all */       false
                    , /* timeout */   0
                    )
              == EVT_SEMAPHORE_ELEM_IN_QUEUE
             );
        
        /* Print a summary before sleeping again. */
        printf("  Received %u samples in this task-awake-cycle\n", noElemGot);
        
        /* Just to make the sample a bit more interessting: We change the task resume
           condition after a while. From now on we wait for a combination of events, so
           that the data is no longer received one-by-one but in packages. The difference
           should become apparent in the console output. */
        if(cnt_ == 100)        
        {
            printf("  Now switching to more complex task resume condition\n");
            eventToWaitForVec |= EVT_TRIGGER_CONSUMER_TASK;
        }
        
        /* Now we have to return the mutex related to the global, shared Serial object.
           Getting this mutex is one of the conditions to awake the data producer. */
        rtos_sendEvent(EVT_MUTEX_SERIAL);
    }

    /* A task function must never return; this would cause a reset. */
    ASSERT(false);
    
} /* End of taskT0C1_consumer */




/**
 * The initialization of the RTOS tasks and general board initialization.
 */ 

void setup(void)
{
    /* Start serial port at high Baud rate (we print a lot) and redirect stdout into
       Serial. */
    init_stdout();
    Serial.begin(115200);
    
    puts_progmem(rtos_rtuinosStartupMsg);

    ASSERT(_noTasks == RTOS_NO_TASKS);

    /* Configure task 0 of priority class 0. The producer has the lower priority. It is
       started immediately. */
    rtos_initializeTask( /* idxTask */          _idxTaskT0C0
                       , /* taskFunction */     tT0C0
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStackT0C0[0]
                       , /* stackSize */        sizeof(_taskStackT0C0)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );

    /* Configure task 0 of priority class 1. The consumer has the higher priority. It is
       started by: Data available AND access to object Serial granted. */
    rtos_initializeTask( /* idxTask */          _idxTaskT0C1
                       , /* taskFunction */     taskT0C1_consumer
                       , /* prioClass */        1
                       , /* pStackArea */       &_taskStackT0C1[0]
                       , /* stackSize */        sizeof(_taskStackT0C1)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     10
                       );
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
    /* Remark: The idle task must not use the terminal as it can't use the suspend command
       to acquire the related mutex. */
    
    /* Caution, reliable CPU load measurement is not possible in this application: The idle
       task triggers the consumer of the data and is thus not asynchronous with the task
       activation pattern. The observation time window of the system load estimation does
       not not have a view on an arbitrary part of this pattern, but will always see the
       same task activation - which does not necessarily result in a valid average. */
    _cpuLoad = gsl_getSystemLoad();
    
    /* In each loop - which is about once a second because of the behavior of
       gsl_getSystemLoad - we trigger the consumer task. It should then report all data
       samples produced meanwhile at once. */
    rtos_sendEvent(EVT_TRIGGER_CONSUMER_TASK);
    
} /* End of loop */




