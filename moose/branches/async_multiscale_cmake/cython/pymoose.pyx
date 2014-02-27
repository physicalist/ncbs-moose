## This is entry of cmake 

import os
import sys
from libcpp cimport bool

cimport basecode_Id
cimport shell_Shell
cimport biophysics_Compartment as comp

cdef class Compartment:
    cdef comp.Compartment* thisptr
    def __cinit__(self):
        print "Initializing a compartment"
        self.thisptr = new comp.Compartment()
    def __deinit__(self):
        del self.thisptr

    def setVm(self, double Vm): self.thisptr.setVm(Vm)
    def getVm(self): return self.thisptr.getVm()
    def setEm(self, double Em): self.thisptr.setEm(Em)
    def getEm(self): return self.thisptr.getEm()
    

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

