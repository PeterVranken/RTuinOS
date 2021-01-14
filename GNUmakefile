#
# Generic Makefile for RTuinOS Project
#
# Compilation and linkage of C(++) code into binary files and upload to the controller.
#
# Help on the syntax of this makefile is got at
# http://www.gnu.org/software/make/manual/make.pdf.
#
# Copyright (C) 2012-2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
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
#
# Preconditions
# =============
#
# The makefile is intended to be executed by the GNU make utility coming along with the
# Arduino package.
#   The name of the project can be assigned to the makefile macro project, see heading part
# of the code section of this makefile. If you don't do, your compilation products will use
# a standard name.
#   The makefile hard codes the hardware target. Among more, see makefile macros
# targetMicroController, cFlags and lFlags. Here, you will have to make some changes
# according to your selection of an ATmega micro controller. More changes will be required
# to the command lines of the object file tool avr-objcopy and the flash tool avrdude.
#   Hint: To find out how to run these tools, you can enable verbose mode in the Arduino
# IDE and compile one of the code examples. Build and upload the sketch in the IDE, then
# copy the contents of the IDE's output window and paste them into a text editor. You will
# find appropriate command lines for all the tools.
#   The Arduino installation directory needs to be referenced. The location is determined
# by environment variable ARDUINO_HOME. The variable holds the name of the folder in which
# the arduino executable is present. Caution: This variable is not created by the original
# Arduino installation process but needs to be created manually.
#   For your convenience, the Windows path should contain the location of the GNU make
# processor. If you name this file either makefile or GNUmakefile you will just have to
# type "make" in order to get your make process running. Typically, the path to the
# executable is $(ARDUINO_HOME)hardware/tools/avr/utils/bin. Consider to extend the
# Windows environment variable PATH accordingly.
#   This makefile does not handle blanks in any paths or file names. Please rename your
# paths and files accordingly prior to using this makefile.
#
# Targets
# =======
#
# The makefile provides several targets, which can be combined on the command line. Get
# some help on the available targets by invoking the makefile using
#   make help
#
# Options
# =======
#
# Options may be passed on the command line.
#   The follow options may be used:
#   CONFIG: The compile configuration is one out of DEBUG (default) or PRODUCTION. By
# means of defining or undefining macros for the C compiler, different code configurations
# can be produced. Please refer to the comments below to get an explanation of the meaning
# of the supported configurations and which according #defines have to be used in the C
# source code files.
#   COM_PORT: The communication port to be used by the flash tool needs to be known. The
# default may be adjusted to your environment in the heading part of the code section of
# this makefile or you may override the variable setting on the make processor's command
# line by writing e.g. make COM_PORT=\\.\COM3.
#   IO_FLOAT_LIB: If this flag is 0 the stdio library with reduced floating point support
# is linked with the RTuinOS application. printf & co do nor recognize floating point
# format characters like %f. This reduces the size of the code by about 1.5kByte, the RAM
# size is not affected. By default this falg is set to 1 and full support of printf & co is
# ensured.
#
# Input Files
# ===========
#
# The makefile compiles and links all source files which are located in a given list of
# source directories. The list of directories is hard coded in the makefile, please look
# for the setting of srcDirList below.
#   A second list of files is found as cFileListExcl. These C/C++ files are excluded from
# build.
#   Additionally required Arduino library files are hard coded in this makefile. They are
# referenced by absolute paths into the Arduino installation directory. Therefore, the
# installation directory needs to be known by means of an environment variable called
# ARDUINO_HOME.
#   These settings are invariant throughout the project life time and don't need
# maintenance.


# The name of the project is used for several build products.
project = RTuinOS_$(APP)

# The target micro controller the code is to be compiled for. The Setting is used in the
# command line of compiler, linker and flash tool. Please be aware, that changing this
# setting is not sufficient to ensure that this makefile is working with another target.
# You definitely have to double-check all the avr tool's command lines.
targetMicroController := atmega328p

# Communication port to be used by the flash tool. The default may be adjusted here to your
# environment or you may override the variable setting on the make processor's command line.
COM_PORT ?= COM8

# By default we link against the Ardunio standard library with reduced floating point
# support for printf & co. This library saves about 2k of code size. You may override the
# variable setting on the make processor's command line. Set it to 1 in order to link
# against the library that fully supports formatted output for floating point data types.
#   A better idea probably is to re-set this variable in the application owned makefile
# fragment code/applications/$(APP)/$(APP).mk.
IO_FLOAT_LIB ?= 0

# All customizing is done, now run the actual makefile code, which is implemented
# independent of the project specific settings made above.
sharedMakefilePath = makefile/
include $(sharedMakefilePath)compileLinkAndUpload.mk
