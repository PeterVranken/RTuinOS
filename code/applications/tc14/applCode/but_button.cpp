/**
 * @file but_button.cpp
 *   Evaluate the button status and implement a state machine that represents the user
 * interface.
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
 *   but_onNewButtonVoltage
 * Local functions
 *   decodeLCDButton
 */

/*
 * Include files
 */

#include <Arduino.h>

#include "rtos_assert.h"
#include "adc_analogInput.h"
#include "clk_clock.h"
#include "but_button.h"


/*
 * Defines
 */


/*
 * Local type definitions
 */

/** The buttons are enumerated. */
typedef enum { btnInvalid
             , btnRight
             , btnUp
             , btnDown
             , btnLeft
             , btnSelect
             , btnNone
             } enumButton_t;


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
 * Transform the ADC value into the index of the pressed button. All buttons of the LCD
 * shield shortcut a voltage divider at different resistor values so that the output
 * voltage of the divider depends on the currently pressed button. See e.g.
 * http://sainsmart.com/zen/documents/20-011-901/schematic.pdf.
 *   @param adcVal
 * The measured analog value of analog pin 0, which the buttons of the LCD shield are
 * connetced to. The voltage at this pin is determined by the currently pressed button.\n
 *   The passed value is the #ADC_NO_AVERAGED_SAMPLES times accumulated raw ADC value.
 */

static enumButton_t decodeLCDButton(uint16_t adcVal)
{
#define RATIO_0 /* RIGHT  */ 0.0
#define RATIO_1 /* UP     */ (330.0/(330.0+2000.0))
#define RATIO_2 /* DOWN   */ ((330.0+620.0)/(330.0+620.0+2000.0))
#define RATIO_3 /* LEFT   */ ((330.0+620.0+1000.0)/(330.0+620.0+1000.0+2000.0))
#define RATIO_4 /* SELECT */ ((330.0+620.0+1000.0+3300.0)/(330.0+620.0+1000.0+3300.0+2000.0))
#define RATIO_5 /* NONE   */ 1.0
#define THRESHOLD(n,n1) (uint16_t)(((RATIO_##n1 + RATIO_##n) / 2.0)                         \
                                   * ADC_NO_AVERAGED_SAMPLES * 0x0400ul)

    if(adcVal > THRESHOLD(4,5))
        return btnNone;
    else if(adcVal > THRESHOLD(3,4))
        return btnSelect;
    else if(adcVal > THRESHOLD(2,3))
        return btnLeft;
    else if(adcVal > THRESHOLD(1,2))
        return btnDown;
    else if(adcVal > THRESHOLD(0,1))
        return btnUp;
    else
        return btnRight;

#undef RATIO_0
#undef RATIO_1
#undef RATIO_2
#undef RATIO_3
#undef RATIO_4
#undef RATIO_5
#undef THRESHOLD
} /* End of decodeLCDButton */



/**
 * The entry into the state machine that evaluates the buttons and represents the user
 * interface is the notification of the input voltage at the analog input which all buttons
 * are connected to. The buttons shortcut a voltage divider at individual paths and can be
 * indentified because of the resulting voltage. The state machine is required to debounce
 * the voltage changes and to translate them safely into button events like button pressed
 * and button released. Furthermore, this module knows about the clients of the buttons and
 * will notify them accordingly.
 *   @remark
 * This function is trigered by an RTOS event whenever a new voltage input value is
 * available. Therefore it doesn't have a parameter. Instead, it reads its input by side
 * effect from \a adc_buttonVoltage.
 */

void but_onNewButtonVoltage()
{
    /* Get the currently recognized button. The input voltage is written by a task of
       higher priority and accordingly, we need to apply a critical section to read the
       value. */
    cli();
    uint16_t buttonVoltage = adc_buttonVoltage;
    sei();
    enumButton_t btn = decodeLCDButton(buttonVoltage);

    /* Debouncing: The recognized button is unsafe. The voltage measurement averages the
       input and the actual button event is in no way synchronized with the averaging time
       window. Any intermediate voltage can be seen and any wrong, never touched button can
       be temporarily recognized. A recognized button is considered pressed only after we
       saw the same information a number of times. */
#define USE_TWO_PAST_VALUES 0

    static enumButton_t btn01_ = btnInvalid;
#if USE_TWO_PAST_VALUES != 0
    static enumButton_t btn02_ = btnInvalid;
#endif

    if(
#if USE_TWO_PAST_VALUES != 0
        btn01_ != btn02_  ||
#endif
        btn != btn01_
      )
    {
#if USE_TWO_PAST_VALUES != 0
        btn02_ = btn01_;
#endif
        btn01_ = btn;
        btn    = btnInvalid;
    }
#if USE_TWO_PAST_VALUES != 0
    else
    {
        btn02_ = btn01_;
    }
#endif

    /* State machine: If btn is btnInvalid we see an intermediate state which carries
       no exploitable information and which must not have any impact on our state.
       Otherwise btn is the validated, current button status. Feed it into the state
       machine. */
    if(btn != btnInvalid)
    {
        /* The state machine has the following perspective: One button can be pressed at a
           time and it needs to be released prior to pressing the same or another button.
           This excludes transitions, which could be generated by first pressing a single
           button, then another one at the same time and then releasing the first one. The
           state machine continues to see only the first button pressed until the user
           really releases all buttons. */
        static enumButton_t btnLast_ = btnNone;

        /* Look for the button-down-event. */
        if(btnLast_ == btnNone  &&  btn != btnNone)
            btnLast_ = btn;
        else
        {
            btnLast_ = btn;
            btn = btnInvalid;
        }
    } /* End if(Stable button status seen?) */

    /* Dispatcher: If btn is not btnInvalid it contains the button, which has been
       pressed in this instance. The information is passed to the client, which is
       listening to this button. */
    if(btn != btnInvalid)
    {
        switch(btn)
        {
            /* Up and down are used to adjust the real time clock. The number of such
               events is counted; the RTC code acknowledges by decrementing by the number
               of events it has considered.
                 The RTC task is running at a lower priority, so we can safely access its
               global interface without synchronization code. */
        case btnUp:
            ++ clk_noButtonEvtsUp;
            break;

        case btnDown:
            ++ clk_noButtonEvtsDown;
            break;

            /* The buttons right and left are used to switch hence and forth between ADC
               inputs. */
        case btnLeft:
            adc_nextInput(/* up */ false);
            break;

        case btnRight:
            adc_nextInput(/* up */ true);
            break;

            /* The select button is not in use. */
        case btnSelect:
            break;

        default:
            ASSERT(false);
        }
    }
} /* End of but_onNewButtonVoltage */




