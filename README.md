# S-Algol
S-algol source and distribution files
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Directory Structure
~~~~~~~~~~~~~~~~~~~
The directory structure for the S-algol release has a distribution directory
and a source directory

1) Source directory
Contains 
comp: directory of compiler source
int: directory of interpreter source
other.things: S-algol source of realio and raster graphics code
              Also includes S in S compiler and other miscellaneous programs
display: source of the library for the X-11 interface

The make files for the source requires a environment variable ARCH to be set to the
result of the "arch" Unix command.
comp,display and int directories contain a makefile for each target architecture
(sun4_makefile, i86pc_makefile etc) automatical selected from the generic makefile.

A make install moves the binaries to the distribution directory under their correct name
( e,g. on a sun4 sc is installed in ../../distribution/lib/exec/sun4_sc)

2) Distribution directory
Contains a bin directory and a lib directory
Both the bin and lib directory can be located anywhere in the filesystem provided the
appropriate environment variables are set up as described below

The lib directory contains
The compiler prelude file S.prelude
The compiler standard functions file S.stand
an exec directory containing compiler and interpreter binaries for each target
a fonts directory 
a runtime directory - containing the .out files for realio and raster 
functions. Note there is one for each target architecture

The bin directory has two executable scripts sc and sr
sc excutes the compiler
sr runs the interpreter

Both these can be copied into the user's command path and will need to have the
following environment variables edited.
for sc: SPRELUDE is set to the location of the prelude file S.prelude
	SSTAND is set to the location of the standard functions file S.stand

for sr: SREALIO is set to the location of the target architecture realio code
	SRASTERFUNCS  is set to the location of the target architecture raster code


