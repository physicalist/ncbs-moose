cmake_minimum_required(VERSION 2.8)
include_directories(../builtins ../basecode ../utility ../kinetics ../external/muparser)
FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(ksolve STATIC
    ${SRCS}
)
