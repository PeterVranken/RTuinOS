# 
# Makefile for GNU Make 3.81
#
# Locate all the external tools used by the other makefile fragments.
#
# Help on the syntax of this makefile is got at
# http://www.gnu.org/software/make/manual/make.pdf.
#
# Copyright (C) 2013 Sudar Muthu (mailto:sudar@sudarmuthu.com),
#                    Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.


# Quite typical, the GNU tools reside on a system at several locations as they come along
# with many applications. Hard to locate problems due to arbitrary order of references in
# the Windows search PATH can easily result. To avoid these problems we reference all tools
# by absolute path. The path is known as we have the Arduino installation directory.
ifeq ($(OS),WINDOWS)
    make := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/make.exe
    mkdir := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/mkdir.exe
    rmdir := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/rmdir.exe
    cat := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/cat.exe
    echo := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/echo.exe
    rm := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/rm.exe
    gawk := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/gawk.exe
    awk := $(gawk)
    touch := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/touch.exe
    mv := $(ARDUINO_HOME)hardware/tools/avr/utils/bin/mv.exe
    avr-gcc := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-gcc.exe
    avr-g++ := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-g++.exe
    avr-ar := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-ar.exe
    avr-objcopy := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-objcopy.exe
    avr-size := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-size.exe
    avrdude := $(ARDUINO_HOME)hardware/tools/avr/bin/avrdude
    avrdude_conf := $(ARDUINO_HOME)hardware/tools/avr/etc/avrdude.conf
else
    make := make
    mkdir := mkdir
    rmdir := rm -r
    cat := cat
    echo := echo
    rm := rm
    gawk := awk
    awk := $(gawk)
    touch := touch
    mv := mv
    avr-gcc := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-gcc
    avr-g++ := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-g++
    avr-ar := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-ar
    avr-objcopy := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-objcopy
    avr-size := $(ARDUINO_HOME)hardware/tools/avr/bin/avr-size
    avrdude := $(ARDUINO_HOME)hardware/tools/avr/bin/avrdude
    avrdude_conf := $(ARDUINO_HOME)hardware/tools/avr/etc/avrdude.conf
endif
