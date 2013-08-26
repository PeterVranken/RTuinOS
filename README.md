What is RTuinOS?
----------------

RTuinOS is a small scale real time operating system for Arduino. The main
documentation is found as doc\manual\RTuinOS-1.0-UserGuide.pdf. The hints
given here are just a brief summary of what's stated there.

This distribution has been compiled for Arduino 1.0.5 under Windows, Linux
or Mac OS. Only the Mega 2560 board is supported just like that. In the
user guide, in the source code and by compiler error directives you'll get
hints how to modify the code for other Arduino boards.

The Arduino environment can be got at www.Arduino.cc. It needs to be
installed and operational. Before using RTuinOS ensure that the original
Arduino IDE and the sample sketches work well.

To install RTuinOS, extract the files in the archive to a directory of
your choice. The target location must grant read access to all files and
write access to all directories and the path to the target location must
not contain any blanks!

The link between the RTuinOS build environment (GNU make based) and the
Arduino installation is made by means of a new environment variable. You
need to create the variable ARDUINO_HOME prior to opening any shell
window. ARDUINO_HOME holds the path to the Arduino installation, like
C:\ProgramFiles\arduino-1.0.5 under Windows.

Caution: No blanks in paths are permitted, and this holds true also for
the Arduino installation files. An Arduino installation at e.g.
C:\Program Files would make the makefile fail to compile the sources.

The GNU make tool, which is shipped with Arduino needs to be on the
operating system's search path. Extending the search path accordingly is
not part of the Arduino standard installation procedure; you will probably
still have to do this. Type make --version to find out: GNU Make 3.81
should start up. (Be aware, revision 3.80 or elder of make is not
sufficient.)


What's new in Release 1.0?
--------------------------

The makefile has been revised:

Different operating systems, Windows, Linux and Mac OS, are now
supported.

Path conventions are now obeyed: Usage of forward slashes and a trailing
slash in path names.

Tools are addressed by absolute paths to avoid conflicts with improperly
set PATH variable.

The build of different test cases has been decoupled; now each one has
its own build folder. A clean is no longer necessary when switching the
application.

The creation of required working directories has been integrated into
the build. Directory creation is no longer a build rule, which has to be
called explicitly.

The makefile has been split in parts. The configuration part is now
separated and clearly recognizable and readable to the user. The invariant
parts of the makefile have been hidden in a sub-directory.

A kind of "callback" is made into the application folder. An (optional)
makefile fragment located in the application folder will be included into
the build and permits to override general settings in an application
related fashion.


Support of Arduino 1.0.5, the current release as of today (31.7.2013)
  All test cases can be built and run with Arduino 1.0.5.

Support of mutexes and semaphores. The existing concept of events has been
extended. An event can now be of kind ordinary (broadcasted event, as
before), mutex or semaphore. Task resume conditions can continue to
combine any events regardless of the kind. An extension of RTuinOS' API
was not necessary to introduce mutexes and semaphores.

The API function rtos_setEvent has been renamed to the more adequate
rtos_sendEvent. The old name is deprecated but still operational as a
macro maps it onto the new name.

More assertions have been placed in the kernel for DEBUG compilation,
which anticipate and notify many typical application errors, like an idle
task, which tries to suspend.

A new test case (see tc14) proves the compatibility of the Arduino
LiquidCrystal library with RTuinOS.

The CPU load estimation is unchanged but has been moved from a test case
folder to the common folder RTOS and is now available to any application
just like that.

The doxygen documentation now includes those test cases, which contain
instructive sample code of general interest.
