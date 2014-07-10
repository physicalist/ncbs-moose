cimport Id as _Id 
cimport ObjId as _ObjId

cdef class PyId:
    cdef _Id.Id* thisptr

    def __cinit__(self):
        print("A new ID is being created by Python")
        self.thisptr = new _Id.Id()
