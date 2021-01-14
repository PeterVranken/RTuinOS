#ifndef DPY_DISPLAY_INCLUDED
#define DPY_DISPLAY_INCLUDED
/**
 * @file dpy_display.h
 * Definition of global interface of module dpy_display.cpp
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
#include <LiquidCrystal.h>


/*
 * Defines
 */


/*
 * Global type definitions
 */

/** A single object of this class exists. It encapsulates the functionality of the Arduino
    standard library LiquidCrystal and reduces it to the printf functions needed for this
    application of the display. Furthermore the print functions implement all needed task
    synchronization: The display is shared by several tasks, which will all write their
    specific information into the display. */
class dpy_display_t: private LiquidCrystal
{
public:
    /** The constructor. It is not accessible from outside as there is only one instance of
        this class. This single object is plublic, not the constructor to make it. */
    dpy_display_t(void);

    /** Print a greeting after reset. */
    void printGreeting();
    
    /** To be called once first and typically only once at all: The invariant background
        parts of the layout are printed. This print may overwrite any of the others. */
    void printBackground(void);

    /** Formatted printing of ADC input selection. */
    void printAdcInput(int8_t adcInput);

    /** Formatted printing of current time. */
    void printTime(uint8_t hour, uint8_t min, uint8_t sec);

    /** Formatted printing of voltage. */
    void printVoltage(float voltage);

    /** Formatted printing of current CPU load. Scaling: 0.5% */
    void printCpuLoad(uint8_t cpuLoad);

private:
    inline boolean acquireMutex(void);
    inline void releaseMutex(void);

}; /* End of class dpy_display_t */



/*
 * Global data declarations
 */

/** The one and only display object. */
extern dpy_display_t dpy_display;


/*
 * Global prototypes
 */



#endif  /* DPY_DISPLAY_INCLUDED */
