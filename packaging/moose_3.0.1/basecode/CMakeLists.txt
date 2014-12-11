cmake_minimum_required(VERSION 2.8)
include_directories(../msg)
FILE(GLOB SRCS *.cpp)
ADD_LIBRARY(basecode STATIC
    ${SRCS}
)
