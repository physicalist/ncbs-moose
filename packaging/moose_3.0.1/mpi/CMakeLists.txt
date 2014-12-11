INCLUDE_DIRECTORIES(../msg ../basecode)

FILE(GLOB SRCS PostMaster.cpp testMpi.cpp)
ADD_LIBRARY(moose_mpi STATIC
    ${SRCS}
)

