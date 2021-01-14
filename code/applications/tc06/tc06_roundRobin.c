/**
 * @file tc06_roundRobin.c
 *   Test case 06 of RTuinOS. Two round robin tasks of same priority are defined. Task
 * switches are controlled by manually posted and time-slice-elapsed events and counted and
 * reported in the idle task. The sample tests correct priority handling when activating
 * resumed tasks and demonstrates how difficult to predict task timing becomes if round
 * robin time slices are in use. Here we have a task which seems to be regular on the first
 * glance but the round robin strategy introduces significant uncertainties. See comments
 * below.\n
 *   The test success is mainly checked by many assertions. The task overruns reported in
 * the console output for task index 1 are unavoidable and no failure. (The function
 * \a rtos_getTaskOverrunCounter is applicable only for simple, regular tasks.) 
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
 *   rtos_enableIRQTimerTic
 *   loop
 * Local functions
 *   blink
 *   subRoutine
 *   task00_class00
 *   task01_class00
 *   task00_class01
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
static RTOS_TRUE_FCT void subRoutine(uint8_t);
 
 
/*
 * Data definitions
 */
 
static uint8_t _taskStack00_C0[STACK_SIZE_TASK00_C0]
             , _taskStack01_C0[STACK_SIZE_TASK01_C0]
             , _taskStack00_C1[STACK_SIZE_TASK00_C1];

static volatile uint16_t _noLoopsTask00_C0 = 0;
static volatile uint16_t _noLoopsTask01_C0 = 0;
static volatile uint16_t _noLoopsTask00_C1 = 0;
 
static volatile uint16_t _task00_C0_cntWaitTimeout = 0;
static volatile uint8_t _touchedBySubRoutine;

 
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
 * A sub routine which has the only meaning of consuming stack - in order to test the stack
 * usage computation.
 *   @param nestedCalls
 * The routine will call itself recursively \a nestedCalls-1 times. In total the stack will
 * be burdened by \a nestedCalls calls of this routine.
 *   @remark
 * The optimizer removes the recursion completely. The stack-use effect of the sub-routine
 * is very limited, but still apparent the first time it is called.
 */ 

static RTOS_TRUE_FCT void subRoutine(uint8_t nestedCalls)
{
    volatile uint8_t stackUsage[43];
    if(nestedCalls > 1)
    {
        _touchedBySubRoutine += 2;
        stackUsage[0] = 
        stackUsage[sizeof(stackUsage)-1] = 0;
        subRoutine(nestedCalls-1);    
    }
    else
    {
        ++ _touchedBySubRoutine;
        stackUsage[0] = 
        stackUsage[sizeof(stackUsage)-1] = nestedCalls;
    }
} /* End of subRoutine */





/**
 * Test of redefining the central interrupt of RTuinOS. The default implementation of the
 * interrupt configuration function is overridden by redefining the same function.\n
 */ 

void rtos_enableIRQTimerTic(void)
{
    Serial.println("Overloaded interrupt initialization rtos_enableIRQTimerTic in " __FILE__);
    
#ifdef __AVR_ATmega2560__
    /* Initialization of the system timer: Arduino (wiring.c, init()) has initialized
       timer2 to count up and down (phase correct PWM mode) with prescaler 64 and no TOP
       value (i.e. it counts from 0 till MAX=255). This leads to a call frequency of
       16e6Hz/64/510 = 490.1961 Hz, thus about 2 ms period time.
         Here, we found on this setting (in order to not disturb any PWM related libraries)
       and just enable the overflow interrupt. */
    TIMSK2 |= _BV(TOIE2);
#else
# error Modifcation of code for other AVR CPU required
#endif
    
} /* End of rtos_enableIRQTimerTic */






