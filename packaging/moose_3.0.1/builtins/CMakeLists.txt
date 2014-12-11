CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(../msg)
include_directories(../basecode)
include_directories(../external/muparser)

FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(moose_builting STATIC
    ${SRCS}
)
