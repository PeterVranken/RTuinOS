/**
 * @file tc13_eventStates.c
 *   Test case 13 of RTuinOS. Three tasks are executed step by step, controlled by
 * triggering events sent by an always due, periodic task. In each step a task either
 * requests or sends/releases an event. Assertions double-check after each step if the
 * system state is as expected. The state is defined by the balances of mutexes and
 * semaphores. (Please note, the balance of mutexes is always double-checked by assertion
 * inside rtos.c.)\n
 *   It's hard to keep track about which task is in which state and what is the current
 * expected system state. This test case can be understood only because of the accompanying
 * Excel document, which lists the different actions and expected, resulting states. Please
 * consult testSequence.xlsx first.\n
 *   This application doesn't produce much terminal output. It is successful if it doesn't
 * end with an assertion firing.
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
 *   tT2
 *   tT1
 *   tT0
 *   tTControl
 */

/* This test case make no sense in PRODUCTION compilation as all results are checked by
   assertion. */
#ifndef DEBUG
# error This test case needs to be compiled in DEBUG configuration only
#endif


/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "gsl_systemLoad.h"
#include "rtos_assert.h"
#include "stdout.h"
#include "aev_applEvents.h"


/*
 * Defines
 */
 
/** Pin 13 has an LED connected on most Arduino boards. */
#define LED 13
 

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
enum {_idxTaskT0, _idxTaskT1, _idxTaskT2, _idxTaskTControl, _noTasks};


/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
 
/*
 * Data definitions
 */
 
 /** The tasks' stack areas. */
static uint8_t _taskStackT2[STACK_SIZE]
             , _taskStackT1[STACK_SIZE]
             , _taskStackT0[STACK_SIZE]
             , _taskStackTControl[STACK_SIZE];
         
/** The array of semaphores is declared by the RTuinOS code but defined in the application
    to give it the opportunity to initialize all semaphore's counter approriately. */
uintSemaphore_t rtos_semaphoreAry[RTOS_NO_SEMAPHORE_EVENTS] = {2, 2};

/** A counter of the test repetitions, just to provide some "still alive" feedback to the
    operator. */
static volatile uint32_t _noTestCyclesT2         = 0
                       , _noTestCyclesT1         = 0
                       , _noTestCyclesT0         = 0
                       , _noTestCyclesTControl = 0;

/** The step of the defined test sequence. Used for state validation in the tasks. */
static volatile uint8_t _step = 0;    


/*
 * Function implementation
 */

/**
 * Trivial routine that flashes the LED a number of times to give simple feedback. The
 * routine is blocking.
 *   @param noFlashes
 * The number of times the LED is lit.
 */
 
static void blink(uint8_t noFlashes)
{
#define TI_FLASH 150

    while(noFlashes-- > 0)
    {
        digitalWrite(LED, HIGH);  /* Turn the LED on. (HIGH is the voltage level.) */
        delay(TI_FLASH);          /* The flash time. */
        digitalWrite(LED, LOW);   /* Turn the LED off by making the voltage LOW. */
        delay(TI_FLASH);          /* Time between flashes. */
    }                              
    delay(1000-TI_FLASH);         /* Wait for a second after the last flash - this command
                                     could easily be invoked immediately again and the
                                     bursts need to be separated. */
#undef TI_FLASH
} /* End of blink */



