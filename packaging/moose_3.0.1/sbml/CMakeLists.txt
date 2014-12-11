FILE(GLOB SRCS *.cpp)
include_directories(../msg)
include_directories(../basecode)
add_library(moose_sbml ${SRCS})
