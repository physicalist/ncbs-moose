cimport Neutral as _neutral

cdef class Neutral:
    cdef _neutral.Neutral *thisptr
    def __cinit__(self):
        self.thisptr = new _neutral.Neutral()


