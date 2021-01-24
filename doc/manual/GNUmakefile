#
# Generic Makefile for LaTeX
#
# Compilation of LaTeX documents
#
# Help on the syntax of this makefile is got at
# http://www.gnu.org/software/make/manual/make.pdf.
#
# Copyright (C) 2012 Peter Vranken (mailto:Peter_Vranken@Yahoo.de)
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

# Set the name of your document.
#   The actually compiled document can be altered by setting a variable on
# the command line of make.
doc ?= RTuinOS-1.0-UserGuide

# General settings for the makefile.
#$(info Makeprocessor in use is $(MAKE))
SHELL := cmd
.SHELLFLAGS := /c

texFlags := -halt-on-error -c-style-errors -synctex=1 -time-statistics -max-print-line=255

# The list of include paths to be considered.
includeDirs := sources sources\figures sources\templates
texFlags += $(foreach dir,$(includeDirs),-include-directory=$(dir))

.PHONY: $(doc).pdf
$(doc).pdf:
	$(info Compiling $(@:.pdf=.tex))
	pdflatex.exe $(texFlags) $(@:.pdf=.tex)
	pdflatex.exe $(texFlags) $(@:.pdf=.tex)
	pdflatex.exe $(texFlags) $(@:.pdf=.tex)
	del $(doc).aux > nul 2> nul
	del $(doc).out > nul 2> nul
	del $(doc).toc > nul 2> nul
	del $(doc).lof > nul 2> nul
	del $(doc).lot > nul 2> nul
	del $(doc).synctex.gz(busy) > nul 2> nul
	del $(doc).synctex.gz > nul 2> nul

.PHONY: onePass
onePass:
	$(info Compiling one pass of $(doc).tex)
	pdflatex.exe $(texFlags) $(doc).tex

# Access help as default target or by several names. This target needs to be the first one
# in this file.
.PHONY: h help targets usage
h help targets usage:
	$(info Usage: make [-s] [doc=<latexMainFile>] [help] [onePass])
	$(info where <latexMainFile> is the LaTeX source file name without extension. Default is $(doc).)
	$(info Available targets are:)
	$(info   - onePass: Run the LaTeX compiler just once, e.g. for fast syntax check)
	$(info   - help: Print this help)
	$(error)
