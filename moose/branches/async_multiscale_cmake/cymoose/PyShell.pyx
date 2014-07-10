# Wrapper around msg/Shell class

from libcpp.string cimport string
from cython.operator import dereference

cimport Shell as _Shell
cimport Id as _Id
cimport ObjId as _ObjId

include "PyId.pyx"
include "PyObjId.pyx"

from libcpp.string cimport string

cdef extern from "./cymoose.hpp":
    _Shell.Shell* initMoose(int argc, char** argv, _Shell.Shell* s)

cdef class PyShell:

    cdef _Shell.Shell *thisptr

    def __cinit__(self):
        cdef char* argv = ""
        self.thisptr = initMoose(0, &argv, self.thisptr)

    def __dealloc__(self):
        del self.thisptr 

    def create(self, string path, PyObjId parent, string name, unsigned int numData
            , _Shell.NodePolicy nodePolicy = _Shell.MooseBlockBalance
            , unsigned int preferredNode = 1):
        """ Wrapper to doCreate function in Shell """

        cdef _Id.Id obj
        if not parent:
            obj = self.thisptr.create(path, NULL, name , numData
                    , nodePolicy, preferredNode
                    )
        else:
            obj = self.thisptr.create(path, parent.thisptr, name, numData
                    , nodePolicy, preferredNode 
                    )

        newObj = PyId()
        newObj.thisptr = &obj
        assert newObj
        return newObj
