/**
 * @file clk_clock.cpp
 *   Implementation of a real time clock for a sample task.
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
 * Local functions
 */

/*
 * Include files
 */

#include <Arduino.h>

#include "rtos.h"
#include "rtos_assert.h"
#include "aev_applEvents.h"
#include "dpy_display.h"
#include "clk_clock.h"


/*
 * Defines
 */
 
/** The standard RTuinOS clock tic on the Arduino Mega 2560 board is 1/(16MHz/64/510) =
    51/25000s. We add 51 in each tic and have the next second when we reach 25000. This
    permits a precise implementation of the real time clock even with 16 Bit integer
    arithmetics. See #CLOCK_TIC_DENOMINATOR also. */
#define CLOCK_TIC_NUMERATOR     51

/** Denominator of ratio, which implements the clock's task rate. See #CLOCK_TIC_NUMERATOR
    for details. */
#define CLOCK_TIC_DENOMINATOR   (25000-(CLOCK_TIC_DENOMINATOR_TRIM_TERM))
 
/** Trim term for clock: By long term observation a correction has been figured out, which
    makes the clock significantly more accurate. The term is defined as (negative) addend
    to the denominator #CLOCK_TIC_DENOMINATOR. It is device dependent. Starting point on a
    new hardware device should be 0. Positive values advance the clock. */
#define CLOCK_TIC_DENOMINATOR_TRIM_TERM 58

/*
 * Local type definitions
 */
 
 
/*
 * Local prototypes
 */
 
 
/*
 * Data definitions
 */

/** Counter of seconds. The value is written without access synchronization code. The time
    information clk_noHour, clk_noMin, clk_noSec can be safely and consistently read only by
    a task of same or lower priority and using a critical section. */ 
volatile uint8_t clk_noSec = 0;

/** Counter of minutes. The value is written without access synchronization code. The time
    information clk_noHour, clk_noMin, clk_noSec can be safely and consistently read only by
    a task of same or lower priority and using a critical section. */
volatile uint8_t clk_noMin = 0;

/** Counter of hours. The value is written without access synchronization code. The time
    information clk_noHour, clk_noMin, clk_noSec can be safely and consistently read only by
    a task of same or lower priority and using a critical section.*/ 
volatile uint8_t clk_noHour = 20;

/** Input to the module: Recognized button-down events, which are used to adjust the clock
    ahead. The value is read/modified using a critical section. */
volatile uint8_t clk_noButtonEvtsUp = 0;

/** Input to the module: Recognized button-down events, which are used to adjust the clock
    towards lower time designations. The value is read/modified using a critical section. */
volatile uint8_t clk_noButtonEvtsDown = 0;


/* Accumulator for task tics which generates a precise one second clock. */
static uint16_t _noTaskTics = 0;


/*
 * Function implementation
 */

/**
 * The regular task function of the real time clock. Has to be called every 100th tic of
 * the RTuinOS system time, which is running in its standard copnfiguration of about 2 ms a
 * tic.
 */
void clk_taskRTC()
{
    /* Global interface of module: Do we have to adjust the time because of a user
       interaction? This code is kept very simple: Any button down event will advance
       or retard the clock by five minutes.
         The interface is written by the user interaction task, which has a higher
       priority. We need to apply a critical section for consistent read and safe
       modification. */
    cli();
    int8_t deltaTime = clk_noButtonEvtsUp - clk_noButtonEvtsDown;
    clk_noButtonEvtsUp   =
    clk_noButtonEvtsDown = 0;
    sei();
    
    /* Adjust time. */
    boolean doDisplay = deltaTime != 0;
    if(doDisplay)
    {
        /* Reset the second and fraction of a second counters. */
        clk_noSec   = 0;
        _noTaskTics = 0;
        
        while(deltaTime > 0)
        {
            -- deltaTime;
            if((clk_noMin+=5) > 59)
            {
                clk_noMin -= 60;
                if(++clk_noHour > 23)
                    clk_noHour = 0;
            }
        }
        while(deltaTime < 0)
        {
            /* By defining the downwards operation not strictly inverse to upwards we can
               reach all times, not just the multiples of five. Maybe not the kind of thing
               one would expect and no true alternative to a state machine, which begins to
               auto-repeat the key event after a while, but this is just a simple
               demonstration of RTuinOS, not a high-end application. */
            ++ deltaTime;
            if((clk_noMin-=4) > 59)
            {
                clk_noMin += 60;
                if(--clk_noHour > 23)
                    clk_noHour = 23;
            }
        }
    }
    else
    {
        /* Advance the clock by one tic. */
        _noTaskTics += CLK_TASK_TIME_RTUINOS_STANDARD_TICS*CLOCK_TIC_NUMERATOR;

    } /* if(A button has been touched to adjust the clock?) */    
    
    /* Carry ripple. */
    if(_noTaskTics >= CLOCK_TIC_DENOMINATOR)
    {
        _noTaskTics -= CLOCK_TIC_DENOMINATOR;
        
        /* We display hh:mm:ss, so a change of the seconds leads to a write into the
           display. */
        doDisplay = true;
        
        if(++clk_noSec > 59)
        {
            clk_noSec = 0;
            if(++clk_noMin > 59)
            {
                clk_noMin = 0;
                if(++clk_noHour > 23)
                    clk_noHour = 0;
            }
        }       
    } /* End if(A second has elapsed?) */
    
    /* Display because visible information has changed. */
    if(doDisplay)    
        dpy_display.printTime(clk_noHour, clk_noMin, clk_noSec);

} /* End of clk_taskRTC */