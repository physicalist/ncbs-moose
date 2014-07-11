# Definition to this class.
cimport ObjId as _ObjId

from cython.operator import dereference as deref

cdef class PyObjId:

    cdef _ObjId.ObjId* thisptr
    cdef public string path

    def __cinit__(self):
        self.thisptr = new _ObjId.ObjId()
