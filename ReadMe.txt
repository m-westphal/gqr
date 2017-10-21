GQR, a Fast Reasoner for Binary Qualitative Constraint Calculi
--------------------------------------------------------------

GQR accepts definitions of qualitative calculi like RCC-8 or Allen's
interval algebra and determines whether constraint networks in those calculi
are algebraically closed or globally consistent. It furthermore supports
building combinations of calculi based on existing calculi definitions.

The source code is documented via doxygen. By typing in "gqr help",
the program gives a brief description of its call parameters.

Examples of calculi and constraint network files can be found in data/.

If you have any questions regarding GQR, feel free to ask them by e-mail to
Matthias Westphal <westpham@informatik.uni-freiburg.de>,
Zeno Gantner <gantner@ismll.uni-hildesheim.de> or
Stefan Wölfl <woelfl@informatik.uni-freiburg.de>.


Some information about GQR can be found in

Zeno Gantner, Matthias Westphal, Stefan Wölfl (2008): GQR - A Fast
Reasoner for Binary Qualitative Constraint Calculi, to appear in
Workshop Notes of the AAAI-08 Workshop on Spatial and Temporal
Reasoning, Chicago, USA, AAAI.

If you use GQR in your own research, we encourage you to cite this article.


Installation
------------

For installation, you need a C++ compiler (tested with g++ 4.3.3) and Python.

To build and run GQR after download, extract the tar file, change to the new
directory and type:

a) ./waf configure

Configures GQR and checks the necessary dependencies.

b) ./waf

Compiles GQR.

c) ./_build_/default/gqr/gqr

Runs the GQR binary. During runtime GQR requires access to the calculi
definition files found in "data/". GQR searches for the calculi definition
files in the directory specified by the environment variable "GQR_DATA_DIR", in
the directory "data/" in the current working directory, and finally in the directory
given during configure by --prefix and --data-dir (in this specific order).

Additionally you can type "./waf check" to perform some unit tests.

GQR uses the waf build system. Further options for compilation and system-wide
installation are as follows:

./waf --help:  for usage page

./waf configure  : configuration step of GQR build
                   --relation-size=N sets the maximum number of base relations
                   to N
                   --enable-xml enables support for calculi definitions in XML
./waf            : build GQR
./waf install    : install GQR into your system;
                   check variables --prefix and
                   --data-dir in the configuration step;
./waf check      : build and run unit tests
./waf uninstall  : 
./waf clean      :
./waf distclean  :
./waf dist       : create a tarball of the current source tree


C Library
---------

The main GQR functions can be compiled into a C library which can be accessed from other
programming languages.

./waf configure --enable-libgqr

enables the library during configure. "./waf" then builds the library binary in
"_build_/default/gqr/". C example code that uses the library can be found in
"gqr/libgqr_example/". "gqr/libgqr_example/ReadMe.txt" contains further information.

Additionally, a python class using ctypes can be found in "gqr/python/".


Acknowledgements
----------------

This program was initially developed by Zeno Gantner under the supervision
of Malte Helmert, Bernhard Nebel and Stefan Wölfl at University of Freiburg.

Zeno would like to thank Jochen Eisinger and Matthias Spiller for their
occasional help in his struggle with C++, and Malte Helmert for introducing
him into the arcane arts of writing Makefiles and using the gprof profiler.

We would also like to thank Bernhard Nebel and Peter van Beek for giving
permission to use their queue implementation and to distribute it under the
terms of the GNU General Public License.

Matthias Westphal wrote and integrated the current RelationFixedBitSet
implementation, as well as all search improvements.

Further work on data structures and major parts of the code refactoring was
done by Isgandar Valizada and Thomas Liebetraut.

License
-------

Copyright (C) 2006, 2007, 2008, 2010 Zeno Gantner, Stefan Wölfl
Copyright (C) 2006, 2007 Micha Altmeyer
Copyright (C) 2007-2012 Matthias Westphal

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
USA.

GQR contains other sections of code that are copyrighted by other
institutions or individuals. The GQR copyright does not override or
invalidate those copyrights.

