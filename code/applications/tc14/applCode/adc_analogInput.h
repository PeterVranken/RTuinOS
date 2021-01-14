#ifndef ADC_ANALOGINPUT_INCLUDED
#define ADC_ANALOGINPUT_INCLUDED
/**
 * @file adc_analogInput.h
 * Definition of global interface of module adc_analogInput.c
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

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"


/*
 * Defines
 */

/** Scaling from binary ADC results to voltage (e.g. adc_buttonVoltage) in V. worldValue =
    #ADC_SCALING_BIN_TO_V(binaryValue) [V]. */
#define ADC_SCALING_BIN_TO_V(binVal)                                                        \
            ((ADC_U_REF/(double)(ADC_NO_AVERAGED_SAMPLES)/1024.0)*(double)(binVal))

/** Do not change: The ADC input which the buttons of the LCD shield are connected to. */
#define ADC_INPUT_LCD_SHIELD_BUTTONS    0

/** The selection of the internal reference voltage 1.1 V as ADC input as compatible with
    variable \a adc_userSelectedInput. */
#define ADC_INPUT_INTERNAL_BAND_GAP 0x1e

/** The number of subsequent ADC conversion results, which are averaged before the mean
    value is passed to the waiting client tasks. The values 1..64 are possible. The smaller
    the value the higher the overhead of the task processing. A value greater than about 40
    leads to a significant degradation of the responsiveness to button down events. */
#define ADC_NO_AVERAGED_SAMPLES     32

/** Value of ADC register ADMUX/REFS1:0. It selects the reference voltage or full scale
    value respectively. 1 means Ucc=5V, 2 means 1.1 V and 3 means 2.56 V. The internal
    references (1.1 V and 2.56 V are related to each other and undergo the same errors. The
    accuracy of these reference voltages is poor (about 5% deviation), the stabilized
    operational voltage seems to be more accurate. */
#define ADC_VAL_ADMUX_REFS    1

/** The reference voltage as floating point value for scaling purpose. */
#if ADC_VAL_ADMUX_REFS == 1
# define ADC_U_REF 5.0
#elif ADC_VAL_ADMUX_REFS == 2
# define ADC_U_REF 1.1
#elif ADC_VAL_ADMUX_REFS == 3
# define ADC_U_REF 2.56
#else
# error Illegal value for ADMUX/REFS (External reference is not supported)
#endif


/*
 * Global type definitions
 */


/*
 * Global data declarations
 */

/** Global counter of all ADC conversion results starting with system reset. The frequency
    should be about 960 Hz.
      @remark The values are written by the ADC task without access synchronization. They
    can be safely read only by tasks of same or lower priority and the latter need a
    critical section to do so. */
extern volatile uint32_t adc_noAdcResults;


/** The voltage measured at analog input #ADC_INPUT_LCD_SHIELD_BUTTONS which the buttons of
    the LCD shield are connected to. Scaling: worldValue = 5/1024/#ADC_NO_AVERAGED_SAMPLES
    * \a adc_buttonVoltage [V].
      @remark The values are written by the ADC task without access synchronization. They
    can be safely read only by tasks of same or lower priority and the latter need a
    critical section to do so. */
extern volatile uint16_t adc_buttonVoltage;


/** The voltage measured at the user selected analog input, see \a adc_userSelectedInput.
    Scaling: worldValue = 5/1024/#ADC_NO_AVERAGED_SAMPLES * \a adc_inputVoltage [V].
      @remark The values are written by the ADC task without access synchronization. They
    can be safely read only by tasks of same or lower priority and the latter need a
    critical section to do so. */
extern volatile uint16_t adc_inputVoltage;


/*
 * Global prototypes
 */

/** Moule initialization. To be called prior to start of RTuinOS kernel, i.e. from void
    setup(). */
void adc_initAfterPowerUp();

/** Select the next or previous input for the next conversion. */
void adc_nextInput(boolean up);

/** Main function of ADC task. Process next conversion result. */
void adc_onConversionComplete();


#endif  /* ADC_ANALOGINPUT_INCLUDED */