/**
 * One of the three inter-communicating tasks. It'll always execute one step of the test
 * sequence on demand of the control task and check the new system state afterwards.
 *   @param initCondition
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void tT0(uint16_t initCondition)

{
    /* The test is forever cyclically repeated. */
    for(;;)
    {
        uint16_t reqEvtVec, gotEvtVec;
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 1);

        /* Check initial balance of semaphores at beginning of test sequence. Normally, an
           application must never evaluate the contents of the semaphore array as it is
           highly volatile. In this specific test case it may as the test advances in
           controlled steps only and data won't change in between. */
        ASSERT(rtos_semaphoreAry[0] == 2  &&  rtos_semaphoreAry[1] == 2);

        /* Step 1: wait for M1, M0 and S0. */
        reqEvtVec = MTX_1 | MTX_0 | SEM_0;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* Double-check: The acqusition must not have blocked the task. Step is unchanged. */
        ASSERT(_step == 1);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 2  &&  rtos_semaphoreAry[0] == 1);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 4);
        
        /* Step 4: This task releases the aquired sync objects M0 and S0. */
        reqEvtVec = MTX_0 | SEM_0;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 5);
        
        /* Step 5: This task sends some ordinary broadcasted events. */
        reqEvtVec = EVT_1 | EVT_0;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 6);
        
        /* Step 6: This task releases the aquired sync object M1. */
        reqEvtVec = MTX_1;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 8);
        
        /* Step 8: This task broadcasts event E1. */
        reqEvtVec = EVT_1;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 9);
        
        /* Step 9: This task broadcasts events E1 and E0. */
        reqEvtVec = EVT_1 | EVT_0;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 11);
        
        /* Step 11: wait for S1 and S0. */
        reqEvtVec = SEM_1 | SEM_0;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* Double-check: The acqusition must not have blocked the task. Step is unchanged. */
        ASSERT(_step == 11);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 1  &&  rtos_semaphoreAry[0] == 0);

        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 12);
        
        /* Step 12: wait for M1 and S1. Both are immediately available. */
        reqEvtVec = MTX_1 | SEM_1;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* Double-check: The acqusition must not have blocked the task. Step is unchanged. */
        ASSERT(_step == 12);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 0  &&  rtos_semaphoreAry[0] == 0);

        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 13);

        /* Step 13: wait for any of the semaphores S1 or S0. */
        reqEvtVec = SEM_1 | SEM_0;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ false, /* timeout */ 0);
        ASSERT((reqEvtVec & gotEvtVec) != 0);
        
        /* The task has been blocked for some steps. Double-check. */
        ASSERT(_step == 14);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 0  &&  rtos_semaphoreAry[0] == 0);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 16);

        /* Step 16: This task releases the aquired sync objects M1, S1 and S0. */
        reqEvtVec = MTX_1 | SEM_1 | SEM_0;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 17);

        /* Step 17: This task releases the aquired sync object S1. */
        reqEvtVec = SEM_1;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T0, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 18);

        /* Step 18: This task releases the aquired sync objects S1. */
        reqEvtVec = SEM_1;
        rtos_sendEvent(reqEvtVec);
        
        /* This task has completed the test sequence. */
        ++ _noTestCyclesT0;
    }
} /* End of tT0 */





/**
 * One of the three inter-communicating tasks. It'll always execute one step of the test
 * sequence on demand of the control task and check the new system state afterwards.
 *   @param initCondition
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void tT1(uint16_t initCondition)

{
    /* The test is forever cyclically repeated. */
    for(;;)
    {
        uint16_t reqEvtVec, gotEvtVec;

        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T1, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 2);
        
        /* Step 2: wait for E1, E0, M0 and S0. */
        reqEvtVec = EVT_1 | EVT_0 | MTX_0 | SEM_0;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* The task has been blocked for some steps. Double-check. */
        ASSERT(_step == 10);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 2  &&  rtos_semaphoreAry[0] == 1);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T1, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 14);
        
        /* Step 14: This task releases the aquired sync object M0 and the not acquired S1
           (meaning: it enlarges the pool managed by this semaphore). */
        reqEvtVec = MTX_0 | SEM_1;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T1, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 15);
        
        /* Step 15: This task releases the aquired sync object S0 and sends the ordinary
           event E1 (which is nobody listening for). */
        reqEvtVec = EVT_1 | SEM_0;
        rtos_sendEvent(reqEvtVec);
        
        /* This task has completed the test sequence. */ 
        ++ _noTestCyclesT1;
    }
} /* End of tT1 */





/**
 * One of the three inter-communicating tasks. It'll always execute one step of the test
 * sequence on demand of the control task and check the new system state afterwards.
 *   @param initCondition
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void tT2(uint16_t initCondition)

{
    /* The test is forever cyclically repeated. */
    for(;;)
    {
        uint16_t reqEvtVec, gotEvtVec;

        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T2, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 3);
        
        /* Step 3: wait for M1, M0 and S0. */
        reqEvtVec = MTX_1 | MTX_0 | SEM_0;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* The task has been blocked for some steps. Double-check. */
        ASSERT(_step == 6);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 2  &&  rtos_semaphoreAry[0] == 0);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T2, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 7);

        /* Step 7: wait for E0 and S1. */
        reqEvtVec = EVT_0 | SEM_1;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ true, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* The task has been blocked for some steps. Double-check. */
        ASSERT(_step == 9);
        
        /* Check balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 1  &&  rtos_semaphoreAry[0] == 0);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T2, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 10);

        /* Step 10: This task releases all the sync objects. */
        reqEvtVec = MTX_1 | MTX_0 | SEM_1 | SEM_0;
        rtos_sendEvent(reqEvtVec);
        
        /* Wait for next step. Check if we are still in sync with the test sequence. */
        rtos_waitForEvent( /* eventMask */ EVT_TRIGGER_T2, /* all */ false, /* timeout */ 0);
        ASSERT(_step == 19);

        /* Step 19. This is the last step of the test sequence. In step 14, task T1 had
           "released" a semaphore it hadn't owned in that step (meaning: It has "produced"
           a data element for the managed pool). Before we can repeat our test sequence we
           have to equalized the balance again. This task will now consume the semaphore
           once, without the intention to ever release it again. The balance should then be
           the same as at the beginning of the sequence. */ 
        reqEvtVec = SEM_1;
        gotEvtVec = rtos_waitForEvent(reqEvtVec, /* all */ false, /* timeout */ 0);
        ASSERT(reqEvtVec == gotEvtVec);
        
        /* The call of task rtos_waitForEvent must not have blocked. Double-check that step
           didn't change. */
        ASSERT(_step == 19);
        
        /* Check (initial) balance of semaphores. */
        ASSERT(rtos_semaphoreAry[1] == 2  &&  rtos_semaphoreAry[0] == 2);
        
        /* This task has completed the test sequence. */
        ++ _noTestCyclesT2;
    }
} /* End of tT2 */