/**
 * One of the low priority round robin tasks in this test case.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void task00_class00(uint16_t initCondition)

{
    for(;;)
    {
        ++ _noLoopsTask00_C0;

        /* To see the stack reserve computation working we invoke a nested sub-routine
           after a while. */
        if(millis() > 20000ul)
            subRoutine(1);
        if(millis() > 30000ul)
            subRoutine(2);
        if(millis() > 40000ul)
            subRoutine(3);
        
        /* The next operation (Arduino delay function) takes the demanded world time in ms
           (as opposed to CPU time) even if it is interrupted because of an elapsed round
           robin counter.
             This task has a round robin time slice of 10 tics (20 ms) only, so it should
           surely be interrupted during execution of delay. The other round robin task has
           a time slice of 4 ms. No other tasks demand the CPU significantly. Consequently,
           the code in delay should not be interrupted for longer than about 4 ms. Coming
           back here means to immediately do the next check if the demanded time has
           elapsed. We expect thus to not prolongue the demanded time by more than about 4
           ms. */
        uint32_t ti0 = millis();
        delay(600 /* ms */);
        uint16_t dT = (uint16_t)(millis() - ti0);
        ASSERT(dT >= 599)
        ASSERT(dT < 609);

        /* Wait for an event from the idle task. The idle task is asynchrounous and its
           speed depends on the system load. The behavior is thus not perfectly
           predictable. Let's have a look on the overrrun counter for this task. It might
           occasionally be incremented. */
        if(rtos_waitForEvent( /* eventMask */ RTOS_EVT_EVENT_03 | RTOS_EVT_DELAY_TIMER
                            , /* all */ false
                            , /* timeout */ 1000 /* unit 2 ms */
                            )
           == RTOS_EVT_DELAY_TIMER
          )
        {
            ++ _task00_C0_cntWaitTimeout;
        }
    }
} /* End of task00_class00 */





/**
 * Second round robin task of low priority in this test case.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void task01_class00(uint16_t initCondition)

{
    uint32_t tiCycle0 = millis();
    for(;;)
    {
        uint16_t u;
        
        ++ _noLoopsTask01_C0;

        /* The next operation (Arduino delay function) takes the demanded world time in ms
           (as opposed to CPU time) even if it is interrupted because of an elapsed round
           robin counter.
             As this task has a round robin time slice of 4 ms, the delay operation will
           surely be interrupted by the other task - which may consume the CPU for up to 20
           ms. The delay operation may thus return after 24 ms. */
        uint32_t ti0 = millis();
        delay(8 /* ms */);
        uint16_t dT = (uint16_t)(millis() - ti0);
        ASSERT(dT >= 7);
        ASSERT(dT <= 25);

        /* Release the high priority task for a single cycle. It should continue operation
           before we leave the suspend function here. Check it. */
        ti0 = millis();
        u = _noLoopsTask00_C1;
        rtos_sendEvent(/* eventVec */ RTOS_EVT_EVENT_00);
        ASSERT(u+1 == _noLoopsTask00_C1)
        ASSERT(_noLoopsTask01_C0 == _noLoopsTask00_C1)
        dT = (uint16_t)(millis() - ti0);
        ASSERT(dT <= 2);
        
        /* The body of this task takes up to about 26 ms (see before). If it suspends here,
           the other round robin task will most often become active and consume the CPU the
           next 20 ms. This tasks wants to cycle with 40 ms. So it'll become due while the
           other round robin task is active. This task will become active only after the
           time slice of the other task has elapsed. Exact cycle time is impossible for
           this task.
             It can even be worse if the other round robin task should be suspendend while
           this task suspends itself till the next multiple of 40 ms: Occasionally, the
           other task will resume just before this task and the activation of this task
           will be delayed by the full time slice duration of the other round robin task.
           Task overruns are unavoidable for this (ir-)regular task, but we can give an
           upper boundary for the cycle time, which is tested by assertion. */
        rtos_suspendTaskTillTime(/* deltaTimeTillRelease */ 20 /* unit 2 ms */);
        uint32_t tiCycleEnd = millis();
        dT = (uint16_t)(tiCycleEnd - tiCycle0);
        tiCycle0 = tiCycleEnd;
        ASSERT(dT <= 62);
    }
} /* End of task01_class00 */





