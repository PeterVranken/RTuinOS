/**
 * @file tc08_applInterrupt.c
 * Test case 08 of RTuinOS. Two timers different to the RTuinOS system timer are installed
 * as additional task switch causing interrupt sources. These interrupts set an individual
 * event which triggers an associated task of high priority. The interrupt events of the
 * associated tasks are counted to demonstrate the operation.\n
 *   A dedicated task is used for feedback. The Arduino LED signals the number of
 * application interrupts. Occasionally, a series of flashes is produced, which represents
 * the number of interrupts so far. (To not overburden the flashes counting human, the
 * length of the series is limited to ten.) This feedback giving task gets active only on
 * demand; it's triggered by an application event from another task.\n
 *   Observations:\n
 *   The frequency of the timer interrupts (timers 4 and 5 have been used) can be varied in
 * a broad range. In this test case the application interrupt 00 is configured to occur
 * with about 1 kHz. This is more than double the frequency of the RTuinOS system clock,
 * which determines the highest frequency of calling regular tasks. Having an even faster
 * application interrupt doesn't matter, the scheduler easily handles task switches faster
 * than the system timer.\n
 *   The start of the application interrupts is significantly delayed. The first interrupts
 * are seen only 2 or 3 seconds after reset. The application reports an according number of
 * timeouts at the beginning. This is a strange, still unexplained behavior. It's probably
 * not an RTuinOS effect as could be proven with a simple standard Arduino sketch. This
 * sketch used the identical code to configure the interrupt source and contained only a
 * ++counter operation in the ISR and a Serial.println(counter) in \a loop. It showed
 * exactly the same effect. One could assume the delay is caused by some initialization of
 * the Arduino libraries, however, the only complex functionality used is Serial.print and
 * this is visibly up and running while the interrupts are still in delay. Furthermore, why
 * does the RTuinOS system timer (timer 2) doesn't show the same effect?
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
 *   rtos_enableIRQUser00
 * Local functions
 *   blinkNoBlock
 *   taskT0_C0
 *   taskT0_C1
 *   taskT0_C2
 *   taskT1_C2
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "rtos_assert.h"
#include "tc08_applEvents.h"

/*
 * Defines
 */

/** Pin 13 has an LED connected on most Arduino boards. */
#define LED 13

/** Stack size of all the tasks. */
#define STACK_SIZE 100

/** The number of system timer tics required to implement the time span given in Milli
    seconds.
      @remark
    The double operations are limited to the compile time. No such operation is found in
    the machine code. */
#define TIC(tiInMs) ((uintTime_t)((double)(tiInMs)/RTOS_TIC_MS+0.5))


/*
 * Local type definitions
 */

/** The task indexes. */
enum { idxTaskT0_C0 = 0
     , idxTaskT0_C1
     , idxTaskT0_C2
     , idxTaskT1_C2
     , noTasks
     };
     

/*
 * Local prototypes
 */

static void taskT0_C0(uint16_t postedEventVec);
static void taskT0_C1(uint16_t postedEventVec);
static void taskT0_C2(uint16_t postedEventVec);
static void taskT1_C2(uint16_t postedEventVec);


/*
 * Data definitions
 */

static uint8_t _stackT0_C0[STACK_SIZE]
             , _stackT0_C1[STACK_SIZE]
             , _stackT0_C2[STACK_SIZE]
             , _stackT1_C2[STACK_SIZE];

/** Task owned variables which record what happens. */
static volatile uint32_t _cntLoopsT0_C2 = 0
                       , _cntLoopsT1_C2 = 0;

/** The application interrupt handler counts missing interrupt events (timeouts) as errors. */
static volatile uint16_t _errT0_C2 = 0;

/** Input for the blink-task: If it is triggered, it'll read this variable and produce a
    sequence of flashes of according length. */
static volatile uint8_t _blink_noFlashes = 0;
   

/*
 * Function implementation
 */


/**
 * Trivial routine that flashes the LED a number of times to give simple feedback. The
 * routine is none blocking. It must not be called by the idle task as it uses a suspend
 * command.\n
 *   The number of times the LED is lit is read by side effect from the global variable \a
 * _blink_noFlashes. Writing this variable doesn't require access synchronization as this
 * function is called solely from the task of lowest priority.\n
 *   The flash sequence is started by setting 
 */

