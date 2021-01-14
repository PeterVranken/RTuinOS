/**
 * @file tc02_oneTask.c
 *   Test case 02 of RTuinOS. One task is defined, which runs alternatingly with the idle
 * task.\n
 *   Observations:\n
 *   The object Serial is used to write progress and status information to console, under
 * which the current CPU load. It is remarkably high, which is not the consumption of the
 * software implemented here but the effect of the data streaming over the RS 232
 * connection. We have selected a Baud rate of only 9600 bps and all print command block
 * until the characters to print are processed. At the beginning it is very fast as the
 * characters immediately fit into the send buffer. Once is has been filled the print
 * function is in mean as fast as the stream, 9600 characters a second. Even though the
 * rest is just waiting somewhere inside print it's lost CPU processing time for RTuinOS. A
 * hypothetical redesign of the library for serial communication for RTuinOS would
 * obviously use a suspend command to free this time for use by other tasks. This way, the
 * mean CPU load would become independent of the chosen Baud rate.\n
 *   Please consider to change the Baud rate to 115200 bps in setup() to prove that the CPU
 * load strongly goes down.
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
 *   blink
 *   task01_class00
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "gsl_systemLoad.h"


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
 
static void task01_class00(uint16_t taskParam);
 
 
/*
 * Data definitions
 */
 
static uint8_t _taskStack[STACK_SIZE_TASK00];
static uint8_t _cpuLoad = 200;

 
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
 * The only task in this test case (besides idle).
 *   @param initCondition
 * The task gets the vector of events, which made it initially due.
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void task01_class00(uint16_t initCondition)

{
#define TICS_CYCLE  125

    uint16_t u;
    uint32_t ti = millis()
           , tiCycle;
    
    Serial.print("task01_class00: Activated by 0x");
    Serial.println(initCondition, HEX);

    for(u=0; u<3; ++u)
        blink(2);
    
    for(;;)
    {
        Serial.println("task01_class00: rtos_delay...");
        u = rtos_delay(55);
        Serial.print("task01_class00: Released with ");
        Serial.println(u, HEX);
        
        Serial.println("task01_class00: Suspending...");
        u = rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ TICS_CYCLE);
        tiCycle = millis();
        Serial.print("task01_class00: Released with ");
        Serial.println(u, HEX);
        
        /* The system timer tic has a frequency of 490.1961 Hz.
             Caution: The compiler fails to recognize the constant floating point
           expression if there's no explicit, superfluous pair of parenthesis around it.
           With parenthesis it compiles just one product, without it uses several products
           and divisions. */
        Serial.print("Cycle time: ");
        Serial.print((tiCycle-ti) * (100.0/1000.0 / (TICS_CYCLE/490.1961)));
        Serial.println("%");
        
        Serial.print("CPU load: ");
        Serial.print(_cpuLoad/2);
        Serial.println("%");
        
        ti = tiCycle;
    }
    
#undef TICS_CYCLE
} /* End of task01_class00 */





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
    
    /* Configure task 1 of priority class 0 */
    rtos_initializeTask( /* idxTask */          0
                       , /* taskFunction */     task01_class00
                       , /* prioClass */        0
                       , /* pStackArea */       &_taskStack[0]
                       , /* stackSize */        sizeof(_taskStack)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     5
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
    
    /* Share current CPU load measurement with task code, which owns Serial and which can
       thus display it. */
    _cpuLoad = gsl_getSystemLoad();
    
} /* End of loop */




