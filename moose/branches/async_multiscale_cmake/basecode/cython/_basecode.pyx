## This is entry of cmake 

cdef extern from "Cinfo.h":
    cdef cppclass Cinfo:
        pass

cdef class PyCinfo:
    cdef Cinfo *thisptr

    def __cinit__(self):
        self.thisptr = new Cinfo()

    def __dealloc__(self):
        del self.thisptr