static void blinkNoBlock(uint8_t noFlashes)
{
#define TI_FLASH 250 /* ms */

    while(noFlashes-- > 0)
    {
        digitalWrite(LED, HIGH);    /* Turn the LED on. (HIGH is the voltage level.) */
        rtos_delay(TIC(TI_FLASH));  /* The flash time. */
        digitalWrite(LED, LOW);     /* Turn the LED off by making the voltage LOW. */
        rtos_delay(TIC(TI_FLASH));  /* Time between flashes. */
    }    
    
    /* Wait for two seconds after the last flash - this command could easily be invoked
       immediately again and the bursts need to be separated. */
    rtos_delay(TIC(2000-TI_FLASH));
    
#undef TI_FLASH
}




/**
 * The task of lowest priority (besides idle) is used for reporting. When released by an
 * event it produces a sequence of flash events of the LED. The number of flashes is
 * determined by the value of the global variable \a _blink_noFlashes. The sequence is
 * released by RTuinOS event \a EVT_START_FLASH_SEQUENCE.
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.\n
 *   The number of times the LED is lit is read by side effect from the global variable \a
 * _blink_noFlashes.
 */

static void taskT0_C0(uint16_t initCondition)
{
    ASSERT(initCondition == EVT_START_FLASH_SEQUENCE);
    do
    {
        /* The access to the shared variable is not protected: The variable is an uint8_t
           and a read operation is atomic anyway. */
        blinkNoBlock(_blink_noFlashes);
    }
    while(rtos_waitForEvent(EVT_START_FLASH_SEQUENCE, /* all */ false, /* timeout */ 0));
    
} /* End of taskT0_C0 */




/**
 * A task of medium priority. It looks at the counter incremented by the interrupt handler
 * and reports when it reaches a certain limit. Reporting is done by releasing the blinking
 * task. 
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */
 
static void taskT0_C1(uint16_t initCondition)
{
#define TASK_TIME_T0_C1_MS  50 
#define TRIGGER_DISTANCE    8000

    /* Since we are the only client of the blink task we can abuse the interface variable as
       static counter at the same time. The first sequence shall have a single flash. */
    _blink_noFlashes = 0;
    
    /* The task inspects the results of the interrupt on a regular base. */
    do
    {
        static uint32_t lastTrigger = TRIGGER_DISTANCE;
        
        /* The pair of functions enter/leaveCriticalSection had to be customized for this
           RTuinOS application: Besides RTuinOS standard system clock interrupt both
           application interrupt sources must be inhibited/released. The customization is
           done in the application owned RTuinOS configuration file. */
        rtos_enterCriticalSection();
        boolean trigger = _cntLoopsT0_C2 >= lastTrigger;
        rtos_leaveCriticalSection();

        if(trigger)
        {
            /* Next reported event is reached. Start the flashing task. The number of times
               the LED is lit is exchanged by side effect in the global variable
               _blink_noFlashes. Writing this variable doesn't basically require access
               synchronization as this task has a higher priority than the blink task and
               because it's a simple uint8_t. However, we are anyway inside a critical
               section. */
               
            /* Limit the length of the sequence to a still recognizable value.
                 A read-modify-write on the shared variable outside a critical section can
               solely be done since we are the only writing task. */
            if(_blink_noFlashes < 10)
                ++ _blink_noFlashes;
                
            /* TRigger the other task. As it has the lower priority, it's actually not
               activated before we suspend a little bit later. */
            rtos_sendEvent(EVT_START_FLASH_SEQUENCE);
            
            /* Set next trigger point. If we are too slow, it may run away. */
            lastTrigger += TRIGGER_DISTANCE;
        }
    }
    while(rtos_suspendTaskTillTime(TIC(TASK_TIME_T0_C1_MS)));

#undef TASK_TIME_T0_C1_MS
} /* End of taskT0_C1 */




/**
 * A task of high priority is associated with the application interrupts. It counts its
 * occurances and when it is missing (timeout).
 *   @param initCondition
 * Which events made the task run the very first time?
 *   @remark
 * A task function must never return; this would cause a reset.
 */
 
