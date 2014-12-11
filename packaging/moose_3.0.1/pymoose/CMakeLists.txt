# Find Numpy 
add_definitions(-DPYMOOSE)
include_directories(../basecode ../msg)

ADD_LIBRARY(moosepython STATIC
    mfield.cpp
    melement.cpp
    vec.cpp
    PyRun.cpp
    test_moosemodule.cpp
)
