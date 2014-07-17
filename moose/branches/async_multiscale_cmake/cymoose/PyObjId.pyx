# Definition to this class.
cimport ObjId as _ObjId
cimport Id as _Id

from cython.operator import dereference as deref

cdef class PyObjId:

    cdef _ObjId.ObjId* thisptr
    cdef public string path_

    def __repr__(self):
        return "{} {}".format("", self.path_)

    def __cinit__(self,  id_ = None):
        self.thisptr = new _ObjId.ObjId()
        if id_:
            self.path_ = id_.path()
        
