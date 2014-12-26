# Wrapper around msg/Shell class

from libcpp.string cimport string

cimport Shell as _Shell
cimport Id as _Id 
cimport ObjId as _ObjId 

cdef class PyShell:

    cdef _Shell.Shell *thisptr

    def __cinit__(self):
        self.thisptr = new _Shell.Shell()

    def __dealloc__(self):
        del self.thisptr 

    cdef _Id.Id doCreate(self
            , _type
            , _ObjId.ObjId  _parent
            ,  _name
            , _numData
            , _nodePolicy
            , _preferedNode
            ):

        return self.thisptr.doCreate(_type, _parent, _name, _numData
                , _nodePolicy , _preferedNode)
    
    def callCreate(self):
        print("Here writing a wrapper function.")
