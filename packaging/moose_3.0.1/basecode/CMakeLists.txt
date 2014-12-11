cmake_minimum_required(VERSION 2.8)
include_directories(../msg)
FILE(GLOB SRCS *.cpp)
## Remove the main.cpp from this list. Else we'll have multiple declarations of
## some symbols error.
LIST(REMOVE_ITEM SRCS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
ADD_LIBRARY(basecode ${SRCS}
)
