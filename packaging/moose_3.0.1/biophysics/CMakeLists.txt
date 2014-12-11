CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
include_directories(../basecode ../synapse)

FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(lib STATIC
    ${SRCS}
)

