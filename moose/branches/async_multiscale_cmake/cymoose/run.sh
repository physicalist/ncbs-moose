#!/bin/bash -e
if [ $# -lt 1 ]; then
    rm -f ./moose_cython.so
    rm -f *.cpp
    rm -f *.c
    CXX="g++" \
    CC="g++" \
    LDFLAGS="-L. -L/usr/lib -L/usr/local/lib -L/usr/lib/mpi/gcc/openmpi/lib/" \
    python ./setup.py build_ext --inplace
else
    echo "Just testing"
fi
./test.py
