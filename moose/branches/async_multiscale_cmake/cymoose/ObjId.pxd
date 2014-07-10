# Wrapper around basecode/Id.h files 
cdef extern from "../basecode/ObjId.h":
    cdef extern cppclass ObjId:
        ObjId() 

