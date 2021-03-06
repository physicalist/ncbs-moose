cmake_minimum_required(VERSION 2.8)
project(moose)
set(CMAKE_INSTALL_PREFIX /usr)

# This snippet is from LLVM project.
# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build (unless on MSVC_IDE, where it is ok), and to make
# sure that we don't have any stray generated files lying around in the tree
# (which would end up getting picked up by header search, instead of the correct
# versions).

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
    message(FATAL_ERROR 
        "In-source builds are not allowed.
        CMake would overwrite the makefiles distributed with Moose.
        Please create a directory and run cmake from there, passing the path
        to this source directory as the last argument.
        This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
        Please delete them."
        )
endif()

################################# CMKAE MACROS #################################

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(INSTALL_LIB_DIR lib CACHE PATH "${CMAKE_INSTALL_PREFIX}/lib")
set(INSTALL_BIN_DIR bin CACHE PATH "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_INCLUDE_DIR include CACHE PATH "${CMAKE_INSTALL_PREFIX}/include/")

########################### COMPILER MACROS #####################################
# Compiler 

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_COMPILER "clang++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_COMPILER "g++")
else()
    message(WARNING "Continuing with unsupported compiler: ${CMAKE_CXX_COMPILER}")
endif()

## Turn warning to error
add_definitions(-Wall
    #-Wno-return-type-c-linkage
    -Wno-unused-variable
    -Wno-unused-function
    #-Wno-unused-private-field
    )
add_definitions(-fPIC)

## Enable/Disable 2011 stupport.
set(ENABLE_STD_2011 0)
if(ENABLE_STD_2011)
    add_definitions(-DENABLE_STD_2011 -DDEBUG2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif(ENABLE_STD_2011)

# DO UNIT TESTS 
set(ENABLE_UNIT_TESTS 0)
if(ENABLE_UNIT_TESTS)
    MESSAGE("++ Unit testing enabled.")
    add_definitions(-DDO_UNIT_TESTS)
endif()

set(VERIFY 1)
if(VERIFY)
    add_definitions(-DSANITY_CHECK -DARGS_CHECK -DRESULT_CHECK -DVALUE_CHECK)
endif(VERIFY)

# VERBOSITY OF OUTPUT
set(VERBOSITY 0)
if(VERBOSITY)
    message("++ Verbosity of output is ${VERBOSITY}")
    add_definitions(-DVERBOSITY=${VERBOSITY})
else()
    #message("++ Moose will be quiet")
    #add_definitions(-DQUIET_MODE)
endif()

# STATS INSIDE MOOSE
set(ENABLE_LOGGER 0)
if(ENABLE_LOGGER)
    message("++ LOGGER ENABLED")
    add_definitions(-DENABLE_LOGGER)
endif()

# Default macros
add_definitions(-DUSE_GENESIS_PARSER)

set(DEBUG 0)
if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
    add_definitions(-UDO_UNIT_TESTS -O2)
endif()

find_package(GSL 1.16 REQUIRED)

# If installed version of gsl is not 1.16 then download and build gsl locally.
# THIS IS USEFULL TO BUILD MOOSE FOR VARIOUS DISTRIBUTIONS.
if(${GSL_VERSION} VERSION_LESS 1.16)
    MESSAGE("+ Trouble. Needed version 1.16.")
    MESSAGE("+-- I'll go ahead, download gsl-1.16 and build it for moose. Make
    sure you have proxy environment variables set.")
    SET(LOCAL_GSL 1)
endif()

if(LOCAL_GSL)
    set(GSL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/gsl-1.16)
    set(GSL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/gsl)
    include(ExternalProject)
    ExternalProject_Add(GSL
        PREFIX ${GSL_SOURCE_DIR}
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/external
        URL https://github.com/dilawar/gsl/archive/master.zip 
        CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_C_FLAGS=-fpic -DCMAKE_INSTALL_PREFIX:PATH=${GSL_INSTALL_DIR} -DBUILD_SHARED_LIBS=OFF
        )
    include_directories(${GSL_INSTALL_DIR}/include)
    set(LOCAL_GSL_LIBRARIES ${GSL_INSTALL_DIR}/lib/libgsl.a 
        ${GSL_INSTALL_DIR}/lib/libgslcblas.a
        )
endif()

find_package(LIBSBML)
if(NOT LIBSBML)
    MESSAGE("+ LIBSBML NOT FOUND. Using local SBML")
    SET(LOCAL_SBML 1)
else()
    include_directories(${LIBSBML_INCLUDE_DIR})
endif()

if(LOCAL_SBML)
    MESSAGE(" ++ I need to download SBML")
    set(SBML_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sbml)
    set(SBML_INSTALL_DIR ${CMAKE_SOURCE_DIR}/sbml)
    include(ExternalProject)
    ExternalProject_Add(LIBSBML
        PREFIX ${EXTERNAL_DIR}
        URL http://sourceforge.net/projects/sbml/files/libsbml/5.10.2/stable/libSBML-5.10.2-core-src.tar.gz
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${SBML_INSTALL_DIR}
        -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS=-fpic 
        )
endif()


################################### SETUP BUILD ################################
include_directories(msg basecode)
add_executable(moose-bin basecode/main.cpp)
add_library(moose SHARED basecode/main.cpp)
set_target_properties(moose PROPERTIES COMPILE_FLAGS  "-DMOOSE_LIB")

## GSL
# Setup GSL
if(LOCAL_GSL)
    set(GSL_FOUND 1)
    add_definitions(-DUSE_GSL)
    add_dependencies(moose GSL)
else()
    if(GSL_FOUND)
        include_directories(${GSL_INCLUDE_DIR})
        add_definitions(-DUSE_GSL)
    endif()
endif()


## Setup hdf5
find_package(HDF5)
if(HDF5_FOUND)
    add_definitions(-DUSE_HDF5)
    include_directories(${HDF5_INCLUDE_DIR})
endif(HDF5_FOUND)

find_package(LIBSBML)
if(LIBSBML_FOUND)
    # If LIBSBML_FOUND then we'll probably also need the LibXML2.
    add_definitions(-DUSE_SBML)
    find_package(LibXML2 REQUIRED)
    add_subdirectory(sbml)
    include_directories(${LIBSBML_INCLUDE_DIR})
elseif()
    add_definitions(-UUSE_SBML)
endif(LIBSBML_FOUND)

find_package(Termcap)
find_package(Readline)

if(READLINE_FOUND AND TERMCAP_FOUND)
    add_definitions(-DUSE_READLINE)
    include_directories(${Readline_INCLUDE_DIR})
endif()

# Openmpi
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})

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
add_subdirectory(external/debug)
add_subdirectory(external/tinyxml)
add_subdirectory(mesh)
add_subdirectory(mpi)
add_subdirectory(signeur)
add_subdirectory(ksolve)
add_subdirectory(hsolve)
add_subdirectory(diffusion)
add_subdirectory(device)
add_subdirectory(benchmarks)
add_subdirectory(kinetics)
add_subdirectory(synapse)

set(LIBRARIES ${LibBZip2_LIBRARIES})
if(HDF5_FOUND)
    list(APPEND LIBRARIES ${HDF5_LIBRARY})
endif()

if(LIBSBML_FOUND)
    list(APPEND LIBRARIES moose_sbml ${LIBSBML_LIBRARY})
endif()

if(LIBXML2_FOUND)
    list(APPEND LIBRARIES ${LibXML2_LIBRARIES})
endif()

if(GSL_FOUND)
    # Link gsl 
    list(APPEND LIBRARIES ${GSL_LIBRARIES})
endif()

if(READLINE_FOUND AND TERMCAP_FOUND)
    list(APPEND LIBRARIES ${Readline_LIBRARY} ${TERMCAP_LIBRARY})
endif()

###################################### LINKING #################################
set(MOOSE_LIBRARIES
    moose_builtins
    basecode
    msg
    benchmarks
    shell
    randnum
    scheduling
    moose_mpi
    utility 
    muparser
    biophysics 
    kinetics 
    hsolve 
    mesh
    signeur
    tinyxml
    synapse
    diffusion 
    device
    ksolve
    )

target_link_libraries(moose
    ${MOOSE_LIBRARIES}
    ${LOCAL_GSL_LIBRARIES}
    ${LIBRARIES}
    )

target_link_libraries(moose-bin 
    moose
    )

######################### BUILD PYMOOSE ########################################
set(BUILD_PYMOOSE 1)
if(BUILD_PYMOOSE)
    find_package(NumpyHeaders)

    if(PYTHON_NUMPY_FOUND)
        add_definitions(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
        include_directories(${PYTHON_NUMPY_INCLUDE_DIR})
        add_definitions(-DUSE_NUMPY)
    else()
        message(WARNING "Could not find numpy/arrayobject.h in usual places")
        add_definitions(-UUSE_NUMPY)
    endif()

    find_package(PythonDev REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    add_subdirectory(pymoose)
    add_library(_moose SHARED pymoose/moosemodule.cpp)

    if(LOCAL_GSL)
        add_dependencies(_moose GSL)
    endif()

    set_target_properties(_moose PROPERTIES COMPILE_DEFINITIONS "PYMOOSE")
    set_target_properties(_moose PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/python/moose"
        PREFIX ""
        )
    list(APPEND LIBRARIES ${PYTHON_LIBRARIES})
    target_link_libraries(
        _moose 
        moosepython  
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        "-Wl,--no-whole-archive"
        ${LIBRARIES}
        )

endif(BUILD_PYMOOSE)

######################### INSTALL ##############################################
install(TARGETS moose 
    LIBRARY DESTINATION lib
    COMPONENT moose-core
    )

install(TARGETS moose-bin
    RUNTIME DESTINATION bin
    COMPONENT moose-core
    )

install(SCRIPT python/InstallPyMoose.cmake
    COMPONENT moose-python
    )

## Install the icon
install(FILES gui/icons/moose_icon.png 
    DESTINATION share/moose
    COMPONENT moose-python
    )

# And the desktop file.
install(FILES scripts/moose.desktop
    DESTINATION share/applications
    COMPONENT moose-python
    )
