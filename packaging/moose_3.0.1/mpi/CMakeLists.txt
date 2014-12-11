include_directories(../msg ../basecode)
IF(USE_MPI) 
    find_package(MPI REQUIRED)
    include_directories(MPI_INCLUDE_PATH)
    ADD_DEFINITIONS(-DUSE_MPI)
ENDIF(USE_MPI)

FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(moose_mpi STATIC
    ${SRCS}
)

