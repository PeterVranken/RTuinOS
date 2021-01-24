#include <Arduino.h>

#include "rtos.h"

int main(void)
{
	init();

#if defined(USBCON)
	USBDevice.attach();
#endif

    /* The next function will never return. */
    rtos_initRTOS();
    
    /* This code is never reached. */
	return 0;
}

