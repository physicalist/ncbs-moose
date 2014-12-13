CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
include_directories(../basecode ../synapse)

FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(biophysics STATIC
    ${SRCS}
)

