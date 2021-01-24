/**
 * @file gsl_systemLoad.c
 *   Measure the current CPU load of the RTuinOS application. A support function for the
 * use with RTuinOS.
 *   @see uint8_t gsl_getSystemLoad(void)
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
 */
/* Module interface
 *   gsl_getSystemLoad
 * Local functions
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos_assert.h"
#include "gsl_systemLoad.h"


/*
 * Defines
 */
 
/** Averaging time window of gsl_getSystemLoad(void) in ms. The window should contain a full
    cycle of tasks activations. */
#define TI_WINDOW_LEN   1000ul
 

/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
 
/*
 * Data definitions
 */
 
 
/*
 * Function implementation
 */

/**
 * A diagnostic function, which can be used to estimate the current system load. The
 * function executes some test code of known CPU execution time and relates this known time
 * to the measured, actually elapsed world time. The function needs to be called from the
 * idle task only. Now the mentioned ratio is a measure for the system load: The less the
 * system idles the longer it'll take to execute the test code.\n
 *   On principal, the execution time of the function depends on the system load and can
 * raise to infinite if the load approaches 100%. This effect is lowered by splitting the
 * test code in pieces and ending the measurement if enough pieces have been executed to
 * get the desired resolution. Nonetheless, the effect remains and no upper boundary can
 * be given for the execution time of this function if the system load is close to 100%.\n
 *   Normally, the execution time of this function is about 1 second. This long time is
 * needed to have a sufficient averaging effect; typically the CPU consumption by the tasks
 * is quite irregular due to the complex task activation pattern of the scheduler. The
 * averaging time can be changed at compile time, please see macro #TI_WINDOW_LEN.
 *   @return
 * The system load is returned with a resolution 0.5%, i.e. as an integer number in the
 * range 0..200.
 *   @remark
 * The function execution takes a long time (above 1s). It must only be called from the
 * idle task and only if there are no other essential jobs for the idle task.
 *   @remark
 * The function will never return if the CPU load is 100% and it might take an arbitrary
 * long time to return if it is close to 100%. The calling code must anticipate this, e.g.
 * by presetting the result variable with 100% prior to calling this function.
 */

uint8_t gsl_getSystemLoad()
{
#define TI_STEP         16u     /* Should be a power of 2, need to <= 65 */

    uint32_t tiStart, tiEnd, tiEndMin;
    uint8_t step;

    step = 0;
    tiStart = millis();
    tiEndMin = tiStart + TI_WINDOW_LEN;
    do
    {
        /* Count the steps. */
        ++ step;

/* The implementation of delayMicroseconds is buggy. It permits a uint16_t input but will
   shift the parameter twice to the left (after subtracting one). An overrun occurs for
   numbers greater than 0x4000. A limitation of the parameter is however not documented. */
#if TI_STEP*1000u > 0x4000 
# error TI_STEP too large, we run into a bug in delayMicroseconds
#endif

        /* One step is exactly 16 ms of code execution time - regardless of how long this
           will take because of interruptions by ISRs and other tasks.
             -10: An arbitrary but conservatively chosen compensation of the overhead
           produced by the surrounding loop. */
        delayMicroseconds(TI_STEP * 1000u - 10);

        /* The number of steps is chosen so, that the total measurement time is at least
           100 ms. Otherwise the resolution of the measurement becomes too bad. */
        tiEnd = millis();
    }
    while(tiEnd < tiEndMin);
    
    /* If the system is idle the number of steps reaches its maximum of about 63. */
    ASSERT(step <= (TI_WINDOW_LEN / TI_STEP + 1));
    
    tiEnd -= tiStart;       /* tiEnd now holds the measured, elapsed time. */
    tiStart = step*TI_STEP; /* tiStart now holds the consumed CPU time. */
    
    if(tiEnd >= 200*tiStart)
    {
        /* If the elapsed time is too large, we can limit the result by rounding to 100%.
           The chosen boundary is to round all above 99.5% to 100%. Since we use 0.5% as
           resolution of the result, the rounded range can anyway not be distinguished from
           100%. */
        return 200; 
    }
    else if(tiEnd <= tiStart)
    {
        /* Theoretically, the consumed CPU time can't be greater than the measured, elapsed
           time. However, we have some limitations of accuracy, e.g. the accuracy of the
           delay function, so that this rule could be hurt. We need a limiting statement to
           safely avoid an overrun. */
        return 0;
    }    
    else
    {
        /* Normal situation. The system load is all the time, which was not spent in the
           idle task inside this test routine in relation to the elapsed world time, or
           (Elapsed time - Consumed CPU time)/Elapsed time respectively. This is taken by
           200 to get the desired resolution of 0.5%, which lets the result safely fit into
           a uint8_t. */
        return 200 - (uint8_t)(200*tiStart/tiEnd);
    }
#undef TI_WINDOW_LEN
#undef TI_STEP      
} /* End of gsl_getSystemLoad */




