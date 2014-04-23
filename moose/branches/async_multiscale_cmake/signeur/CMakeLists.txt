cmake_minimum_required(VERSION 2.8)
include_directories(../basecode ../utility ../kinetics)
add_library(signeur
    Adaptor.cpp
    Adaptor.h
    )
target_link_libraries(signeur basecode)
