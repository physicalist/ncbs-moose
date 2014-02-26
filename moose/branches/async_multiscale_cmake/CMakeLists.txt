cmake_minimum_required(VERSION 2.8)
project(moose)

# Compiler 
set(CMAKE_CXX_COMPILER g++)

# Enable 2011 stupport.
set(ENABLE_STD_2011 1)
if(ENABLE_STD_2011)
    add_definitions(-DENABLE_STD_2011)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif(ENABLE_STD_2011)

# Strick checking 
set(CHECK_STRICT 1)
if(CHECK_STRICT)
    add_definitions(-DSTRICT_CHECKING)
endif(CHECK_STRICT)

set(CMAKE_VERBOSE_MAKEFILE ON)

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

## Here we add an executable
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

# Add subdirectroeis
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


## Setup hdf5
find_package(HDF5)
if(HDF5_FOUND)
    add_definitions(-DUSE_HDF5)
    find_package(HDF5 REQUIRED)
    include_directories(${HDF5_INCLUDE_DIR})
    target_link_libraries(moose ${HDF5_LIBRARY})
else()
    add_definitions(-UHDF5)
endif(HDF5_FOUND)

find_package(LIBSBML)
if(LIBSBML_FOUND)
    add_definitions(-DUSE_SBML)
    add_subdirectory(sbml)
    include_directories(${LIBSBML_INCLUDE_DIR})
    target_link_libraries(moose moose_sbml)
    find_package(BZip2)
    if(BZIP2_FOUND)
        target_link_libraries(moose ${BZIP2_LIBRARIES})
    endif()
elseif()
    message("SMBL is not found")
    add_definitions(-UUSE_SBML)
endif(LIBSBML_FOUND)

set(USE_READLINE 1)
if(USE_READLINE)
    add_definitions(-DUSE_READLINE)
    target_link_libraries(moose readline)
endif(USE_READLINE)
## Build pymoose 
set(BUILD_PYMOOSE 1)
if(BUILD_PYMOOSE)
    find_package(NumpyHeaders REQUIRED)
    include_directories(${NUMPY_PATH})

    # Openmpi
    find_package(MPI REQUIRED)
    set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})

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
        ${GSL_LIBRARIES}
        "-Wl,--no-whole-archive"
        )
endif(BUILD_PYMOOSE)
