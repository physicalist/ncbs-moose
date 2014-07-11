# Wrapper around basecode/Id.h files 

from libcpp.string cimport string 
cimport Id as _Id

cdef extern from "../basecode/ObjId.h":
    cdef extern cppclass ObjId:
        ObjId() 
        ObjId(_Id.Id)
        string path()
