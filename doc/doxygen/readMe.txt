doxygen is a freeware tool to generate professional documentation from
information found in the various C source files and maybe other files. In
order to run the generation process you need to install the tool first.
See http://www.doxygen.org for details.

The doxyfile in this directory controls the run of the tool in order to
produce the wanted output. It solely uses relative paths which found here.
In order to run doxygen successfully, you need to set the working
directory here and then start using the following commands:

doxygen

To open the compiled doxygen help you may type

html\index.html

or double-click on

openDoxygenHelp.cmd

The documentation can be generated for various outputs in according
folders, e.g. html and latex.

Please consider that any change in the doxyfile must not use absolute
paths (which easily happens if you use the doyygen GUI with its browse
buttons to edit the file) or relative paths which found on another
directory than here!