/**
 * Task of high priority.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */ 

static void task00_class01(uint16_t initCondition)

{
    ASSERT(initCondition == RTOS_EVT_EVENT_00)
    
    /* This tasks cycles once each time it is awaked by the event. The timeout condition
       must be weak: The triggering task seems to have a cycle time 40 ms on the first
       glance, but there's an uncertainty in the magnitude of the round robin time slice
       duration of the second, concurring task. Although this leads to an upper boundary of
       about 60 ms for the (irregular) cycle time of the triggering task, the uncertainty
       here is even larger: The point in time of the trigger event relative to the begin of
       a cycle does also vary in the magnitude of the other round robin's time slice. The
       maximum distance in time of two trigger events can thus be accordingly larger in the
       worsed case. */
    do
    {
        /* As long as we stay in the loop we didn't see a timeout. */
        
        /* Count the loops. */
        ++ _noLoopsTask00_C1;
    }
    while(rtos_waitForEvent( /* eventMask */ RTOS_EVT_EVENT_00 | RTOS_EVT_DELAY_TIMER
                           , /* all */ false
                           , /* timeout */ (62+20)/2 /* unit 2 ms */
                           )
          == RTOS_EVT_EVENT_00
         );
    
    /* We must never get here. Otherwise the test case failed. In compilation mode
       PRODUCTION, when there's no assertion, we will see an immediate reset because we
       leave a task function. */
    ASSERT(false)

} /* End of task00_class01 */





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
    
    /* Task 0 of priority class 0 */
    rtos_initializeTask( /* idxTask */          0
                       , /* taskFunction */     task00_class00
                       , /* prioClass */        0
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   10
#endif
                       , /* pStackArea */       &_taskStack00_C0[0]
                       , /* stackSize */        sizeof(_taskStack00_C0)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );

    /* Task 1 of priority class 0 */
    rtos_initializeTask( /* idxTask */          1
                       , /* taskFunction */     task01_class00
                       , /* prioClass */        0
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   2
#endif
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
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                       , /* timeRoundRobin */   0
#endif
                       , /* pStackArea */       &_taskStack00_C1[0]
                       , /* stackSize */        sizeof(_taskStack00_C1)
                       , /* startEventMask */   RTOS_EVT_EVENT_00
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
    uint8_t idxStack;
    
    /* An event can be posted even if nobody is listening for it. */
    rtos_sendEvent(/* eventVec */ RTOS_EVT_EVENT_04);

    /* This event will release task 0 of class 0. However we do not get here again fast
       enough to avoid all timeouts in that task. */
    rtos_sendEvent(/* eventVec */ RTOS_EVT_EVENT_03);

#define Serial_println(volatileN) {cli(); uint16_t n=(volatileN); sei(); Serial.println(n);}

    Serial.println("RTuinOS is idle");
    Serial.print("noLoopsTask00_C0: "); Serial_println(_noLoopsTask00_C0);
    Serial.print("_task00_C0_cntWaitTimeout: "); Serial_println(_task00_C0_cntWaitTimeout);
    Serial.print("noLoopsTask01_C0: "); Serial_println(_noLoopsTask01_C0);
    Serial.print("noLoopsTask00_C1: "); Serial_println(_noLoopsTask00_C1);
    
#undef Serial_println

    /* Look for the stack usage and task overruns. (The task concept implemented here
       brings such overruns for task 1.) */
    for(idxStack=0; idxStack<RTOS_NO_TASKS; ++idxStack)
    {
        Serial.print("Stack reserve of task ");
        Serial.print(idxStack);
        Serial.print(": ");
        Serial.print(rtos_getStackReserve(idxStack));
        Serial.print(", task overrun: ");
        Serial.println(rtos_getTaskOverrunCounter(idxStack, /* doReset */ false));
    }
    
    blink(2);
    
} /* End of loop */




