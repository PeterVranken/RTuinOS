/**
 * Empty yield() hook.
 *
 * This function is intended to be used by library writers to build
 * libraries or sketches that supports cooperative threads.
 *
 * Its defined as a weak symbol and it can be redefined to implement a
 * real cooperative scheduler.
 */

#include "Arduino.h"

void yield(void)
{}

// No idea what's wrong with the original Arduino implementation of yield() but it doesn't compile
/*
static void __empty()
{
	// Empty
}

void yield(void) __attribute__ ((weak, alias("__empty")));
*/
