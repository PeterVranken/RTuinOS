#ifndef RTOS_INCLUDED
#define RTOS_INCLUDED
/**
 * @file rtos.h
 * Definition of global interface of module rtos.c
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
 */

/*
 * Include files
 */
 
#include "Arduino.h"
#include "rtos.config.h"


/*
 * Defines
 */

/** Version string of RTuinOS. */
#define RTOS_RTUINOS_VERSION    "1.0"

/** Startup message for RTuinOS applications. */
#define RTOS_RTUINOS_STARTUP_MSG                                                        \
    "RTuinOS " RTOS_RTUINOS_VERSION " for Arduino 1.0.5\n"                              \
    "Copyright (C) 2012-2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)\n"           \
    "This is free software; see the source for copying conditions. There is NO\n"       \
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

/** Switch to make feature selecting defines readable. Here: Feature is enabled. */
#define RTOS_FEATURE_ON     1
/** Switch to make feature selecting defines readable. Here: Feature is disabled. */
#define RTOS_FEATURE_OFF    0


/** Derive a switch telling whether events of type semaphore are in use. */
#if RTOS_NO_SEMAPHORE_EVENTS > 0
# define RTOS_USE_SEMAPHORE RTOS_FEATURE_ON
#else
# define RTOS_USE_SEMAPHORE RTOS_FEATURE_OFF
#endif


/** Derive a switch telling whether events of type mutex are in use. */
#if RTOS_NO_MUTEX_EVENTS > 0
# define RTOS_USE_MUTEX RTOS_FEATURE_ON
#else
# define RTOS_USE_MUTEX RTOS_FEATURE_OFF
#endif


/* Some global, general purpose events and the two timer events. Used to specify the
   resume condition when suspending a task.
     Conditional definition: If the application defines an interrupt which triggers an
   event, the same event gets a deviating name. */
/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 0
# define RTOS_EVT_SEMAPHORE_00      (0x0001u<<0)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 0
# define RTOS_EVT_MUTEX_00          (0x0001u<<0)
#else
# define RTOS_EVT_EVENT_00          (0x0001u<<0)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 1
# define RTOS_EVT_SEMAPHORE_01      (0x0001u<<1)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 1
# define RTOS_EVT_MUTEX_01          (0x0001u<<1)
#else
# define RTOS_EVT_EVENT_01          (0x0001u<<1)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 2
# define RTOS_EVT_SEMAPHORE_02      (0x0001u<<2)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 2
# define RTOS_EVT_MUTEX_02          (0x0001u<<2)
#else
# define RTOS_EVT_EVENT_02          (0x0001u<<2)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 3
# define RTOS_EVT_SEMAPHORE_03      (0x0001u<<3)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 3
# define RTOS_EVT_MUTEX_03          (0x0001u<<3)
#else
# define RTOS_EVT_EVENT_03          (0x0001u<<3)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 4
# define RTOS_EVT_SEMAPHORE_04      (0x0001u<<4)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 4
# define RTOS_EVT_MUTEX_04          (0x0001u<<4)
#else
# define RTOS_EVT_EVENT_04          (0x0001u<<4)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 5
# define RTOS_EVT_SEMAPHORE_05      (0x0001u<<5)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 5
# define RTOS_EVT_MUTEX_05          (0x0001u<<5)
#else
# define RTOS_EVT_EVENT_05          (0x0001u<<5)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 6
# define RTOS_EVT_SEMAPHORE_06      (0x0001u<<6)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 6
# define RTOS_EVT_MUTEX_06          (0x0001u<<6)
#else
# define RTOS_EVT_EVENT_06          (0x0001u<<6)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 7
# define RTOS_EVT_SEMAPHORE_07      (0x0001u<<7)
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 7
# define RTOS_EVT_MUTEX_07          (0x0001u<<7)
#else
# define RTOS_EVT_EVENT_07          (0x0001u<<7)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS > 8
# error No more than eight semaphores are permitted
#elif RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 8
# define RTOS_EVT_MUTEX_08          (0x0001u<<8)
#else
# define RTOS_EVT_EVENT_08          (0x0001u<<8)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 9
# define RTOS_EVT_MUTEX_09          (0x0001u<<9)
#else
# define RTOS_EVT_EVENT_09          (0x0001u<<9)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 10
# define RTOS_EVT_MUTEX_10          (0x0001u<<10)
#else
# define RTOS_EVT_EVENT_10          (0x0001u<<10)
#endif

