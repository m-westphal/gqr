# Overview

GQR features a C library based on the gobject framework. The library supports
standard operations on constraint networks over binary qualitative constraint
calculi, such as algebraic closure (path consistency) and depth-first search
to derive a scenario (atomic constraint network).

The C interface will certainly evolve and be extended in future releases, but
we plan on maintaining backwards compatibility.

## Building the library

Enable compiling of the library by running `./waf configure --enable-libgqr`.
Then run `./waf` to build gqr and the library. Once build, the library will
be available at *_build_/default/gqr/libgqr.so*.

## Using the library

In order to use the library in a C program, you have to include `libgqr.h`.
When compiling include and link the necessary paths/libraries, e.g., by using
*pkg-config* to obtain those for the gobject framework:
`pkg-config  --libs gobject-2.0` and `pkg-config  --cflags gobject-2.0`.
For *libgqr.so* and *libgqr.h* you need to set this information manually.
See the included simplistic Makefile for an example.

Further, to run your own programs you will need *libgqr.so* and the used
calculus data files (e.g., those shipped with GQR).

## Example C file

The file *example.c* contains a simple C program that uses the library. Run
the included Makefile to compile the program to *example* (and run it once).

To run the example use `export LD_LIBRARY_PATH=../../_build_/default/gqr`
(Note: on MacOS you need `export DYLD_LIBRARY_PATH=../../_build_/default/gqr)
to include the library directory in the search path for libraries, and run
`./example`.
