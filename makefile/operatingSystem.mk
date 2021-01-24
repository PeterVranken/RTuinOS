# 
# Makefile for GNU Make 3.81
#
# Determine the operation system in use.
#
# Help on the syntax of this makefile is got at
# http://www.gnu.org/software/make/manual/make.pdf.
#
# Copyright (C) 2013 Sudar Muthu (mailto:sudar@sudarmuthu.com),
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

# Detect OS
ifeq ($(OS),Windows_NT)
    OS := WINDOWS
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS := LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        OS := MAC
    endif
endif

# General settings for the makefile.
ifeq ($(OS),WINDOWS)
    SHELL = cmd
    .SHELLFLAGS = /c
endif
