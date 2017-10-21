#!/usr/bin/env bash

# Dynamic loader on Mac:
# When the library name is a filename (doesn’t include directory names), 
# the dynamic loader searches in the following locations (in the given order):
# (1) $LD_LIBRARY_PATH, (2) $DYLD_LIBRARY_PATH, (3) process’s working directory
# (4) $DYLD_FALLBACK_LIBRARY_PATH
# When the library name contains at least one directory name (relative or fully 
# qualified pathname), the dynamic loader searches for the library (pathname) in 
# the following order:  (1) $DYLD_LIBRARY_PATH  (2) given pathname,
# (3) $DYLD_FALLBACK_LIBRARY_PATH.

LIBGQR_PATH=../../_build_/default/gqr
# LIBGOBJECT_PATH=/usr/local/homebrew/lib

export LD_LIBRARY_PATH=${LIBGQR_PATH}:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${LIBGOBJECT_PATH}:${LD_LIBRARY_PATH}
python gqr.py


