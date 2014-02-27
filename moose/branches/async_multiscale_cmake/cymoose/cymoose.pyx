## This is entry of cmake 

import os
import sys
from libcpp cimport bool

cimport basecode_Id
cimport basecode_Eref as _eref
cimport shell_Shell
cimport biophysics_Compartment as _comp
cimport cython_header as _header

cdef class Compartment:
    cdef _comp.Compartment* thisptr
    def __cinit__(self):
        print "Initializing a compartment"
        self.thisptr = new _comp.Compartment()
    def __deinit__(self):
        del self.thisptr

    property Vm:
        def __get__(self): return self.thisptr.getVm()
        def __set__(self, x): self.thisptr.setVm(x)

    property Em:
        def __get__(self): return self.thisptr.getEm()
        def __set__(self, x): self.thisptr.setEm(x)
    
    property Rm:
        def __get__(self): return self.thisptr.getRm()
        def __set__(self, x): self.thisptr.setRm(x)
    
    property Ra:
        def __get__(self): return self.thisptr.getRa()
        def __set__(self, x): self.thisptr.setRa(x)

    property Im:
        def __get__(self): return self.thisptr.getIm()
        def __set__(self, x): self.thisptr.setIm(x)

    property Inject:
        def __get__(self): return self.thisptr.getInject()
        def __set__(self, x): self.thisptr.setInject(x)

    property initVm:
        def __get__(self): return self.thisptr.getInitVm()
        def __set__(self, x): self.thisptr.setInitVm(x)

    property diameter:
        def __get__(self): return self.thisptr.getDiameter()
        def __set__(self, x): self.thisptr.setDiameter(x)

    property lenght:
        def __get__(self): return self.thisptr.getLength()
        def __set__(self, x): self.thisptr.setLength(x)

    property p0:
        def __get__(self): 
            return (self.thisptr.getX0()
                    , self.thisptr.getY0()
                    , self.thisptr.getZ0()
                    )
        def __set__(self, p):
            self.thisptr.setX0(p[0])
            self.thisptr.setY0(p[1])
            self.thisptr.setZ0(p[2])

    property p1:
        def __get__(self): 
            return (self.thisptr.getX()
                    , self.thisptr.getY()
                    , self.thisptr.getZ()
                    )
        def __set__(self, p):
            self.thisptr.setX(p[0])
            self.thisptr.setY(p[1])
            self.thisptr.setZ(p[2])


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