/** General purpose event, posted explicitly by rtos_sendEvent. */
#if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 11
# define RTOS_EVT_MUTEX_11          (0x0001u<<11)
#else
# define RTOS_EVT_EVENT_11          (0x0001u<<11)
#endif

/* The name of the next event depends on the configuration of RTuinOS. */
#if RTOS_USE_APPL_INTERRUPT_01 == RTOS_FEATURE_ON
/** This event is posted by the application defined ISR 01.
      @remark The expression here is passed on to the assembler as is. It needs to be
    compatible with both, compiler and assembler. Type casts, type post fixes, nested
    macros etc. must not be used. */
# define RTOS_EVT_ISR_USER_01       (0x0001<<12)
# if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 12
#  error Too many semaphores and mutexes specified. The limit is 12 when using two application interrupts
# endif
#else
/** General purpose event, posted explicitly by rtos_sendEvent. */
# if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 12
#  define RTOS_EVT_MUTEX_12         (0x0001u<<12)
# else
#  define RTOS_EVT_EVENT_12         (0x0001u<<12)
# endif
#endif

/* The name of the next event depends on the configuration of RTuinOS. */
#if RTOS_USE_APPL_INTERRUPT_00 == RTOS_FEATURE_ON
/** This event is posted by the application defined ISR 00.
      @remark The expression here is passed on to the assembler as is. It needs to be
    compatible with both, compiler and assembler. Type casts, type post fixes, nested
    macros etc. must not be used. */
# define RTOS_EVT_ISR_USER_00       (0x0001<<13)
# if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 13
#  error Too many semaphores and mutexes specified. The limit is 13 when using a single application interrupt
# endif
#else
/** General purpose event, posted explicitly by rtos_sendEvent. */
# if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 13
#  define RTOS_EVT_MUTEX_13         (0x0001u<<13)
# else
#  define RTOS_EVT_EVENT_13         (0x0001u<<13)
# endif
#endif

#if RTOS_NO_SEMAPHORE_EVENTS + RTOS_NO_MUTEX_EVENTS > 14
# error Too many semaphores and mutexes specified. The limit is 14 in total
#endif

/** Real time clock is elapsed for the task. */
#define RTOS_EVT_ABSOLUTE_TIMER     (0x0001u<<14)
/** The relative-to-start clock is elapsed for the task */
#define RTOS_EVT_DELAY_TIMER        (0x0001u<<15)


/** The system timer frequency as floating point constant. The unit is Hz.\n
      The value is derived from  #RTOS_TIC, which is about 2 ms in the RTuinOS standard
    configuration. The macro is defined in the configuration file rtos.config.h as it might
    be subject to changes by the application. */
#define RTOS_TIC_FREQUENCY (1.0/(RTOS_TIC))

/** The scale factor between RTuinOS' system timer tic and Arduinos \a millis() as a
    floating point constant. Same as tic period in unit ms.
      The value is derived from  #RTOS_TIC, which is about 2 ms in the RTuinOS standard
    configuration. The macro is defined in the configuration file rtos.config.h as it might
    be subject to changes by the application. */
#define RTOS_TIC_MS ((RTOS_TIC)*1000.0)


/** Function prototype decoration which declares a function of RTuinOS just a default
    implementation of the required functionality. The application code can redefine the
    function and override the default implementation.\n
      We use this type decoration for the initialization of the system timer interrupt --
    an RTuinOS application may use any other interrupts source than the default
    TIMER2_OVF. */
#define RTOS_DEFAULT_FCT __attribute__((weak))

/** Function prototype decoration which ensures that a function is implemented without stack
    frame generating machine code. The machine code starts with the implementation of the
    visible code lines of the function body. If used, one has to ensure, that the function
    body does not require a stack frame or has to set it up himself by some introductory
    inline assembly code.\n
      We use this type decoration for all software interrupts (all API functions which can
    cause a task switch). */
