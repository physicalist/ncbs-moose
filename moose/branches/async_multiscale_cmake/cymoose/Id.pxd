# Wrapper around basecode/Id.h files 

cimport Eref as _Eref
from libcpp.string cimport string 

cdef extern from "../basecode/Id.h":
    cdef extern cppclass Id:
        Id() 
        string path(string)
        _Eref.Eref eref()
