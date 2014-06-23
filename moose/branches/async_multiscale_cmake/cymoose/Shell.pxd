# distutils: language = c++
# distutils: include_dirs = ., cymoose
# distutils: extra_compiler_args = -DCYTHON

from libcpp.string cimport string
cimport Id as _Id 
cimport ObjId as _ObjId

cdef enum NodePolicy:
    MooseGlobal
    MooseBlockBalance
    MooseSingleNode

cdef extern from "../shell/Shell.h":
    cdef cppclass Shell:
        Shell()
        
        _Id.Id doCreate( string type
                , _ObjId.ObjId parent
                , string name
                , unsigned int numData
                , NodePolicy nodePolicy = MooseBlockBalance
                , unsigned int preferredNode = 1 
                )