static void taskT0_C2(uint16_t initCondition)
{
#define TIMEOUT_MS  10

    /* This task just reports the application interrupt 00 by incrementing a global counter. */
    while(true)
    {
        while(rtos_waitForEvent( RTOS_EVT_ISR_USER_00 | RTOS_EVT_DELAY_TIMER
                               , /* all */ false
                               , /* timeout */ TIC(TIMEOUT_MS)
                               )
              == RTOS_EVT_ISR_USER_00
             )
        {
            /* Normal situation: Application interrupt came before timeout. No access
               synchronization is required as this task has the highest priority of all
               data accessors. */
            ++ _cntLoopsT0_C2;
        }
    
        /* Inner loop left because of timeout. This may happen only at system
           initialization, because the application interrupts are always enabled a bit
           later than the RTuinOS system timer intterrupt.
              No access synchronization is required as this task has the highest priority
           of all data accessors. */
        if(_errT0_C2 < (uint16_t)-1)
            ++ _errT0_C2;

        /* Outer while condition: No true error recovery, just wait for next application
           interrupt. */
    }
#undef TIMEOUT_MS
} /* End of taskT0_C2 */




static void taskT1_C2(uint16_t initCondition)
{
    /* This task just reports the application interrupt 01 by incrementing a global counter. */
    while(true)
    {
#ifdef DEBUG
        ASSERT(
#endif
               rtos_waitForEvent(RTOS_EVT_ISR_USER_01, /* all */ false, /* timeout */ 0)
#ifdef DEBUG
               == RTOS_EVT_ISR_USER_01
              )
#endif
        ;

        /* No access synchronization is required as this task has the highest priority of all
           data accessors. */
        ++ _cntLoopsT1_C2;

        /* Outer while condition: Wait for next application interrupt. */
    }
#undef TIMEOUT_MS
} /* End of taskT1_C2 */




/**
 * Callback from RTuinOS: The application interrupt 00 is configured and released.
 */

void rtos_enableIRQUser00()
{
#ifdef __AVR_ATmega2560__
    /* Timer 4 is reconfigured. Arduino has put it into 8 Bit fast PWM mode. We need the
       phase and frequency correct mode, in which the frequency can be controlled by
       register OCA. This register is buffered, the CPU writes into the buffer only. After
       each period, the buffered value is read and determines the period duration of the
       next period. The period time (and thus the frequency) can be varied in a well
       defined and glitch free manner. The settings are:
         WGM4 = %1001, the mentioned operation mode is selected. The 4 Bit word is partly
       found in TCCR4A and partly in TCCR4B.
         COM4A/B/C: don't change. The three 2 Bit words determine how to derive up to
       three PWM output signals from the counter. We don't change the Arduino setting; no
       PWM wave form is generated. The three words are found as the most significant 6 Bit
       of register TCCR4A.
         OCR4A = 7812.5 Hz/f_irq, the frequency determining 16 Bit register. OCR4A must not
       be less than 3.
         CS4 = %101, the counter selects the CPU clock divided by 1024 as clock. This
       yields the lowest possible frequencies -- good make the operation visible using the
       LED. */
    TCCR4A &= ~0x03; /* Lower half word of WGM */
    TCCR4A |=  0x01;
    
    TCCR4B &= ~0x1f; /* Upper half word of WGM and CS */
    TCCR4B |=  0x15;
    
    /* We choose 8 as initial value, or f_irq = 976 Hz. This is about double the system
       clock of RTuinOS in its standard configuration (which is used in this test case). */
    OCR4A = 8u;

    TIMSK4 |= 1;    /* Enable overflow interrupt. */
#else
# error Modification of code for other AVR CPU required
#endif

} /* End of rtos_enableIRQUser00 */





/**
 * Callback from RTuinOS: The application interrupt 01 is configured and released.
 */

