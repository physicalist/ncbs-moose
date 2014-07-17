# Definition to this class.
cimport ObjId as _ObjId
cimport Id as _Id

from cython.operator import dereference as deref

cdef class PyObjId:

    cdef _ObjId.ObjId* thisptr
    cdef public string path

    def __repr__(self):
        return "{} {}".format("ObjId", self.path)

    def __cinit__(self, id_ = None, dataIndex = 0, fieldIndex = 0):
        self.thisptr = new _ObjId.ObjId()

