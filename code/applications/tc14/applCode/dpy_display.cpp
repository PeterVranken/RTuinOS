/**
 * @file dpy_display.cpp
 *   A class around the LCD display object grants properly synchronized access to the LCD
 * from different RTuinOS tasks. The display becomes a shared resource.\n
 *   The class only offers some application specific, formatted print functions. No other
 * information than anticpated by these functions can be written to the display. With other
 * words, the entire layout design of the application output is controlled by this module.
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
 *   dpy_display_t::dpy_display_t
 *   dpy_display_t::printBackground
 *   dpy_display_t::printAdcInput
 *   dpy_display_t::printTime
 *   dpy_display_t::printVoltage
 *   dpy_display_t::printCpuLoad
 * Local functions
 *   dpy_display_t::acquireMutex
 *   dpy_display_t::releaseMutex
 */

/*
 * Include files
 */

#include <Arduino.h>
#include <LiquidCrystal.h>

#include "rtos.h"
#include "rtos_assert.h"
#include "aev_applEvents.h"
#include "dpy_display.h"


/*
 * Defines
 */


/*
 * Local type definitions
 */


/*
 * Local prototypes
 */


/*
 * Data definitions
 */

/** The one and only display object. */
dpy_display_t dpy_display;


/*
 * Function implementation
 */


/**
 * Class construtor. Only a single instance of the class must exist in an application,
 * therefore this constructor must never be used.
 *   @remark
 *   The configuration of the underlaying class LiquidCrystal, particularly the supported
 * pins, is done hardcoded in the implementation of this constructor.
 */

dpy_display_t::dpy_display_t()
    : LiquidCrystal(8, 9, 4, 5, 6, 7)
{
    /* Initialize LCD shield. */
    begin(16, 2);

} /* End of dpy_display_t::dpy_display_t */



/**
 * Print a standard greeting text. This function should be called immediately after reset.\n
 *   The function must not be called at run time, when concurrent tasks try to access the
 * display. This function will not acquire the mutex for safe access of the display.
 */

void dpy_display_t::printGreeting()
{
    char lcdLine[16+1];
#ifdef DEBUG
    int noChars =
#endif
    sprintf(lcdLine, "%-16s", "RTuinOS " RTOS_RTUINOS_VERSION);
    ASSERT(noChars < (int)sizeof(lcdLine));
    setCursor(/* col */ 0, /* row */ 0);
    print(lcdLine);

    lcdLine[sizeof(lcdLine)-1] = '\0';
    uint8_t u;
    for(u=0; u<14; ++u)
    {
        memset(lcdLine, ' ', sizeof(lcdLine)-1);
        lcdLine[u] = lcdLine[u+1] = lcdLine[u+2] = '*';
        setCursor(/* col */ 0, /* row */ 1);
        print(lcdLine);

        /* rtos_delay() must not be used here: A prerequiste of this function is that the
           RTOS kernel is not yet running. */
        delay(200 /* ms */);
    }
    memset(lcdLine, ' ', sizeof(lcdLine)-1);
    setCursor(/* col */ 0, /* row */ 1);
    print(lcdLine);

} /* End of dpy_display_t::printGreeting */




/**
 * Print the invariant parts of the display layout. This function must be called once at
 * the beginning, prior to the call of any of the other formatted print commands.\n
 *   The function must not be called at run time, when concurrent tasks try to access the
 * display. This function will not acquire the mutex for safe access of the display.
 */

void dpy_display_t::printBackground()
{
    char lcdLine[16+1];
#ifdef DEBUG
    int noChars =
#endif
    /* @todo "BG" as part of the background is an ugly work around the problem, that we
       don't have a chance to write the actual initial input selection: The call of the
       display function requires to know, what the currently selected input is and this
       information is encapsulated and hidden in module adc. We could add a get function to
       that modules interface ... */
    sprintf(lcdLine, "ADC: BG         ");
    ASSERT(noChars < (int)sizeof(lcdLine));
    setCursor(/* col */ 0, /* row */ 0);
    print(lcdLine);

    sprintf(lcdLine, "      V        %%");
    ASSERT(noChars < (int)sizeof(lcdLine));
    setCursor(/* col */ 0, /* row */ 1);
    print(lcdLine);

} /* End of dpy_display_t::printBackground */




/**
 * Print the currently selected ADC input (or multiplexer input respectively).\n
 *   The function can be called only at run time from an \b RTuinOS task other than the
 * idle task. (It acquires the mutex for safe access of the display, which is forbidden for
 * the idle task.)
 *   @param idxInput
 * The index of the selected ADC input. The range is [-1..15]. (A value of -1 can be passed
 * to indicate that none of the external multiplexer inputs is currently chosen.) Exceeding
 * the range will lead to a runtime error!
 */

void dpy_display_t::printAdcInput(int8_t idxInput)
{
    char lcdString[2+1];
    if(idxInput >= 0  &&  idxInput <= 15)
    {
#ifdef DEBUG
        int noChars =
#endif
        sprintf(lcdString, "%02u", idxInput);
        ASSERT(noChars < (int)sizeof(lcdString));
    }
    else
        lcdString[0] = 'B', lcdString[1] = 'G', lcdString[2] = '\0';

    /* Get access to the display, or wait until anybody else has finished respectively. A
       timeout has been defined which should never elapse, but who knows. In case it
       should, we simply deny printing. */
    if(acquireMutex())
    {
        setCursor(/* col */ 5, /* row */ 0);
        print(lcdString);

        /* And release the mutex as soon as possible after writing to the underlaying class has
           been done. */
        releaseMutex();
    }
} /* End of dpy_display_t::printAdcInput */