#define RTOS_NAKED_FCT __attribute__((naked, noinline))

/** Function prototype decoration which ensures that a function is generated by the compiler
    as such, but neither inlined nor removed from the code.\n
      We use this type decoration for all functions called from a software interrupt. If such
    a function was inlined it could cause the (hazardous) need for a stack frame in the
    calling function which implements the software interrupt. */
#define RTOS_TRUE_FCT __attribute__((used, noinline))

/** A data type decoration to place constant data in the program memory. Mainly used for
    the RTuinOS startup message.\n
      See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734 why not simply using PROGMEM
    for such declarations. */
#define RTOS_PROGMEM_SECTION __attribute__((section(".progmem.rtuinos")))


/**
 * Delay a task without looking at other events. \a rtos_delay(delayTime) is identical to
 * \a rtos_waitForEvent(#RTOS_EVT_DELAY_TIMER, false, delayTime), i.e. \a eventMask's only
 * set bit is the delay timer event.\n
 *   @param delayTime
 * The duration of the delay in the unit of the system time. The permitted range is
 * 0..max(uintTime_t). The resolution of any timing operation is the tic of the system
 * timer. A delay time of \a n may actually mean any delay in the range \a n .. \a n+1
 * tics.
 *   @remark
 * This method is one of the task suspend commands. It must not be used by the idle task,
 * which can't be suspended. A crash would be the immediate consequence.
 *   @remark
 * This function actually is a macro calling \a rtos_waitForEvent using fixed parameters.
 *   @see rtos_waitForEvent
 */
#define rtos_delay(delayTime)                                               \
                rtos_waitForEvent(RTOS_EVT_DELAY_TIMER, false, delayTime)




/**
 * Suspend the current task (i.e. the one which invokes this method) until a specified
 * point in time.\n
 *   Although specified as a increment in time, the time is meant absolute. The meant time
 * is the time specified at the last recent call of this function by this task plus the now
 * specified increment. This way of specifying the desired time of resume supports the
 * intended use case, which is the implementation of regular real time tasks: A task will
 * suspend itself with a constant time value at the end of the infinite loop which contains
 * its functional code. This (fixed) time value becomes the sample time of the task. This
 * behavior is opposed to a delay or sleep function: The execution time of the task is no
 * time which additionally elapses between two task resumes.\n
 *   The idle task can't be suspended. If it calls this function a crash would be the
 * immediate result.
 *   @return
 * The event mask of resuming events is returned. Since no combination with other events
 * than the elapsed system time is possible, this will always be #RTOS_EVT_ABSOLUTE_TIMER.
 *   @param deltaTimeTillResume
 * \a deltaTimeTillResume specifies a time in the future at which the task will become due
 * again. To support the most relevant use case of this function, the implementation of
 * regular real time tasks, the time designation is relative. It refers to the last recent
 * absolute time at which this task had been resumed. This time is defined by the last
 * recent call of either this function or \a rtos_waitForEvent with parameter
 * #RTOS_EVT_ABSOLUTE_TIMER. In the very first call of the function it refers to the point
 * in time the task was started.\n
 *   The value of \a deltaTimeTillResume must neither be 0 nor exceed half the range of
 * the data type configured for the system time. Otherwise a false task overrun recognition
 * and bad task timing could result. Please, refer to the RTuinOS manual for details.
 *   @remark
 * This function actually is a macro calling \a rtos_waitForEvent using fixed parameters.
 *   @see rtos_waitForEvent
 */
#define rtos_suspendTaskTillTime(/* uintTime_t */ deltaTimeTillResume)      \
    rtos_waitForEvent( /* eventMask */ RTOS_EVT_ABSOLUTE_TIMER              \
                     , /* all */       false                                \
                     , /* timeout */   deltaTimeTillResume                  \
                     )


