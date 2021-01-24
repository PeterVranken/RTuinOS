/**
 * @file tc03_RTTasks.c
 *   Test case 03 of RTuinOS. Several tasks of different priority are defined. Task
 * switches are counted and reported in the idle task.
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
 *   task01_class00
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"


/*
 * Defines
 */
 
/** Pin 13 has an LED connected on most Arduino boards. */
#define LED 13
 
/** Stack size of all the tasks. */
#define STACK_SIZE_TASK00_C0   256
#define STACK_SIZE_TASK01_C0   256
#define STACK_SIZE_TASK00_C1   256
 

/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
static void task00_class00(uint16_t postedEventVec);
static void task01_class00(uint16_t postedEventVec);
static void task00_class01(uint16_t postedEventVec);
 
 
/*
 * Data definitions
 */
 
static uint8_t _taskStack00_C0[STACK_SIZE_TASK00_C0]
             , _taskStack01_C0[STACK_SIZE_TASK01_C0]
             , _taskStack00_C1[STACK_SIZE_TASK00_C1];



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
}



/**
 * One of the low priority tasks in this test case.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static volatile uint16_t _noLoopsTask00_C0 = 0;
static void task00_class00(uint16_t initCondition)

{
    for(;;)
    {
        ++ _noLoopsTask00_C0;

        /* This tasks cycles with about 200ms but it is nearly always suspended and doesn't
           produce significant CPU load. */
        rtos_delay(80);
        rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 100);
    }
} /* End of task00_class00 */





/**
 * Second task of low priority in this test case.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static volatile uint16_t _noLoopsTask01_C0 = 0;
static void task01_class00(uint16_t initCondition)

{
    for(;;)
    {
        ++ _noLoopsTask01_C0;

        /* For test purpose only: This task consumes the CPU for most of the cycle time and
           while the task is active. The call of delay produces a CPU load of about 80%.
           This is less self-explaining as it looks on the first glance. The Arduino
           function delay is implemented as loop, which compares the current system time
           with a target time, the desired time of return. The current system time is
           clocked by an interrupt independent of RTuinOS. This loop will basically run
           during 80 ms in a cycle of about 100 ms - but not continuously. The task of
           higher priority will frequently interrupt and shortly halt the loop. Therefore
           the 80% of CPU load do not result from this task (as it may seem) but from this
           task and all others which may interrupt it while it is looping inside delay. */
        delay(80 /*ms*/);
        
        /* This tasks cycles with about 100ms. */
        rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 50);
    }
} /* End of task01_class00 */





/**
 * Task of high priority.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static volatile uint16_t _noLoopsTask00_C1 = 0;
static void task00_class01(uint16_t initCondition)

{
    for(;;)
    {
        ++ _noLoopsTask00_C1;

        /* This tasks cycles with about 2 ms. */
        //u = rtos_delay(255);
        rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 1);
    }
} /* End of task00_class01 */





/**
 * The initalization of the RTOS tasks and general board initialization.
 */ 

void setup(void)
{
    /* Start serial port at 9600 bps. */
    Serial.begin(9600);
    Serial.println("\n" RTOS_RTUINOS_STARTUP_MSG);

    /* All tasks are set up. */
    
    /* Task 0 of priority class 0 */
    rtos_initializeTask( /* idxTask */          0
                       , /* taskFunction */     task00_class00
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack00_C0[0]
                       , /* stackSize */        sizeof(_taskStack00_C0)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     5
                       );

    /* Task 1 of priority class 0 */
    rtos_initializeTask( /* idxTask */          1
                       , /* taskFunction */     task01_class00
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack01_C0[0]
                       , /* stackSize */        sizeof(_taskStack01_C0)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     15
                       );

    /* Task 0 of priority class 1 */
    rtos_initializeTask( /* idxTask */          2
                       , /* taskFunction */     task00_class01
                       , /* prioClass */        1
                       , /* pStackArea */       &_taskStack00_C1[0]
                       , /* stackSize */        sizeof(_taskStack00_C1)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     5
                       );
    
    /* Initialize the digital pin as an output. The LED is used for most basic feedback about
       operability of code. */
    pinMode(LED, OUTPUT);
    
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
    Serial.println("RTuinOS is idle");
    Serial.print("_noLoopsTask00_C0: "); Serial.println(_noLoopsTask00_C0);
    Serial.print("_noLoopsTask01_C0: "); Serial.println(_noLoopsTask01_C0);
    Serial.print("_noLoopsTask00_C1: "); Serial.println(_noLoopsTask00_C1);
    blink(4);
    
} /* End of loop */




