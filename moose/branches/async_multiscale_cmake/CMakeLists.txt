cmake_minimum_required(VERSION 2.8)
project(moose)
include(CheckIncludeFiles)

enable_testing()

# Compiler 
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6)
        message(FALTAL_ERROR "GCC version must be at least 4.6")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.0)
        message(FATAL_ERROR "Clang version must be at least 3.0")
    endif()
else()
    message(WARNING "Continuing with unsupported compiler")
endif()

# Use clang++ as default compiler for time being
set(CMAKE_CXX_COMPILER "clang++")

# Turn warning to error
add_definitions(-Werror 
    -Wno-return-type-c-linkage
    )

# Enable 2011 stupport.
set(ENABLE_STD_2011 1)
if(ENABLE_STD_2011)
    add_definitions(-DENABLE_STD_2011 -DDEBUG2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif(ENABLE_STD_2011)

# Strick checking 
set(DEVELOPER 1)
if(DEVELOPER)
    add_definitions(-DDEVELOPER -fPIC)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(INSTALL_LIB_DIR lib CACHE PATH "/usr/local/lib")
set(INSTALL_BIN_DIR bin CACHE PATH "/usr/local/bin")
set(INSTALL_INCLUDE_DIR include CACHE PATH "/usr/local/include/")

# Default macros
add_definitions(-DUSE_GENESIS_PARSER)

set(DEBUG 1)
if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Enable strict check. Raise exception instead of warning.
set(DEVELOPER 1)
if(DEVELOPER)
    add_definitions(-DDEVELOPER)
endif(DEVELOPER)

## Here we add an executable
include_directories(basecode msg)
add_executable(moose basecode/main.cpp)

# Build
find_package(GSL)
if(GSL_FOUND)
    # Check if header file exists 
    find_path(GSL_HAVE_ODE_SOLVER gsl/gsl_odeiv2.h ${GSL_INCLUDE_DIRS}
        /usr/include 
        /usr/local/include
        /usr/share/include 
        )
    if(NOT GSL_HAVE_ODE_SOLVER)
        message(FATAL_ERROR "Outdated version of libgsl")
    endif()
    set(USE_GSL)
endif()

if(USE_GSL)
    add_definitions(-DUSE_GSL)
    include_directories(${GSL_INCLUDE_DIRS} ${GSLCBLAS_INCLUDE_DIRS})
    target_link_libraries(moose ${GSL_LIBRARIES})
endif(USE_GSL)


## Setup hdf5
find_package(HDF5)
if(HDF5_FOUND)
    add_definitions(-DUSE_HDF5)
    include_directories(${HDF5_INCLUDE_DIR})
    target_link_libraries(moose ${HDF5_LIBRARY})
endif(HDF5_FOUND)

find_package(LIBSBML)
if(LIBSBML_FOUND)
    add_definitions(-DUSE_SBML)
    add_subdirectory(sbml)
    include_directories(${LIBSBML_INCLUDE_DIR})
    find_package(BZip2)
elseif()
    add_definitions(-UUSE_SBML)
endif(LIBSBML_FOUND)

find_package(Readline)
if(READLINE_FOUND)
    add_definitions(-DUSE_READLINE)
    include_directories(${Readline_INCLUDE_DIR})
    target_link_libraries(moose ${Readline_LIBRARY} termcap)
endif()

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
add_subdirectory(external/debug/)
add_subdirectory(mesh)
add_subdirectory(mpi)
add_subdirectory(ksolve)
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
    debug_print
    benchmarks
    shell
    )

if(LIBSBML_FOUND)
    if(BZIP2_FOUND)
        target_link_libraries(moose ${BZIP2_LIBRARIES})
    endif()
    target_link_libraries(moose moose_sbml ${LIBSBML_LIBRARY})
endif()

## Build pymoose 
set(BUILD_PYMOOSE 1)
if(BUILD_PYMOOSE)
    find_package(NumpyHeaders REQUIRED)
    if(PYTHON_NUMPY_FOUND)
        add_definitions(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
        include_directories(${PYTHON_NUMPY_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "Could not find numpy/arrayobject.h in usual places")
    endif()
    # Openmpi
    find_package(MPI REQUIRED)
    set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})

    find_package(PythonLibs REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    add_subdirectory(pymoose)
    add_library(_moose SHARED pymoose/moosemodule.cpp)
    set_target_properties(_moose PROPERTIES COMPILE_DEFINITIONS "PYMOOSE")
    set_target_properties(_moose PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY "./python/moose"
        PREFIX ""
        )

    set(LIBRARIES ${PYTHON_LIBRARY})
    if(HDF5_FOUND)
        list(APPEND LIBRARIES ${HDF5_LIBRARY})
    endif()
    if(LIBSBML_FOUND)
        list(APPEND LIBRARIES moose_sbml ${LIBSBML_LIBRARY})
    endif()
    if(GSL_FOUND)
        list(APPEND LIBSBML_LIBRARY ${GSL_LIBRARIES})
    endif()

    target_link_libraries(_moose 
        "-Wl,--whole-archive"
        moosepython
        ksolve
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
        debug_print
        device
        benchmarks
        pthread
        ${LIBRARIES}
        "-Wl,--no-whole-archive"
        )
endif(BUILD_PYMOOSE)
