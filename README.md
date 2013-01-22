RTuinOS
=======

RTuinOS is a small scale Real Time Operating System (RTOS) for Arduino.
The main documentation is found as doc\manual\manual.pdf. The hints given
here are just a brief summary of what's stated there.

This distribution has been compiled for Arduino 1.0.1. Only the Mega 2560
board is supported without more. In the manual, in the source code and by
compiler error directives you'll get hints how to adapt the code for other
Arduino boards.

The Arduino environment can be got at www.Arduino.cc. It needs to be
installed and operational. Before using RTuinOS ensure that the original
Arduino IDE and the sample sketches work well.

To install RTuinOS, extract the files in the archive to a directory of
your choice. The target location must grant read access to all files and
write access to all directories and the path to the target location must
not contain any blanks!

The link between the RTuinOS build environment (GNU make based) and the
Arduino installation is made by means of a new environment variable. You
need to create the variable ARDUINO_HOME prior to opening any Command
Prompt or Powershell window. ARDUINO_HOME holds the path to the Arduino
installation, like C:\ProgramFiles\arduino-1.0.1.

Caution: Also for the Arduino files holds true: No blanks in paths are
permitted. An Arduino installation at e.g. C:\Program Files would make
the makefile fail to compile the sources.

The GNU tools, compiler, linker and make processor, which are shipped with
Arduino need to be on the Windows search path. Extending the search path
accordingly (by two directories) is not part of the Arduino standard
installation procedure; you will probably still have to do this. Type
avr-gcc --version and make --version to find out: avr-gcc 4.3.2 and GNU
Make 3.81 should start up. (Be aware, revision 3.80 or elder of make is
not sufficient.)

You need to do a clean when switching between different applications.
