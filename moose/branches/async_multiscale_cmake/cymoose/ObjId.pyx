# Definition to this class.
cimport ObjId as _ObjId
cimport Id as _Id

from cython.operator import dereference as deref
from libc.stdio cimport printf

cdef class PyObjId:

    cdef _ObjId.ObjId* thisptr
    cdef _ObjId.ObjId objId
    cdef public string path_

    def __repr__(self):
        msg = "<type: ObjId> at {}"
        if len(self.path_) < 1:
            self.path_ = self.objId.path()
        return msg.format(self.path_)

    def __cinit__(self, id_ = None, dataIndex = 0, fieldIndex = 0):
        self.thisptr = new _ObjId.ObjId()


    cdef getObj(self):
        return self.objId

    def path(self):
        """Get the path"""
        if len(self.path_) > 0:
            return self.path_
        else:
            return self.objId.path()


