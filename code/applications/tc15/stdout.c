/**
 * @file tc15/stdout.c
 *   stdout, the character stream used by the printf & co routines from the C standard
 * library, is redirected into the stream Serial. Using printf, Arduino applications can
 * communicate much easier with the console window as possible with the members of Serial
 * for formatted writing.
 *   The idea of the code has been found in the Arduino Forum, at
 * http://forum.arduino.cc/index.php?topic=120440.0, visited at June 12, 2013. It has been
 * published by an anonymous author.
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
 *   init_stdout
 *   puts_progmem
 * Local functions
 *   serial_putchar
 */

/*
 * Include files
 */

#include <Arduino.h>
#include "rtos_assert.h"
#include "stdout.h"


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
 
 
/*
 * Function implementation
 */

/**
 * This function writes a single character into Serial. It is associated with the global
 * FILE pointer stdout, so any write access on stdout will use Serial as channel.
 *   @return
 * 0 if operation succeeded, 1 otherwise.
 *   @param c
 * The character to print.
 *   @param f
 * The C FILE to print to. Not used, as this function is solely associated and in use
 * with our local FILE object.
 */ 

static int serial_putchar(char c, FILE* f)
{
    ASSERT(f == stdout);
    
    /* The console requires a carriage return at any line end. Possible error information
       is not evaluated. We'll probably get the same report in the next step anyway. */
    if(c == '\n')
        Serial.write('\r');

    return Serial.write(c) == 1? 0 : 1;
    
} /* End of serial_putchar */




/**
 * Initialization: The redirection of stdout into Serial, mainly for use by printf & co, is
 * done. This needs to be done prior to the first use of stdout and it may be done prior to
 * the initialization of Serial.
 */

void init_stdout()
{
    /* Create a persistent FILE object. */
    static FILE myStdout;
    
    /* By default stdout, the pointer to the FILE object to use, is null, i.e. no standard
       out is available. We let it point to our persistent FILE object. */
    stdout = &myStdout;
    
    /* Initialize our FILE object ans associate it (and thus stdout) with the charater
       write function, which will write the character into Serial. */
    fdev_setup_stream (&myStdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);

} /* End of init_stdout */




/**
 * Write a null terminated string located in the CPU's flash ROM to stdout. End output with
 * writing an additional newline character.
 *   @return
 * No failure is recognized and the function always returns the non-negative value 0.
 *   @param string
 * A pointer into the flash ROM.
 *   @remark
 * The function behaves like the function puts from the C library.
 */

int puts_progmem(const char *string)
{
    while(true)
    {
        char nextChar = pgm_read_byte_near(string++); 
        if(nextChar == '\0')
            break;
        
        putchar(nextChar);
    }
    
    putchar('\n');

    /* puts: "On success, a non-negative value is returned. On error, the function returns
       EOF and sets the error indicator (ferror)." */
    return 0;
    
} /* End of puts_progmem */