void rtos_enableIRQUser01()
{
#ifdef __AVR_ATmega2560__
    /* Timer 5 is reconfigured. Arduino has put it into 8 Bit fast PWM mode. We need the
       phase and frequency correct mode, in which the frequency can be controlled by
       register OCA. This register is buffered, the CPU writes into the buffer only. After
       each period, the buffered value is read and determines the period duration of the
       next period. The period time (and thus the frequency) can be varied in a well
       defined and glitch free manner. The settings are:
         WGM5 = %1001, the mentioned operation mode is selected. The 4 Bit word is partly
       found in TCCR5A and partly in TCCR5B.
         COM5A/B/C: don't change. The three 2 Bit words determine how to derive up to
       three PWM output signals from the counter. We don't change the Arduino setting; no
       PWM wave form is generated. The three words are found as the most significant 6 Bit
       of register TCCR5A.
         OCR5A = 7812.5 Hz/f_irq, the frequency determining 16 Bit register. OCR5A must not
       be less than 3.
         CS5 = %101, the counter selects the CPU clock divided by 1024 as clock. This
       yields the lowest possible frequencies -- good make the operation visible using the
       LED. */
    TCCR5A &= ~0x03; /* Lower half word of WGM */
    TCCR5A |=  0x01;
    
    TCCR5B &= ~0x1f; /* Upper half word of WGM and CS */
    TCCR5B |=  0x15;
    
    /* We choose 7813 as initial value, or f_irq = 1 Hz. */
    OCR5A = 7813u;

    TIMSK5 |= 1;    /* Enable overflow interrupt. */
#else
# error Modification of code for other AVR CPU required
#endif

} /* End of rtos_enableIRQUser01 */





/**
 * The initalization of the RTOS tasks and general board initialization.
 */

void setup(void)
{
    /* Start serial port at 9600 bps. */
    Serial.begin(9600);
    Serial.println(F("\r" RTOS_RTUINOS_STARTUP_MSG));

    /* Initialize the digital pin as an output. The LED is used for most basic feedback about
       operability of code. */
    pinMode(LED, OUTPUT);

    uint8_t idxTask = 0;
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C0
                       , /* prioClass */        0
                       , /* pStackArea */       &_stackT0_C0[0]
                       , /* stackSize */        sizeof(_stackT0_C0)
                       , /* startEventMask */   EVT_START_FLASH_SEQUENCE
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C1
                       , /* prioClass */        1
                       , /* pStackArea */       &_stackT0_C1[0]
                       , /* stackSize */        sizeof(_stackT0_C1)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT0_C2
                       , /* prioClass */        2
                       , /* pStackArea */       &_stackT0_C2[0]
                       , /* stackSize */        sizeof(_stackT0_C2)
                       , /* startEventMask */   RTOS_EVT_DELAY_TIMER
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    rtos_initializeTask( /* idxTask */          idxTask++
                       , /* taskFunction */     taskT1_C2
                       , /* prioClass */        2
                       , /* pStackArea */       &_stackT1_C2[0]
                       , /* stackSize */        sizeof(_stackT1_C2)
                       , /* startEventMask */   RTOS_EVT_ISR_USER_01
                       , /* startByAllEvents */ false
                       , /* startTimeout */     0
                       );
    ASSERT(idxTask == RTOS_NO_TASKS  &&  noTasks == RTOS_NO_TASKS);

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
    /* Get a safe copy of the volatile global data. */
    rtos_enterCriticalSection();
    uint32_t noInt = _cntLoopsT0_C2;
    uint16_t noTimeout = _errT0_C2;
    rtos_leaveCriticalSection();
            
    Serial.print("No application interrupts 00: ");
    Serial.print(noInt);
    Serial.print(", timeouts: ");
    Serial.println(noTimeout);

    rtos_enterCriticalSection();
    noInt = _cntLoopsT1_C2;
    rtos_leaveCriticalSection();
    
    Serial.print("No application interrupts 01: ");
    Serial.println(noInt);
    
    Serial.print("Stack reserve: ");
    Serial.print(rtos_getStackReserve(/* idxTask */ 0));
    Serial.print(", ");
    Serial.print(rtos_getStackReserve(/* idxTask */ 1));
    Serial.print(", ");
    Serial.print(rtos_getStackReserve(/* idxTask */ 2));
    Serial.print(", ");
    Serial.println(rtos_getStackReserve(/* idxTask */ 3));

    Serial.print("Overrun T0_C1: "); 
    Serial.println(rtos_getTaskOverrunCounter(/* idxTask */ idxTaskT0_C1
                                             , /* doReset */ false
                                             )
                  );
    
    /* Don't flood the console windows too much. We anyway show only arbitrarily sampled
       data.
         Caution: Do not use rtos_delay here in the idle task. An attempt to suspend the
       idle task definitely causes a crash. */
    delay(800);

} /* End of loop */




