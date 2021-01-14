/**
 * @file tc04_2Task1PrioClass.c
 *   Test case 04 of RTuinOS. Two tasks of same priority class are defined besides the idle
 * task.
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
#include "rtos_assert.h"


/*
 * Defines
 */
 
/** Pin 13 has an LED connected on most Arduino boards. */
#define LED 13
 
/** Stack size of task. */
#define STACK_SIZE_TASK00   256
 

/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
static void task01_class00(uint16_t initCondition);
static void task02_class00(uint16_t initCondition);
 
 
/*
 * Data definitions
 */
 
static uint8_t _taskStack1[STACK_SIZE_TASK00];
static uint8_t _taskStack2[STACK_SIZE_TASK00];
static volatile uint16_t _t1=0, _t2=0, _id=0;
 
 
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
    
    /* Task 1 of priority class 0 */
    rtos_initializeTask( /* idxTask */          0
                       , /* taskFunction */     task01_class00
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack1[0]
                       , /* stackSize */        sizeof(_taskStack1)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     5
                       );

    /* Task 2 of priority class 0 */
    rtos_initializeTask( /* idxTask */          1
                       , /* taskFunction */     task02_class00
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack2[0]
                       , /* stackSize */        sizeof(_taskStack2)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     250
                       );
    
} /* End of setup */




/**
 * The only task in this test case (besides idle).
 *   @param initParam
 * The task gets an initialization parameter for whatever configuration purpose.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void task01_class00(uint16_t initCondition) __attribute((used, noinline));
static void task01_class00(uint16_t initCondition)

{
    uint16_t u;
    uint32_t ti0, ti1;
    
    Serial.print("task01_class00: Activated by 0x");
    Serial.println(initCondition, HEX);

    for(;;)
    {
        ++ _t1;
        Serial.print("_t1: "); Serial.print(_t1);
        Serial.print(", _t2: "); Serial.print(_t2);
        Serial.print(", _id: "); Serial.println(_id);
        
        Serial.println("task01_class00: rtos_delay(20)");
        ti0 = millis();
        rtos_delay(20);
        ti1 = millis();
        Serial.print("task01_class00: Back from delay after ");
        Serial.print((ti1-ti0)/RTOS_TIC_MS);
        Serial.println(" tics");
        
        Serial.print("task01_class00: Suspending at ");
        Serial.println(millis());

        u = rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 125);
        
        Serial.print("task01_class00: Released at ");
        Serial.println(millis());
        //Serial.println(u, HEX);
    }
} /* End of task01_class00 */




static void task02_class00(uint16_t initCondition) __attribute((used, noinline));
static void task02_class00(uint16_t initCondition)

{
    uint16_t u;
    
    for(;;)
    {
        ++ _t2;

        u = rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 100);
    }
} /* End of task02_class00 */





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
    uint8_t u;    
    boolean ok = true;
    
    /* Check stack reserve. */
    for(u=0; u<10; ++u)
    {
        if(_taskStack1[u] != 0x29  ||  _taskStack2[u] != 0x29)
        {
            ok = false;
            break;
        }
    }
    
    /* Try to indicate corruption of stack (if this still works then ...). */
    if(ok)
        blink(2);
    else
        blink(3);

    /* No task overruns should occur. */
    for(u=0; u<RTOS_NO_TASKS; ++u)
    {
        ASSERT(rtos_getTaskOverrunCounter(/* idxTask */ u, /* doReset */ false) == 0);
    }

    ++ _id;    
   
} /* End of loop */




