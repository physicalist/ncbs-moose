cmake_minimum_required(VERSION 2.8)
include_directories(../basecode ../utility ../kinetics)
add_library(synapse
    GraupnerBrunel2012CaPlasticitySynHandler.cpp
    SimpleSynHandler.cpp
    STDPSynapse.cpp
    STDPSynHandler.cpp
    Synapse.cpp
    SynHandlerBase.cpp
    testSynapse.cpp
    )
