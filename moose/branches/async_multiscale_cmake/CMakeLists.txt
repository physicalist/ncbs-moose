cmake_minimum_required(VERSION 2.6)
project(moose)

# Compiler 
set(CMAKE_CXX_COMPILER g++)

# Enable 2011 stupport.
set(ENABLE_STD_2011 1)
if(ENABLE_STD_2011)
    add_definitions(-DENABLE_STD_2011)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif(ENABLE_STD_2011)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(INSTALL_LIB_DIR lib CACHE PATH "/usr/local/lib")
set(INSTALL_BIN_DIR bin CACHE PATH "/usr/local/bin")
set(INSTALL_INCLUDE_DIR include CACHE PATH "/usr/local/include/")

# Default macros
add_definitions(-DUSE_GENESIS_PARSER -DDO_UNIT_TESTS)

if(debug)
    set(CMAKE_BUILD_TYPE Debug)
endif(debug)

# Enable strict check. Raise exception instead of warning.
set(STRICT_CHECK 1)
if(STRICT_CHECK)
    add_definitions(-DSTRICT_CHECK)
endif(STRICT_CHECK)

add_subdirectory(basecode)
add_subdirectory(biophysics)
add_subdirectory(msg)
add_subdirectory(shell)
add_subdirectory(randnum)
add_subdirectory(scheduling)
add_subdirectory(builtins)
add_subdirectory(utility)
add_subdirectory(external/muparser)
add_subdirectory(external/colored_print/)
add_subdirectory(mesh)
add_subdirectory(mpi)
add_subdirectory(device)
add_subdirectory(benchmarks)
add_subdirectory(kinetics)

include_directories(basecode msg)
add_executable(moose basecode/main.cpp)

# Build
set(USE_GSL 1)
if(USE_GSL)
    find_package(GSL REQUIRED)
    add_definitions(-DUSE_GSL)
    include_directories(${GSL_INCLUDE_DIRS} ${GSLCBLAS_INCLUDE_DIRS})
    target_link_libraries(moose ${GSL_LIBRARIES})
endif(USE_GSL)
set(USE_HDF5 1)
if(USE_HDF5)
    add_definitions(-DUSE_HDF5)
    #find_package(HDF5 REQUIRED)
    #include_directories(${HDF5_INCLUDE_DIR})
    #target_link_libraries(moose ${HDF5_LIBRARIES})
    target_link_libraries(moose hdf5)
endif(USE_HDF5)

set(USE_SBML 1)
if(USE_SBML)
    add_definitions(-DUSE_SBML)
    add_subdirectory(sbml)
    target_link_libraries(moose moose_sbml sbml)
endif(USE_SBML)

set(USE_READLINE 1)
if(USE_READLINE)
    add_definitions(-DUSE_READLINE)
    target_link_libraries(moose readline)
endif(USE_READLINE)

# Links our libraries 
target_link_libraries(moose 
    scheduling
    builtins
    muparser
    mesh
    device
    moose_mpi
    msg 
    colored_print
    benchmarks
    )

## Build pymoose 
set(BUILD_PYMOOSE 1)
if(BUILD_PYMOOSE)
    find_package(MPI REQUIRED)
    find_package(PythonLibs 2.7 EXACT REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    add_subdirectory(pymoose)
    add_library(_moose SHARED pymoose/moosemodule.cpp)
    set_target_properties(_moose PROPERTIES COMPILE_DEFINITIONS "PYMOOSE")
    set_target_properties(_moose PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY "./python/moose"
        PREFIX ""
        )

    target_link_libraries(_moose 
        "-Wl,--whole-archive"
        moosepython
        basecode
        msg
        shell
        scheduling
        randnum
        moose_mpi
        builtins
        utility
        muparser
        biophysics
        kinetics
        mesh
        device
        benchmarks
        pthread
        colored_print
        ${PYTHON_LIBRARY}
        ${MPI_LIBRARY}
        ${GSL_LIBRARIES}
        "-Wl,--no-whole-archive"
        )
endif(BUILD_PYMOOSE)
