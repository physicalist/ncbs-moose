# Wrapper around basecode/Id.h files 

from libcpp.string cimport string 

cdef extern from "../basecode/Id.h":
    cdef extern cppclass Id:
        Id() 
        string path(string)