/**
 * Alias of function void rtos_sendEvent(uint16_t). Post a set of events to the suspended
 * tasks. Suspend the current task if the events resume another task of higher priority.
 *   @param eventVec
 * The set of events to be posted.
 *   @remark
 * This macro is deprecated. Use \a rtos_sendEvent instead. 
 *   @remark
 * This macro exists for backward compatibility only: The function \a rtos_sendEvent had
 * been named \a rtos_setEvent in the first release of RTuinOS, version 0.9.
 *   @see void rtos_sendEvent(uint16_t)
 */
#define /* void */ rtos_setEvent(/* uint16_t */ eventVec) rtos_sendEvent(eventVec)


/*
 * Global type definitions
 */

/** The type of any task.\n
      The function is of type void; it must never return.\n
      The function takes a single parameter. It is the event vector of the very event
    combination which made the task initially run. Typically this is just the delay timer
    event. */
typedef void (*rtos_taskFunction_t)(uint16_t postedEventVec);


/*
 * Global data declarations
 */

/** The RTuinOS startup message is placed in the flash ROM. See #RTOS_RTUINOS_STARTUP_MSG
    for the definition of string contents. */
extern RTOS_PROGMEM_SECTION const char rtos_rtuinosStartupMsg[];

#if RTOS_USE_SEMAPHORE == RTOS_FEATURE_ON
/** All declared semaphores are held in an array of counters.\n
      The type of the counter depends on the maximum number of pooled resources managed by
    a semaphore and is configurable, please see rtos.config.h for the typedef of
    typeSemaphore_t.\n
      The array is declared extern and it is defined by the application code. This way,
    it's most easy to initialize the semaphore counters. Any value is possible as a start
    value, this depends only on the application. */
extern uintSemaphore_t rtos_semaphoreAry[RTOS_NO_SEMAPHORE_EVENTS];
#endif


/*
 * Global prototypes
 */

/* Initialze all application parameters of one task. To be called for each of the tasks in
   setup(). */
void rtos_initializeTask( uint8_t idxTask
                        , rtos_taskFunction_t taskFunction
                        , uint8_t prioClass
#if RTOS_ROUND_ROBIN_MODE_SUPPORTED == RTOS_FEATURE_ON
                        , uintTime_t timeRoundRobin
#endif
                        , uint8_t * const pStackArea
                        , uint16_t stackSize
                        , uint16_t startEventMask
                        , boolean startByAllEvents
                        , uintTime_t startTimeout
                        );

/** Configure and enable the interrupt which clocks the system time of RTuinOS. This
    function has a default implementation, the application may but need not to implement
    it.\n
      If the application decides to set up its specific system timer interrupt, it'll
    probably have to alter also the interrupt vector name, see
    #RTOS_ISR_SYSTEM_TIMER_TIC.\n
      If the application decides to set up its specific system timer interrupt, it'll
    probably have to state the new system clock frequency, see #RTOS_TIC. */
void rtos_enableIRQTimerTic(void);

#if RTOS_USE_APPL_INTERRUPT_00 == RTOS_FEATURE_ON
/** An application supplied callback, which contains the code to set up the hardware to
    generate application interrupt 0. */
extern void rtos_enableIRQUser00(void);
#endif

#if RTOS_USE_APPL_INTERRUPT_01 == RTOS_FEATURE_ON
/** An application supplied callback, which contains the code to set up the hardware to
    generate application interrupt 1. */
extern void rtos_enableIRQUser01(void);
#endif

/* Initialization of the internal data structures of RTuinOS and start of the timer
   interrupt (see void rtos_enableIRQTimerTic(void)). This function does not return but
   forks into the configured tasks.
     This function is not called by the application (but only from main()). */
void rtos_initRTOS(void);

/* Post a set of events to the suspended tasks. Suspend the current task if the events
   resume another task of higher priority. */
void rtos_sendEvent(uint16_t eventVec);

/* Suspend task until a combination of events appears or a timeout elapses. */
uint16_t rtos_waitForEvent(uint16_t eventMask, boolean all, uintTime_t timeout);

/* How often could a real time task not be reactivated timely? */
uint8_t rtos_getTaskOverrunCounter(uint8_t idxTask, boolean doReset);

/* How many bytes of the stack of a task are still unused? */
uint16_t rtos_getStackReserve(uint8_t idxTask);

#endif  /* RTOS_INCLUDED */
