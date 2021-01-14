/**
 * @file tc07_criticalSection.c
 * Test case 07 of RTuinOS. Several tasks of same priority are defined plus a few of
 * higher priority. The tasks of same priority use round robin and all tasks access some
 * shared global data using the two different pairs of functions for the implementation of
 * mutual exclusion.\n
 *   One task purposely accesses the data in the same way but disregarding the mutual
 * exclusion in order to prove that this leads to errors, which are not seen in all other
 * cases.\n
 *   Also the task of highest priority doesn't use the specific function pairs to ensure
 * mutual exclusion. Just having the highest priority already guarantees this - no error
 * must be seen in this task.\n
 *   Observations:\n
 *   The round robin tasks show far the most loops. They are free running and use all
 * remaining processing time.\n
 *   The first and the forth of the round robin tasks have the same implementation
 * (including the implementation of mutual exclusion) but the latter has a larger time
 * slice and reports an accordingly larger number of loops.\n
 *   The reporting task is clearly the slowest, although it got a significantly larger time
 * slice.\n
 * One task reports occasional data errors, all others don't. The particular task
 * (purposely) disregards the need to use a critical section for safe data access.\n
 *   The first three of the round robin tasks have the same time slice and they are
 * implemented identically besides using different functions to implement mutual exclusion.
 * We would expect the same number of loops. Actually, we see a difference, which reflects
 * the different cost of the different implementations of mutual exclusion. cli/sei is
 * indeed faster than enter/leaveCriticalSection.\n
 *   The idle task implements the blinking LED. However, here we have an example of
 * starvation: The idle task is always due but will not become active, since the round
 * robin tasks (which have a higher priority) never suspend. Consequently, the LED on the
 * Arduino board is not blinking.
 *
 * Copyright (C) 2012 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
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
 *   taskT0_C0
 *   taskT1_C0
 *   taskT2_C0
 *   taskT3_C0
 *   taskT4_C0
 *   taskT0_C1
 *   taskT1_C1
 *   taskT0_C2
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "rtos_assert.h"


/*
 * Defines
 */

/** Pin 13 has an LED connected on most Arduino boards. */
#define LED 13

/** Stack size of all but the reporting tasks. */
#define STACK_SIZE 100

/** Stack size of the reporting task. */
#define STACK_SIZE_REPORT 200

/*
 * Local type definitions
 */


/*
 * Local prototypes
 */

static void taskT0_C0(uint16_t postedEventVec);
static void taskT1_C0(uint16_t postedEventVec);
static void taskT2_C0(uint16_t postedEventVec);
static void taskT3_C0(uint16_t postedEventVec);
static void taskT4_C0(uint16_t postedEventVec);
static void taskT0_C1(uint16_t postedEventVec);
static void taskT1_C1(uint16_t postedEventVec);
static void taskT0_C2(uint16_t postedEventVec);


/*
 * Data definitions
 */

static uint8_t _stackT0_C0[STACK_SIZE]
             , _stackT1_C0[STACK_SIZE]
             , _stackT2_C0[STACK_SIZE]
             , _stackT3_C0[STACK_SIZE]
             , _stackT4_C0[STACK_SIZE_REPORT]
             , _stackT0_C1[STACK_SIZE]
             , _stackT1_C1[STACK_SIZE]
             , _stackT0_C2[STACK_SIZE];

/** A global variable shared by all tasks. */
static volatile uint8_t _globalVar = 0;

/** Task owned variables which record what happens. */
static volatile uint16_t _errT0_C0 = 0
                       , _errT1_C0 = 0
                       , _errT2_C0 = 0
                       , _errT3_C0 = 0
                       , _errT4_C0 = 0
                       , _errT0_C1 = 0
                       , _errT1_C1 = 0
                       , _errT0_C2 = 0;
static volatile uint32_t _cntLoopsT0_C0 = 0
                       , _cntLoopsT1_C0 = 0
                       , _cntLoopsT2_C0 = 0
                       , _cntLoopsT3_C0 = 0
                       , _cntLoopsT4_C0 = 0
                       , _cntLoopsT0_C1 = 0
                       , _cntLoopsT1_C1 = 0
                       , _cntLoopsT0_C2 = 0;

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
#define TI_FLASH 150 /* ms */

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
}




/**
 * One of the low priority tasks in this test case. The task continously increments a
 * global variable and compares the result with the same computation done on local data.
 * Without harmful interference of another task the results are identical. The found
 * differences are counted as errors.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */

static void taskT0_C0(uint16_t initCondition)
{
    /* The infinite loop of this task is interrupted by task switches due to round robin
       events and by regular tasks of higher priority. */
    while(true)
    {
        uint8_t localVar, globalResult;

        /* This task inhibits all interrupts to ensure mutal exclusion on access to the
           shared data. */
        cli();
        localVar = _globalVar;
        globalResult = ++_globalVar;
        sei();

        if(++localVar != globalResult)
            ++ _errT0_C0;

        ++ _cntLoopsT0_C0;
    }
} /* End of taskT0_C0 */


static void taskT1_C0(uint16_t initCondition)
{
    /* The infinite loop of this task is interrupted by task switches due to round robin
       events and by regular tasks of higher priority. */
    while(true)
    {
        uint8_t localVar, globalResult;

        /* To ensure mutal exclusion, this task uses the RTuinOS functions to inhibit only
           those interrupts, which are known to RTuinOS to cause possible task switches.
           All other Arduino interrupts stay enabled. */
        rtos_enterCriticalSection();
        localVar = _globalVar;
        globalResult = ++_globalVar;
        rtos_leaveCriticalSection();

        if(++localVar != globalResult)
            ++ _errT1_C0;

        ++ _cntLoopsT1_C0;
    }
} /* End of taskT1_C0 */


static void taskT2_C0(uint16_t initCondition)
{
    /* The infinite loop of this task is interrupted by task switches due to round robin
       events and by regular tasks of higher priority. */
    while(true)
    {
        uint8_t localVar, globalResult;

        /* This task disregards the mutual exclusion while accessing the data and should
           show occasional errors. */
        localVar = _globalVar;
        globalResult = ++_globalVar;

        if(++localVar != globalResult)
            ++ _errT2_C0;

        ++ _cntLoopsT2_C0;
    }
} /* End of taskT2_C0 */


static void taskT3_C0(uint16_t initCondition)
{
    /* The infinite loop of this task is interrupted by task switches due to round robin
       events and by regular tasks of higher priority. */
    while(true)
    {
        uint8_t localVar, globalResult;

        cli();
        localVar = _globalVar;
        globalResult = ++_globalVar;
        sei();

        if(++localVar != globalResult)
            ++ _errT3_C0;

        ++ _cntLoopsT3_C0;
    }
} /* End of taskT3_C0 */