/**
 * The consumer task. It waits for produced data and reports it to the terminal.
 *   @param eventToWaitForVec
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void tTControl(uint16_t initCondition)
{
    /* The pattern in which the tasks each do one step is predetermined by the test case
       specification. */
    static const uint8_t nextTaskAry[] = { 0, 1, 2, 0, 0, 0, 2, 0, 0, 2, 0
                                         , 0, 0, 1, 1, 0, 0, 0, 2
                                         };
    
    /* The basic pattern is to run the control task function regularly. We do this as fast
       as possible; it is resumed in every first timer tic, which is about every 2 ms. */
    while(rtos_suspendTaskTillTime(/* deltaTimeTillResume */ 1))
    {
        /* Trigger one step of the next task. */
        uint16_t evtTriggerTask = EVT_TRIGGER_TASK << nextTaskAry[_step];
        ++ _step;
        rtos_sendEvent(evtTriggerTask);
        
        /* The test is cyclically repeated. */
        if(_step >= sizeof(nextTaskAry)/sizeof(nextTaskAry[0]))
        {
            /* End of cycle reached. */
            _step = 0;
            
            /* Count the successful test cycles and double-check synchronicity of all tasks. */
            ++ _noTestCyclesTControl;
            ASSERT(_noTestCyclesTControl == _noTestCyclesT0
                   &&  _noTestCyclesTControl == _noTestCyclesT1
                   &&  _noTestCyclesTControl == _noTestCyclesT2
                  );
        }
        
        /* Any task may query the task overrun counter and this task is known to be
           regular. So we double-check the counter. */
        ASSERT(rtos_getTaskOverrunCounter(_idxTaskTControl, /* doReset */ false) == 0);
    }

    /* A task function must never return; this would cause a reset. */
    ASSERT(false);

} /* End of tTControl */




/**
 * The initialization of the RTOS tasks and general board initialization.
 */ 

void setup(void)
{
    /* Start serial port and redirect stdout into Serial. */
    init_stdout();
    Serial.begin(9600);
    
    printf("\n" RTOS_RTUINOS_STARTUP_MSG "\n");

    /* Initialize the digital pin as an output. The LED is used for most basic feedback about
       operability of code. */
    pinMode(LED, OUTPUT);
    
    ASSERT(_noTasks == RTOS_NO_TASKS);

    /* Configure task 0 of priority class 1. */
    rtos_initializeTask( /* idxTask */          _idxTaskT0
                       , /* taskFunction */     tT0
                       , /* prioClass */        1
                       , /* pStackArea */       &_taskStackT0[0]
                       , /* stackSize */        sizeof(_taskStackT0)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );

    /* Configure task 0 of priority class 2. */
    rtos_initializeTask( /* idxTask */          _idxTaskT1
                       , /* taskFunction */     tT1
                       , /* prioClass */        2
                       , /* pStackArea */       &_taskStackT1[0]
                       , /* stackSize */        sizeof(_taskStackT1)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
                       
    /* Configure task 0 of priority class 3. */
    rtos_initializeTask( /* idxTask */          _idxTaskT2
                       , /* taskFunction */     tT2
                       , /* prioClass */        3
                       , /* pStackArea */       &_taskStackT2[0]
                       , /* stackSize */        sizeof(_taskStackT2)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );

    /* Configure the control task of priority class 0. */
    rtos_initializeTask( /* idxTask */          _idxTaskTControl
                       , /* taskFunction */     tTControl
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStackTControl[0]
                       , /* stackSize */        sizeof(_taskStackTControl)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
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
    blink(3);
    
    /* The next statement has a significant impact on the frequency of the calls of loop:
       gsl_getSystemLoad blocks for about a second. */
    printf( "Idle task, test cycles: %6lu, CPU load: %3u%%\n"
          , _noTestCyclesTControl
          , (gsl_getSystemLoad()+1)/2
          );
          
} /* End of loop */




