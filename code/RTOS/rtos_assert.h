#ifndef RTOS_ASSERT_INCLUDED
#define RTOS_ASSERT_INCLUDED
/**
 * @file rtos_assert.h
 * Implementation of macro ASSERT for the Arduino board. If the assertion fires the code
 * attempts to write an error string into the global Serial object (its initialization
 * therefore is a prerequisite of using ASSERT), wait for a while and than makes a reset.
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

/*
 * Include files
 */


/*
 * Defines
 */
#ifdef DEBUG
/** Implementation of macro ASSERT for the Arduino board. If the assertion fires the code
    attempts to write an error string into the global Serial object (its initialization
    therefore is a prerequisite of using ASSERT), wait for a while and than makes a reset.\n
      If the compilation is not made in DEBUG mode ASSERT expands to nothing. */
# define ASSERT(cond)                                                                   \
    {                                                                                   \
        if(!(cond))                                                                     \
        {                                                                               \
            asm volatile                                                                \
            (                                                                           \
                "sei \n\t"                                                              \
            );                                                                          \
            volatile uint32_t u = 0x400000ul;                                           \
            Serial.print("Assertion failed in file " __FILE__ ", line ");               \
            Serial.println(__LINE__);                                                   \
            while(u>0)                                                                  \
                -- u;                                                                   \
            asm volatile                                                                \
            (                                                                           \
                "jmp 0 \n\t"                                                            \
            );                                                                          \
        }                                                                               \
    } /* End of macro ASSERT */
#else
# define ASSERT(cond)
#endif


/*
 * Global type definitions
 */


/*
 * Global data declarations
 */


/*
 * Global prototypes
 */



#endif  /* RTOS_ASSERT_INCLUDED */