static void taskT4_C0(uint16_t initCondition)
{
    /* The infinite loop of this task is interrupted by task switches due to round robin
       events and by regular tasks of higher priority. */
    while(true)
    {
        uint8_t localVar, globalResult;

        /* To ensure mutal exclusion, this task uses the RTuinOS functions to inhibit only
           those interrupts, which are known to RTuinOS to cause possible task switches.
           All other Arduino interrupts stay enabled. */
        rtos_enterCriticalSection();
        localVar = _globalVar;
        globalResult = ++_globalVar;
        rtos_leaveCriticalSection();

        if(++localVar != globalResult)
            ++ _errT4_C0;

        ++ _cntLoopsT4_C0;

        /* This task (the last one in the chain of round robin tasks) reports the errors
           and loop counts. */
#define REPORT_TASK(T, C)                                                                   \
        {                                                                                   \
            uint16_t noErr;                                                                 \
            uint32_t cntLoops;                                                              \
                                                                                            \
            /* Get safe and consistent copy of data to print. */                            \
            cli();                                                                          \
            cntLoops = _cntLoopsT##T##_C##C;                                                \
            noErr = _errT##T##_C##C;                                                        \
            sei();                                                                          \
                                                                                            \
            /* Print data, where this can be interrupted by all other tasks, in particular  \
               by the other round robin tasks, which might continue to count loops,         \
               although their last result has not yet completely printed here. The output   \
               made here are arbitrary samples of loop and error counters. We expect to see \
               much lees loops of this task compared to the other round robin tasks having  \
               the same time slice. */                                                      \
            Serial.print("TaskT" #T "_C" #C ": loops: ");                                   \
            Serial.print(cntLoops);                                                         \
            Serial.print(", errors: ");                                                     \
            Serial.println(noErr);                                                          \
                                                                                            \
        } /* Macro definition REPORT_TASK */

        REPORT_TASK(0, 0)
        REPORT_TASK(1, 0)
        REPORT_TASK(2, 0)
        REPORT_TASK(3, 0)
        REPORT_TASK(4, 0)
        REPORT_TASK(0, 1)
        REPORT_TASK(1, 1)
        REPORT_TASK(0, 2)

#undef REPORT_TASK

        Serial.print("Stack reserve: ");
        Serial.println(rtos_getStackReserve(/* idxTask */ 0));
        Serial.print("Stack reserve: ");
        Serial.println(rtos_getStackReserve(/* idxTask */ 4));
        
    } /* End while(true) */
} /* End of taskT4_C0 */


static void taskT0_C1(uint16_t initCondition)
{
#define TASK_TIME_T0_C1 11

    do
    {
        uint8_t localVar, globalResult;

        cli();
        localVar = _globalVar;
        globalResult = ++_globalVar;
        sei();

        if(++localVar != globalResult)
            ++ _errT0_C1;

        ++ _cntLoopsT0_C1;
    }
    while(rtos_suspendTaskTillTime(TASK_TIME_T0_C1));

#undef TASK_TIME_T0_C1
} /* End of taskT0_C1 */


static void taskT1_C1(uint16_t initCondition)
{
#define TASK_TIME_T1_C1 5

    do
    {
        uint8_t localVar, globalResult;

        cli();
        localVar = _globalVar;
        globalResult = ++_globalVar;
        sei();

        if(++localVar != globalResult)
            ++ _errT1_C1;

        ++ _cntLoopsT1_C1;
    }
    while(rtos_suspendTaskTillTime(TASK_TIME_T1_C1));

#undef TASK_TIME_T1_C1
} /* End of taskT1_C1 */


static void taskT0_C2(uint16_t initCondition)
{
#define TASK_TIME_T0_C2 3

    do
    {
        uint8_t localVar, globalResult;

        /* To ensure mutal exclusion, this task does nothing. It has the highest priority
           of all tasks, no other task has the same priority and the task can't be
           interrupted by anybody. */
        localVar = _globalVar;
        globalResult = ++_globalVar;

        if(++localVar != globalResult)
            ++ _errT0_C2;

        ++ _cntLoopsT0_C2;
    }
    while(rtos_suspendTaskTillTime(TASK_TIME_T0_C2));

#undef TASK_TIME_T0_C2
} /* End of taskT0_C2 */





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

    uint8_t idxTask = 0;
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C0
                       , /* prioClass */        0
                       , /* timeRoundRobin */   10
                       , /* pStackArea */       &_stackT0_C0[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT1_C0
                       , /* prioClass */        0
                       , /* timeRoundRobin */   10
                       , /* pStackArea */       &_stackT1_C0[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT2_C0
                       , /* prioClass */        0
                       , /* timeRoundRobin */   10
                       , /* pStackArea */       &_stackT2_C0[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT3_C0
                       , /* prioClass */        0
                       , /* timeRoundRobin */   30
                       , /* pStackArea */       &_stackT3_C0[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT4_C0
                       , /* prioClass */        0
                       , /* timeRoundRobin */   75
                       , /* pStackArea */       &_stackT4_C0[0]
                       , STACK_SIZE_REPORT
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C1
                       , /* prioClass */        1
                       , /* timeRoundRobin */   0
                       , /* pStackArea */       &_stackT0_C1[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT1_C1
                       , /* prioClass */        1
                       , /* timeRoundRobin */   0
                       , /* pStackArea */       &_stackT1_C1[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C2
                       , /* prioClass */        2
                       , /* timeRoundRobin */   0
                       , /* pStackArea */       &_stackT0_C2[0]
                       , STACK_SIZE
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    ASSERT(idxTask == RTOS_NO_TASKS);

} /* End of setup */




/**
 * The application owned part of the idle task. This routine is repeatedly called whenever
 * there's some execution time left. It's interrupted by any other task when it becomes
 * due.
 *   @remark
 * In this specific application, where we have a set of always due round robin tasks, idle
 * will never become active once the first task has been started. The LED should not blink.
 *   @remark
 * Different to all other tasks, the idle task routine may and should return. (The task as
 * such doesn't terminate). This has been designed in accordance with the meaning of the
 * original Arduino loop function.
 */

void loop(void)
{
    /* Idle may be active the very short time until the first system timer tic, which
       releases most of the tasks. To avoid a half way entered routine blink, we wait
       initially 3 ms. */
    delay(3);
    blink(2);

} /* End of loop */




