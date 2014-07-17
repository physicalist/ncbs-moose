cimport Id as _Id 
cimport ObjId as _ObjId
cimport Eref as _Eref

from cython.operator import dereference

include "PyEref.pyx"

cdef class PyId:

    cdef _Id.Id* thisptr

    cdef public PyEref eref_
    cdef public string path

    def __cinit__(self):
        self.thisptr = new _Id.Id()

    def __del__(self):
        del self.thisptr

    def setPath(self, sep = "/"):
        self.path = self.thisptr.path(sep)

    def eref(self):
        erefObj = PyEref()
        cdef _Eref.Eref e = self.thisptr.eref()
        erefObj.thisptr = &e
        self.eref_ = erefObj
        return erefObj
