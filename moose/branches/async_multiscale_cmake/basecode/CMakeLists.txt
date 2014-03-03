cmake_minimum_required(VERSION 2.8)
include_directories(../msg ../external/debug)
add_library(basecode 
    Cinfo.cpp
    consts.cpp
    DataElement.cpp
    DestFinfo.cpp
    doubleEq.cpp
    Element.cpp
    EpFunc.cpp
    Eref.cpp
    FieldElement.cpp
    FieldElementFinfo.cpp
    Finfo.cpp
    GlobalDataElement.cpp
    HopFunc.cpp
    Id.cpp
    LocalDataElement.cpp
    ObjId.cpp
    OpFuncBase.cpp
    SetGet.cpp
    SharedFinfo.cpp
    SparseMatrix.cpp
    SrcFinfo.cpp
    testAsync.cpp
    ValueFinfo.cpp
    )
target_link_libraries(basecode biophysics)
