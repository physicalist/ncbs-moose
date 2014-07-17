# distutils: language = c++
# distutils: include_dirs = ., cymoose
# distutils: extra_compiler_args = -DCYTHON

cimport Id as _Id
cimport ObjId as _ObjId
from libcpp.string cimport string
from libcpp.map cimport map

cdef extern from "../shell/Shell.h":
    cdef enum NodePolicy:
       MooseGlobal
       MooseBlockBalance 
       MooseSingleNode

    cdef enum AssignmentType:
       SINGLE
       VECTOR
       REPEAT

    cdef extern cppclass Shell:
        Shell() 
        Shell* initShell()
        _Id.Id create(string elemType
                , string path
                , unsigned int numData 
                , NodePolicy nodePolicy 
                , unsigned int preferredNode 
                ) 

cdef extern from "../shell/Wildcard.h":
    int wildcardFind(const string& n, map[string, _ObjId.ObjId]& ret) 

