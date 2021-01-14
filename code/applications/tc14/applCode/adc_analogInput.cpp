/**
 * @file adc_analogInput.cpp
 *   The ADC task code: Process the analog input.
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
 *   adc_initAfterPowerUp
 *   adc_nextInput
 *   adc_onConversionComplete
 * Local functions
 *   selectAdcInput
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos.h"
#include "aev_applEvents.h"
#include "dpy_display.h"
#include "adc_analogInput.h"


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
 
/** Global counter of all ADC conversion results starting with system reset. The frequency
    should be about 960 Hz. */
volatile uint32_t adc_noAdcResults = 0;

/** The voltage at the buton input. */
volatile uint16_t adc_buttonVoltage = 0;

/** The voltage measured at the user selected analog input, see \a _userSelectedInput. */
volatile uint16_t adc_inputVoltage = 0;

/** The user selected ADC input as a linear number between 0 and 16. (16 references the
    internal band gap voltage reference as ADC input.
      @remark This variable has a special character. It is manipulated via function \a
    adc_nextInput solely by another task, the user interface task. The task, which is
    implemented in this module will never touch this variable.
      @remark The initial value 16 corresponds to the MUX value ADC_INPUT_INTERNAL_BAND_GAP
    and must never be changed. */
static uint8_t _userSelectedInputLin = 16;

/** The user selected ADC input. Caution, the value is expected as can be written directly
    into the ADC register MUX5:0. The numbers of the normal inputs in the range 0..15 are
    split by two inserted null bits at position b3 and b4 and the internal band gap
    reference is selected by the constant 0x1e.
      @remark The initial value must be chosen in close correspondence with the other
    variable \a _userSelectedInputLin. */
static volatile uint8_t _userSelectedInputMux = ADC_INPUT_INTERNAL_BAND_GAP;


/*
 * Function implementation
 */



/**
 * Reprogram the ADC so that the next conversion will use another input.
 *   @param input
 * The input to select as ADC register value MUX5:0.
 */ 

static void selectAdcInput(uint8_t input)
{
    /* Two registers contain bits of the register value MUX5:0, which selects the input.
       Both are written in a read/modify/write operation as they also contain other
       configuration information, which must not be changed. */
    ADMUX  = (ADMUX  & ~0x1f) | (input & 0x1f);
    ADCSRB = (ADCSRB & ~0x08) | ((input & 0x20) >> 2);
    
} /* End of selectAdcInput */




/**
 * Configure the ADC but don not release the interrupt on ADC conversion complete yet. Most
 * important is the hardware triggered start of the conversions, see chosen settings for
 * ADATE and ADTS.\n
 *   The initialization is called at system startup time, before the RTuinOS kernel is
 * started and multitasking takes place. Therefore it's crucial to not enable the actual
 * interrupts yet. This must be done as part of the start of the kernel, when the system is
 * ready to accept and handle the interrupts. Please refer to the RTuinOS manual for more.
 */

void adc_initAfterPowerUp()
{
    /* Setup the ADC configuration. */

    /* ADMUX */
#define VAL_ADLAR   0    /* ADLAR: Result must not be left aligned. */

/** The initial setting for register MUX needs be be the button input in order to be inline
    with the initialization of the state machine doing the result evaluation, see void
    adc_onConversionComplete(void). */
#define VAL_MUX ((((ADC_INPUT_LCD_SHIELD_BUTTONS) & 0x8) << 2) + ((ADC_INPUT_LCD_SHIELD_BUTTONS) & 0x7))

    ADMUX = (ADC_VAL_ADMUX_REFS << 6)
            + (VAL_ADLAR << 5)
            + ((VAL_MUX & 0x1f) << 0);

#undef VAL_ADLAR

    /* ADCSRB */
#define VAL_ACME    0   /* Don't allow analog comparator to use ADC multiplex inputs. */
#define VAL_ADTS    4   /* Auto trigger source is Timer/Counter 0, Overflow, 977 Hz. */

    ADCSRB = (((VAL_MUX & 0x20) != 0) << 3)
             + (VAL_ADTS << 0);

#undef VAL_MUX
#undef VAL_ACME
#undef VAL_ADTS

    /* ADCSRA */
#define VAL_ADEN 1 /* Turn ADC on. */
#define VAL_ADSC 1 /* Start series of conversion; may be done in same register write access. */
#define VAL_ADATE 1 /* Turn auto triggering on to minimize jitter in conversion timing. */
#define VAL_ADIF 1 /* Reset the "conversion-ready" flag by writing a one. */
#define VAL_ADIE 0 /* Do not allow interrupts on conversion-ready yet. */
#define VAL_ADPS 7 /* ADPS2:0: Prescaler needs to generate lowest possible frequency. */

    /* The regular conversions are running after this register write operation. */
    ADCSRA = (VAL_ADEN << 7)
             + (VAL_ADSC << 6)
             + (VAL_ADATE << 5)
             + (VAL_ADIF << 4)
             + (VAL_ADIE << 3)
             + (VAL_ADPS << 0);

#undef VAL_ADEN
#undef VAL_ADSC
#undef VAL_ADATE
#undef VAL_ADIF
#undef VAL_ADIE
#undef VAL_ADPS
} /* End of adc_initAfterPowerUp */





