# 
# Makefile for GNU Make 3.81
#
# Included makefile fragment, which specifies some application dependent settings.
#   The main purpose of this makefile is to demonstrate how the "callback" from RTuinOS'
# general purpose makefile into the application can be used to support a more complex
# directory structure to organize the source files. (Most samples just use a flat
# directory.) Furthermore, this sample requires to link against the floating point library
# for printf & co.
#   Remark: The name of this makefile fragment needs to be identical to the name of the
# application folder, which is located in RTuinOS/code/applications. The name extension is
# mk and the makefile needs to be located in the root of the application folder.
#
# Help on the syntax of this makefile is got at
# http://www.gnu.org/software/make/manual/make.pdf.
#
# Copyright (C) 2013 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
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

# This sample requires the floating point support for the standard I/O library (printf in
# the first place).
IO_FLOAT_LIB = 1
$(info tc12.mk: tc12 makes use of the stdio library with floating point support for printf & co)

# The sample writes its output with a higher Baud rate than usual and which deviates from
# the standard setting of the Arduino Serial Monitor. We can apply the makefile
# capabilities to issue a warning at least.
$(warning tc14.mk: This test case uses a Baud rate of 115200 bps for communication. \
Please, adjust the setting of the Arduino Serial Monitor prior to running the test case!)

