## This is entry of cmake 

import os
import sys
from libcpp cimport bool

cimport basecode_Id
cimport shell_Shell
cimport biophysics_Compartment as comp

def PyGetNumCores():
    return getNumCores()

cdef extern from "main_cython.hpp":
    unsigned int getNumCores()
    int initMoose(int, char**)

def _main():
    cdef char* args = ""
    cdef bool doUnitTests = 0
    cdef bool doRegressionTests = 0
    cdef unsigned int benchmark = 0

