# distutils: language = c++
# distutils: include_dirs = ., cymoose
# distutils: extra_compiler_args = -DCYTHON

include "PyShell.pyx"
include "PyNeutral.pyx"

cimport Id as _Id
cimport ObjId as _ObjId 
cimport Neutral as _Neutral
cimport Compartment as _Compartment

shell = PyShell()

cdef class Neutral:

    """Neutral class """

    cdef public PyId obj

    def __cinit__(self, path):
        self.obj =  shell.create("Neutral", path, 1)

    def __repr__(self):
        return "Id: {0}, Type: Neutral, Path: {1}".format(None, self.obj.path)

    def path(self):
        return self.obj.path


cdef class Compartment:

    """ Compartment class """

    cdef public PyId obj

    def __cinit__(self, path, numData = 1):
        self.obj = shell.create("Compartment", path, numData)

    def __repr__(self):
        print self.obj.eref()
        return "Id: {0}, Type: Compartment, Path: {1}".format(1, self.obj.path)

    def path(self):
        return self.obj.path




