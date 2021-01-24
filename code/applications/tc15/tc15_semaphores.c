/**
 * @file tc15_semaphores.c
 *   Test case tc15 has been created to catch a problem found in a pre-release of RTuinOS
 * 1.0. It's used as regression test only and doesn't do anything exciting or instructive.
 * It double-check the correctness of semaphore counting balances. The test results are
 * checked by assertion, it's useless to compile this code in production configuation.
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
 */
/* Module interface
 *   setup
 *   loop
 * Local functions
 *   taskT0C3
 *   taskT0C2
 *   taskT0C1
 *   taskT0C0
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "stdout.h"
#include "gsl_systemLoad.h"
//#include "tc15_semaphores.h"


/*
 * Defines
 */

#define STACK_SIZE 256


/*
 * Local type definitions
 */


/*
 * Local prototypes
 */


/*
 * Data definitions
 */

static uint8_t stackT0C3[STACK_SIZE]
             , stackT0C2[STACK_SIZE]
             , stackT0C1[STACK_SIZE]
             , stackT0C0[STACK_SIZE];

static volatile uint32_t noLoopsT0C3 = 0
                       , noLoopsT0C2 = 0
                       , noLoopsT0C1 = 0
                       , noLoopsT0C0 = 0;

uintSemaphore_t rtos_semaphoreAry[RTOS_NO_SEMAPHORE_EVENTS] = {0, 0, 0, 0, 0, 0, 0, 0};


/*
 * Function implementation
 */

/**
 * Task of highest priority. It's a regular task, which generates semaphore counts.
 *   @param initialResumeCondition
 * The vector of events, which made this task initially due.
 */

static void taskT0C3(uint16_t initialResumeCondition)
{
    do
    {
        /* All events 0..7 are defined as semaphores. Increment count of some of these. The
           sent set is partly demanded by several other tasks. We can double-check if the
           one of higher priority gets it. */
        rtos_sendEvent(0x0E);

        /* Wait a bit and give chance to other due tasks of lower priority to become
           active. */
        rtos_delay(1 /* unit 2ms */);

        /* Increment count of other semaphores. */
        rtos_sendEvent(0x70);
        
        /* By counting the loops in between the two sendEvent operations and shortly
           suspending this task, we can double-check at the receiving task, if the instance
           of resuming is correct. */ 
        ++ noLoopsT0C3;
        rtos_delay(1 /* unit 2ms */);
        
        /* Increment count of other semaphores. */
        rtos_sendEvent(0x80);
    }
    while(rtos_suspendTaskTillTime(/* deltaTimeTillResume */ 5 /* unit 2ms */));

} /* End of taskT0C3 */



/**
 * Task of medium priority. It's a got-semaphore triggered task, which consumes semaphore
 * counts.
 *   @param initialResumeCondition
 * The vector of events, which made this task initially due.
 */

static void taskT0C2(uint16_t initialResumeCondition)
{
    for(;;)
    {
#ifdef DEBUG
        uint16_t gotEventsVec =
#endif
        rtos_waitForEvent( /* eventMask */ 0x07 | RTOS_EVT_DELAY_TIMER
                         , /* all       */ false
                         , /* timeout   */ 6 /* unit 2ms */
                         );
        ASSERT(gotEventsVec == 0x06);
        
        ASSERT(noLoopsT0C2 == noLoopsT0C3);
        ++ noLoopsT0C2;
    }
} /* End of taskT0C2 */



/**
 * Task of lower priority. It's a got-semaphore triggered task, which consumes semaphore
 * counts.
 *   @param initialResumeCondition
 * The vector of events, which made this task initially due.
 */

static void taskT0C1(uint16_t initialResumeCondition)
{
    for(;;)
    {
#ifdef DEBUG
        uint16_t gotEventsVec =
#endif
        rtos_waitForEvent( /* eventMask */ 0x0F | RTOS_EVT_DELAY_TIMER
                         , /* all       */ false
                         , /* timeout   */ 6 /* unit 2ms */
                         );

        /* This task of lower priority just got one of the sent semaphores, the two others,
           which are also requested by T0C2 went to that task as it has the higher
           priority. */
        ASSERT(gotEventsVec == 0x08);

        ASSERT(noLoopsT0C1 == noLoopsT0C3);
        ++ noLoopsT0C1;
    }
} /* End of taskT0C1 */




/**
 * Task of lowest priority. It's a got-semaphore triggered task, which consumes semaphore
 * counts.
 *   @param initialResumeCondition
 * The vector of events, which made this task initially due.
 */

static void taskT0C0(uint16_t initialResumeCondition)
{
    for(;;)
    {
#ifdef DEBUG
        uint16_t gotEventsVec =
#endif
        rtos_waitForEvent( /* eventMask */ 0xF0 | RTOS_EVT_DELAY_TIMER
                         , /* all       */ true
                         , /* timeout   */ 6 /* unit 2ms */
                         );
        ASSERT(gotEventsVec == 0xF0);
        
        ++ noLoopsT0C0;
        ASSERT(noLoopsT0C0 == noLoopsT0C3);
    }
} /* End of taskT0C0 */




/**
 * Initialization of system, particularly specification of tasks and their properties.
 */

void setup()
{
    /* Start serial communication for some feedback about the progress and redirect stdout
       into Serial. */
    Serial.begin(9600);
    init_stdout();

    rtos_initializeTask( /* idxTask          */ 0
                       , /* taskFunction     */ taskT0C0
                       , /* prioClass        */ 0
                       , /* pStackArea       */ stackT0C0
                       , /* stackSize        */ sizeof(stackT0C0)
                       , /* startEventMask   */ RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout     */ 1
                       );

    rtos_initializeTask( /* idxTask          */ 1
                       , /* taskFunction     */ taskT0C1
                       , /* prioClass        */ 1
                       , /* pStackArea       */ stackT0C1
                       , /* stackSize        */ sizeof(stackT0C1)
                       , /* startEventMask   */ RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout     */ 1
                       );

    rtos_initializeTask( /* idxTask          */ 2
                       , /* taskFunction     */ taskT0C2
                       , /* prioClass        */ 2
                       , /* pStackArea       */ stackT0C2
                       , /* stackSize        */ sizeof(stackT0C2)
                       , /* startEventMask   */ RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout     */ 1
                       );

    rtos_initializeTask( /* idxTask          */ 3
                       , /* taskFunction     */ taskT0C3
                       , /* prioClass        */ 3
                       , /* pStackArea       */ stackT0C3
                       , /* stackSize        */ sizeof(stackT0C3)
                       , /* startEventMask   */ RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout     */ 1
                       );

    /* Write greeting to the console. */
    puts_progmem(rtos_rtuinosStartupMsg);

} /* End of setup */




/**
 * The idle task loop function. Is cyclically invoked by the RTuinOS kernel if no other
 * task is due.
 */

void loop()
{
    static __attribute__((section(".progmem.strings"))) const char idleMsg[] = "RTuinOS is idle";
    puts_progmem(idleMsg);

    uint8_t cpuLoad = gsl_getSystemLoad();
    printf("CPU load: %3u.%s%%\n", cpuLoad>>1, (cpuLoad&0x01)!=0? "5": "0");
    
    cli();
    uint32_t noLoops = noLoopsT0C2;
    sei();
    printf("%5lu test cycles after %7lu ms\n", noLoops, millis());

} /* End of loop */







