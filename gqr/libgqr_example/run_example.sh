#!/usr/bin/env bash

export DYLD_LIBRARY_PATH=../../_build_/default/gqr:${DYLD_LIBRARY_PATH}
export LD_LIBRARY_PATH=../../_build_/default/gqr:${LD_LIBRARY_PATH}
./example