/**
 * Print the current time.\n
 *   The function can be called only at run time from an \b RTuinOS task other than the
 * idle task. (It acquires the mutex for safe access of the display, which is forbidden for
 * the idle task.)
 *   @param hour
 * The hours of the current time, range 0..23. Exceeding the range will lead to a runtime
 * error!
 *   @param min
 * The minutes of the current time, range 0..59. Exceeding the range will lead to a runtime
 * error!
 *   @param sec
 * The seconds of the current time, range 0..59. Exceeding the range will lead to a runtime
 * error!
 */

void dpy_display_t::printTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    char lcdString[8+1];
#ifdef DEBUG
    int noChars =
#endif
    sprintf(lcdString, "%02u:%02u:%02u", hour, min, sec);
    ASSERT(noChars < (int)sizeof(lcdString));

    /* Get access to the display, or wait until anybody else has finished respectively. A
       timeout has been defined which should never elapse, but who knows. In case it
       should, we simply deny printing. */
    if(acquireMutex())
    {
        /* "16-sizeof" means to display right aligned. */
        setCursor(/* col */ 16-(sizeof(lcdString)-1), /* row */ 0);
        print(lcdString);

        /* And release the mutex as soon as possible after writing to the underlaying class has
           been done. */
        releaseMutex();
    }
} /* End of dpy_display_t::printTime */




/**
 * Print the measured voltage.\n
 *   The function can be called only at run time from an \b RTuinOS task other than the
 * idle task. (It acquires the mutex for safe access of the display, which is forbidden for
 * the idle task.)
 *   @param voltage
 * The value to print. Scaling is 1 V. The range is [0..10). Exceeding the range will lead
 * to a runtime error!
 */

void dpy_display_t::printVoltage(float voltage)
{
    char lcdString[5+1];
#ifdef DEBUG
    int noChars =
#endif
    sprintf(lcdString, "%5.3f", (double)voltage);
    ASSERT(noChars < (int)sizeof(lcdString));

    /* Get access to the display, or wait until anybody else has finished respectively. A
       timeout has been defined which should never elapse, but who knows. In case it
       should, we simply deny printing. */
    if(acquireMutex())
    {
        setCursor(/* col */ 0, /* row */ 1);
        print(lcdString);

        /* And release the mutex as soon as possible after writing to the underlaying class has
           been done. */
        releaseMutex();
    }
} /* End of dpy_display_t::printVoltage */




/**
 * Print the current CPU load.\n
 *   The function can be called only at run time from an \b RTuinOS task other than the
 * idle task. (It acquires the mutex for safe access of the display, which is forbidden for
 * the idle task.)
 *   @param cpuLoad
 * The value to print. Scaling is 0.5%, a passed value of 200 will result in a displayed
 * value of 100%. The range is [0..200].
 */

void dpy_display_t::printCpuLoad(uint8_t cpuLoad)
{
    char lcdString[5+1];
#ifdef DEBUG
    int noChars =
#endif
    sprintf(lcdString, "%5.1f", (double)cpuLoad/2.0);
    ASSERT(noChars < (int)sizeof(lcdString));

    /* Get access to the display, or wait until anybody else has finished respectively. A
       timeout has been defined which should never elapse, but who knows. In case it
       should, we simply deny printing. */
    if(acquireMutex())
    {
        setCursor(/* col */ 10, /* row */ 1);
        print(lcdString);

        /* And release the mutex as soon as possible after writing to the underlaying class
           has been done. */
        releaseMutex();
    }
} /* End of dpy_display_t::printCpuLoad */





/**
 * At runtime, when the \b RTuinOS tasks compete for the display, strict synchronization is
 * required. All requests to write to the display are serialized by an \b RTuinOS mutex.
 * This private method is called by all the public print methods immediately before the
 * first access to the underlaying class LiquidCrystal.\n
 *   This method blocks until the mutex is available or the wait timeout elapses.
 *   @return
 * The function uses a timeout when waiting for the mutex. If the mutex was got in the
 * defined time span the function returns true and the calling task owns the display. If it
 * returns false, the calling task must not access the display.\n
 *   @remark
 * The time span is defined such that the function will always return true in case of
 * correct usage of this class. An assertion will otherwise fire in debug compilation.
 *   @remark
 * If and only if this function returns true the mutex must be released again as soon as
 * possible.
 *   @see void dpy_display_t::releaseMutex(void)
 */

inline boolean dpy_display_t::acquireMutex()
{
    uint16_t gotEvtVec = rtos_waitForEvent( EVT_MUTEX_LCD | RTOS_EVT_DELAY_TIMER
                                          , /* all */ false
                                          , 1 /* unit is 2 ms */
                                          );

    /* Normally, no task will block the display longer than 2ms and the debug compilation
       double-checks this. Production code can nonetheless be implemented safe; in case it
       can simply skip display operation. */
    ASSERT(gotEvtVec == EVT_MUTEX_LCD);
    return (gotEvtVec & EVT_MUTEX_LCD) != 0;

} /* End of dpy_display_t::acquireMutex */



/**
 * Release the display; return the mutex to the \b RTuinOS system that had been acquired
 * shortly before by a successful call of boolean dpy_display_t::acquireMutex(void).\n
 *   @see boolean dpy_display_t::acquireMutex(void)
 */

inline void dpy_display_t::releaseMutex()
{
    /* Release the mutex. */
    rtos_sendEvent(EVT_MUTEX_LCD);

} /* End of dpy_display_t::releaseMutex */








