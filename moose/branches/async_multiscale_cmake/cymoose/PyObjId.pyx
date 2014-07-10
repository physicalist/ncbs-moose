# Definition to this class.
cimport ObjId as _ObjId

cdef class PyObjId:

    cdef _ObjId.ObjId* thisptr

    def __cinit__(self):
        print("++ Creating a new ObjId")
        self.thisptr = new _ObjId.ObjId()

