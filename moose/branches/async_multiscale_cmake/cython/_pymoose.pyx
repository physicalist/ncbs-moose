## This is entry of cmake 

import os
import sys
from libcpp cimport bool

cimport basecode_Id

def PyGetNumCores():
    return getNumCores()

cdef extern from "main_cython.hpp":
    unsigned int getNumCores()
    basecode_Id.Id init(int, char**, bool, bool, unsigned int)

def _main():
    cdef char* args = ""
    cdef bool doUnitTests = 0
    cdef bool doRegressionTests = 0
    cdef unsigned int benchmark = 0
    cdef basecode_Id.Id shellId = init(0
            , &args, doUnitTests, doRegressionTests, benchmark
            )

