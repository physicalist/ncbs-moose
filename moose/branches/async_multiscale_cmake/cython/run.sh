#!/bin/bash -e
if [ $# -lt 1 ]; then
    rm -f ./moose_cython.so
    CXX="g++" \
    CC="g++" \
    LDFLAGS="-L/usr/lib -L/usr/local/lib -L/usr/lib/mpi/gcc/openmpi/lib/" \
    python ./setup.py build_ext --inplace
else
    echo "Just testing"
fi
./test.py
