cimport Id as _Id 
cimport ObjId as _ObjId

cdef class PyId:
    cdef _Id.Id* thisptr
    cdef public string path

    def __cinit__(self):
        self.thisptr = new _Id.Id()

    def __del__(self):
        del self.thisptr

    def setPath(self, sep = "/"):
        self.path = self.thisptr.path(sep)
