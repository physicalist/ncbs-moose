include_directories(../msg)
include_directories(../basecode ../utility)
FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(kinetics STATIC
    ${SRCS}
)
