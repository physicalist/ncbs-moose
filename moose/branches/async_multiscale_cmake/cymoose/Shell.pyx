# Wrapper around msg/Shell class
include "Id.pyx"
include "ObjId.pyx"

from libcpp.string cimport string
cimport Shell as _shell
cimport Id as _id 
cimport ObjId as _objid 

cdef class Shell:
    cdef _shell.Shell *thisptr
    cdef _objid.ObjId objId
    def __cinit__(self):
        self.thisptr = new _shell.Shell()

    def __dealloc__(self):
        del self.thisptr 

    cdef _id.Id doCreate(self
            , _type
            , _objid.ObjId  _parent 
            ,  _name
            , _numData
            , _nodePolicy
            , _preferedNode
            ):
        return self.thisptr.doCreate(_type, _parent, _name, _numData
                , _nodePolicy , _preferedNode)
    
    def callCreate(self):
        print("Here writing a wrapper function.")