/**
 * The input for the measured and displayed voltage is changed by one, upwards or downwards.
 *   @param up
 * True to go from input n to n+1, false step back.
 *   @remark
 * The intended use case of this function is that it is called by another task, the user
 * interface task. The implemented data access synchronization requires that the other
 * task has the same or a lower priority than this task, the ADC interrupt task.
 */

void adc_nextInput(boolean up)
{
    /* Select the new input by increment/decrement. 
         Remark: Although implemented here in the ADC module the variable
       _userSelectedInputLin is completely owned by the user interface task ("owned" with
       respect to concurrency and access rights). No access synchronization code is needed
       although we have a read/modify/write operation. */
    if(up)
    {
        if(++_userSelectedInputLin > 16)
            _userSelectedInputLin = 0;
    }
    else
    {
        /* <0 is detected as overrun, it's an unsigned variable. */
        if(--_userSelectedInputLin > 16)
            _userSelectedInputLin = 16;
    }

    /* Transform the linear input number into the binary format, which can be used directly
       at run time, when the input is selected.
         The variable _userSelectedInput is read by the ADC task without access
       synchronization. It can be safely written by other tasks as as long as we use a
       single simple 8 Bit write operation. Therefore we need an intermediate variable. */
    uint8_t tmpMux;
    if(_userSelectedInputLin == 16)
        tmpMux = ADC_INPUT_INTERNAL_BAND_GAP;
    else
    {
        /* Insert two null bits into the binary number to make it the valid 6 Bit MUX
           value. */
        tmpMux = ((_userSelectedInputLin & 0x8) << 2) + (_userSelectedInputLin & 0x7);
    }

    /* Now write to the target variable in an atomic operation. */
    _userSelectedInputMux = tmpMux;

    /* Display selection of new ADC input. */
    dpy_display.printAdcInput(_userSelectedInputLin);

} /* End of adc_nextInput */




/**
 * The main function of the ADC task: It is the handler for the conversion complete
 * interrupt. It reads the new input sample from the ADC registers and processes it.
 * Processing means to do some averaging as a kind of simple down sampling and notify the
 * sub-sequent, slower running clients of the data.\n
 *   There are two kinds of data and two related clients: The analog input 0, which the LCD
 * shield's buttons are connected to, is read regularly and the input values are passed to
 * the button evaluation task, which implements the user interface state machine.\n
 *   A user selected ADC input is measured and converted to Volt. The client of this
 * information is a simple display task.
 */

void adc_onConversionComplete()
{
    /* We need a state machine here. There are two alternating series of conversions: Read
       the LCD shield's button input every first time and the "true", user selected input
       every second time.
         Averaging: Each series accumulates 64 samples. */
    static boolean readButton_ = true;
    static uint16_t accumuatedAdcResult_ = 0;
    static uint8_t noMean_ = ADC_NO_AVERAGED_SAMPLES;
    
    /* Accumulate all samples of the running series. Add the new ADC conversion result.
       First read ADCL then ADCH. Two statements are needed as it is not guaranteed in
       which order an expression a+b is evaluated. */
    accumuatedAdcResult_ += ADCL;
    accumuatedAdcResult_ += (ADCH<<8);

    /* Accumulate up to 64 values to do averaging and anti-aliasing for slower reporting
       task. */
    if(--noMean_ == 0)
    {
        /* A new down-sampled result is available for one of our clients. Since the clients
           have a lower priority as this task we don't need a critical section to update
           the client's input. */
        if(readButton_)
        {
            /* New ADC input is the user selected input. We do this as early as possible in
               the processing here, to have it safely completed before the next conversion
               start interrupt fires. */
            selectAdcInput(_userSelectedInputMux);
            
            /* Notify the new result to the button evaluation task. */
            adc_buttonVoltage = accumuatedAdcResult_;
            rtos_sendEvent(EVT_TRIGGER_TASK_BUTTON);
        }
        else
        {
            /* New ADC input is the user selected input. We do this as early as possible in
               the processing here, to have it safely completed before the next conversion
               start interrupt fires. */
            selectAdcInput(ADC_INPUT_LCD_SHIELD_BUTTONS);
           
            /* Notify the new result to the button evaluation task. */
            adc_inputVoltage = accumuatedAdcResult_;
            rtos_sendEvent(EVT_TRIGGER_TASK_DISPLAY_VOLTAGE);
        }

        /* Start next series on averaged samples. */
        readButton_ = !readButton_;
        noMean_ = ADC_NO_AVERAGED_SAMPLES;
        accumuatedAdcResult_ = 0;
    }
        
    /* Count the read cycles. The frequency should be about 960 Hz. */
    ++ adc_noAdcResults;

} /* End of adc_onConversionComplete */




