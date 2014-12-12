cmake_minimum_required(VERSION 2.8)
project(moose)
set(CMAKE_INSTALL_PREFIX /usr)

# This variable should point to the source code.
set(MOOSE_SRC_DIRNAME moose_3.0.1)

## Project related macros
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${MOOSE_SRC_DIRNAME})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules")

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
set(INSTALL_LIB_DIR lib CACHE PATH "${CMAKE_INSTALL_PREFIX}/lib")
set(INSTALL_BIN_DIR bin CACHE PATH "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_INCLUDE_DIR include CACHE PATH "${CMAKE_INSTALL_PREFIX}/include/")

# DONT DELETE IT.
LINK_DIRECTORIES(${GSL_INSTALL_DIR}/lib)

########################### ARCHITECTURE SPECIFIC ##############################
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if(LIB64)
    set(LIBSUFFIX "64")
else()
    set(LIBSUFFIX "")
endif()

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
set(ENABLE_UNIT_TESTS 1)
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
endif()

## Setup hdf5
find_package(HDF5)
if(HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIR})
else()
    MESSAGE("++ Using local HDF5")
    SET(LOCAL_HDF 1)
    set(HDF5_FOUND)
endif(HDF5_FOUND)
add_definitions(-DUSE_HDF5)


find_package(GSL 1.16)
# If installed version of gsl is not 1.16 then download and build gsl locally.
# THIS IS USEFULL TO BUILD MOOSE FOR VARIOUS DISTRIBUTIONS.
if(NOT GSL_FOUND)
    MESSAGE("++ No GSL found. Using local gsl")
    set(LOCAL_GSL 1)
else()
    if(${GSL_VERSION} VERSION_LESS 1.16)
        MESSAGE("+ Trouble. Needed version 1.16.")
        MESSAGE("+-- I'll go ahead, download gsl-1.16 and build it for moose. Make
        sure you have proxy environment variables set.")
        SET(LOCAL_GSL 1)
    endif()
endif()

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/dependencies)
if(LOCAL_GSL)
    MESSAGE("++ GSL include path ${GSL_HEADER_PATHS}")
    set(GSL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/_libgsl)
    include(ExternalProject)
    ExternalProject_Add(GSL
        PREFIX ${EXTERNAL_DIR}
        URL ${EXTERNAL_DIR}/gsl-1.16.tar.gz
        CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_C_FLAGS=-fpic -DCMAKE_INSTALL_PREFIX:PATH=${GSL_INSTALL_DIR} -DBUILD_SHARED_LIBS=OFF
        )
    include_directories(${GSL_INSTALL_DIR}/include)
    set(LOCAL_GSL_LIBRARIES ${GSL_INSTALL_DIR}/lib/libgsl.a 
        ${GSL_INSTALL_DIR}/lib/libgslcblas.a
        )
endif()

## SBML
set(SBML_INSTALL_DIR ${CMAKE_SOURCE_DIR}/_libsbml)
find_package(LIBSBML)
if(NOT LIBSBML_FOUND)
    MESSAGE("+ LIBSBML NOT FOUND. Using local SBML")
    SET(LOCAL_SBML 1)
    include_directories(${SBML_INSTALL_DIR}/include)
else()
    SET(LOCAL_SBML 0)
    MESSAGE("++ SBML found")
    include_directories(${LIBSBML_INCLUDE_DIR})
endif()
add_definitions(-DUSE_SBML)

set(HDF_INSTALL_DIR ${CMAKE_SOURCE_DIR}/_libhdf)
include_directories(${HDF_INSTALL_DIR}/include)
if(LOCAL_HDF)
    include(ExternalProject)
    ExternalProject_Add(LIBHDF
        PREFIX ${EXTERNAL_DIR}
        URL ${EXTERNAL_DIR}/hdf5-1.8.14.tar.gz
        CONFIGURE_COMMAND CXXFLAGS=-fPIC ../LIBHDF/configure --prefix=${HDF_INSTALL_DIR}
        --enable-cxx --enable-static --disable-shared --with-pic --enable-production
        BUILD_COMMAND make 
        INSTALL_COMMAND make install
        #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${HDF_INSTALL_DIR}
        #-DBUILD_SHARED_LIBS:BOOL=OFF
        #-DBUILD_STATIC_EXECS:BOOL=ON
        #-DHDF5_BUILD_CPP_LIB:BOOL=ON
        #-C Release
        #-DWITH_BZIP2:BOOL=ON 
        #-DWITH_LIBXML:BOOL=ON
        )
    set(HDF5_LIBRARY ${HDF_INSTALL_DIR}/lib/libhdf5_cpp.a
        ${HDF_INSTALL_DIR}/lib/libhdf5.a
        )
endif()


if(LOCAL_SBML)
    include(ExternalProject)
    ExternalProject_Add(LIBSBML
        PREFIX ${EXTERNAL_DIR}
        URL ${EXTERNAL_DIR}/libSBML-5.10.2-core-src.tar.gz
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${SBML_INSTALL_DIR}
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DWITH_BZIP2:BOOL=ON 
        -DWITH_LIBXML:BOOL=ON
        )
    set(LIBSBML_LIBRARY ${SBML_INSTALL_DIR}/lib/libsbml-static.a)
endif()

################################### SETUP BUILD ################################

## DoxyGen based documentation.

find_package(Doxygen)
IF(DOXYGEN_FOUND)
    MESSAGE("++ Doxygen found. Creating document")
    SET(DOXYGEN_INPUT ${PROJECT_SOURCE_DIR}/Doxyfile)
    SET(DOXYGEN_OUTPUT ${PROJECT_SOURCE_DIR}/docs/api/index.html)
    SET(API_DOC_DIR ${PROJECT_SOURCE_DIR}/docs/api)

    ADD_CUSTOM_COMMAND(
        OUTPUT ${DOXYGEN_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
        COMMAND rm -rf ${API_DOC_DIR}
        COMMAND mkdir ${API_DOC_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${DOXYGEN_INPUT}
        )

    ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

    ADD_CUSTOM_TARGET(apidoc_forced
        COMMAND rm -rf ${API_DOC_DIR}
        COMMAND mkdir ${API_DOC_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

ENDIF (DOXYGEN_FOUND)


include_directories(${PROJECT_SOURCE_DIR}/msg ${PROJECT_SOURCE_DIR}/basecode)
add_executable(moose-bin ${PROJECT_SOURCE_DIR}/basecode/main.cpp)
# Without these two macros, main function will not get compiled."
set_target_properties(moose-bin PROPERTIES COMPILE_FLAGS "-UMOOSE_LIB -UPYMOOSE")

add_library(moose SHARED ${PROJECT_SOURCE_DIR}/basecode/main.cpp)
set_target_properties(moose PROPERTIES COMPILE_FLAGS  "-DMOOSE_LIB")
set_target_properties(moose PROPERTIES VERSION 3.0.1 SOVERSION 3)

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


if(LIBSBML_FOUND)
    # If LIBSBML_FOUND then we'll probably also need the LibXML2.
    find_package(LibXML2 REQUIRED)
endif(LIBSBML_FOUND)

find_package(Termcap)
find_package(Readline)

if(READLINE_FOUND AND TERMCAP_FOUND)
    add_definitions(-DUSE_READLINE)
    include_directories(${Readline_INCLUDE_DIR})
endif()

# Openmpi
find_package(MPI REQUIRED)
INCLUDE_DIRECTORIES(${MPI_INCLUDE_DIR})


# Add subdirectroeis
add_subdirectory(${PROJECT_SOURCE_DIR}/basecode)
add_subdirectory(${PROJECT_SOURCE_DIR}/biophysics)
add_subdirectory(${PROJECT_SOURCE_DIR}/msg)
add_subdirectory(${PROJECT_SOURCE_DIR}/shell)
add_subdirectory(${PROJECT_SOURCE_DIR}/randnum)
add_subdirectory(${PROJECT_SOURCE_DIR}/scheduling)
add_subdirectory(${PROJECT_SOURCE_DIR}/builtins)
add_subdirectory(${PROJECT_SOURCE_DIR}/utility)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/muparser)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/debug)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/tinyxml)
add_subdirectory(${PROJECT_SOURCE_DIR}/mesh)
add_subdirectory(${PROJECT_SOURCE_DIR}/mpi)
add_subdirectory(${PROJECT_SOURCE_DIR}/signeur)
add_subdirectory(${PROJECT_SOURCE_DIR}/ksolve)
add_subdirectory(${PROJECT_SOURCE_DIR}/hsolve)
add_subdirectory(${PROJECT_SOURCE_DIR}/diffusion)
add_subdirectory(${PROJECT_SOURCE_DIR}/device)
add_subdirectory(${PROJECT_SOURCE_DIR}/benchmarks)
add_subdirectory(${PROJECT_SOURCE_DIR}/kinetics)
add_subdirectory(${PROJECT_SOURCE_DIR}/synapse)
add_subdirectory(${PROJECT_SOURCE_DIR}/sbml)
add_subdirectory(${PROJECT_SOURCE_DIR}/intfire)

set(LIBRARIES ${LibBZip2_LIBRARIES})
list(APPEND LIBRARIES ${HDF5_LIBRARY})

if(LIBSBML_FOUND)
    list(APPEND LIBRARIES moose_sbml ${LIBSBML_LIBRARY})
else()
    list(APPEND LIBRARIES moose_sbml ${LIBSBML_LIBRARY})
endif()

if(LIBXML2_FOUND)
    list(APPEND LIBRARIES ${LibXML2_LIBRARIES})
endif()

# Link gsl 
if(LOCAL_GSL)
    list(APPEND LIBRARIES ${LOCAL_GSL_LIBRARIES})
else(GSL_FOUND)
    list(APPEND LIBRARIES ${GSL_LIBRARIES})
endif()

## Append bz2 and xml2 to the dependencies because of sbml.
list(APPEND LIBRARIES bz2 xml2)

if(READLINE_FOUND AND TERMCAP_FOUND)
    list(APPEND LIBRARIES ${Readline_LIBRARY} ${TERMCAP_LIBRARY})
endif()


###################################### LINKING #################################
MESSAGE("++ Linking: ${MOOSE_LIBRARIES}")
MESSAGE(" ++ ${LIBRARIES}")

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
    intfire
    signeur
    tinyxml
    synapse
    diffusion 
    device
    ksolve
    )

target_link_libraries(moose
    ${MOOSE_LIBRARIES}
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
    add_subdirectory(${PROJECT_SOURCE_DIR}/pymoose)
    add_library(_moose SHARED ${PROJECT_SOURCE_DIR}/pymoose/moosemodule.cpp)

    if(LOCAL_GSL)
        add_dependencies(_moose GSL)
    endif()

    set_target_properties(_moose PROPERTIES COMPILE_DEFINITIONS "PYMOOSE")
    set_target_properties(_moose PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/python/moose"
        PREFIX ""
        )
    list(APPEND LIBRARIES ${PYTHON_LIBRARIES})
    target_link_libraries(
        _moose 
        # This library is in pymoose folder.
        moosepython  
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        ${LIBRARIES}
        "-Wl,--no-whole-archive"
        )

endif(BUILD_PYMOOSE)

######################### INSTALL ##############################################
install(TARGETS moose
    LIBRARY DESTINATION lib
    COMPONENT moose-core
    )

install(TARGETS moose-bin
    DESTINATION bin
    COMPONENT moose-core
    )

install(PROGRAMS ${PROJECT_SOURCE_DIR}/scripts/moose
    DESTINATION bin
    COMPONENT moose-core
    )

install(FILES ${PROJECT_SOURCE_DIR}/scripts/moose
    DESTINATION bin
    COMPONENT moose-core
    )

install(PROGRAMS ${PROJECT_SOURCE_DIR}/scripts/moosegui
    DESTINATION bin
    COMPONENT moose-gui
    )

## Install the icon
install(FILES ${PROJECT_SOURCE_DIR}/gui/icons/moose_icon.png 
    DESTINATION share/moose
    COMPONENT moose-python
    )

# And the desktop file.
install(FILES ${PROJECT_SOURCE_DIR}/scripts/moose.desktop
    DESTINATION share/applications
    COMPONENT moose-python
    )

# And the gui
install(DIRECTORY ${PROJECT_SOURCE_DIR}/gui
    DESTINATION lib/moose
    COMPONENT moose-gui
    )

# and the python dependencies
install(DIRECTORY ${PROJECT_SOURCE_DIR}/python
    DESTINATION lib/moose
    COMPONENT moose-python
    )

## The demos goes here.
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Demos
    DESTINATION share/moose
    COMPONENT moose-python
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/docs
    DESTINATION share/doc/moose
    COMPONENT moose-core
    COMPONENT moose-python
    )

########################## PACKAGING #########################################

set(CPACK_GENERATOR "DEB;RPM;STGZ")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
#  We need to compile python scripts on the installation  host.
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh")
set(CPACK_PACKAGE_CONTACT "dilawars@ncbs.res.in")
#set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MOOSE, The Neural Simulator")
set(CPACK_PACKAGE_VERSION "3.0.1")
set(CPACK_SYSTEM_NAME "i386")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VENDOR "NCBS Bangalore")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh <dilawars@ncbs.res.in>")
if(WIN32)
    MESSAGE("++ Currently only Unix is supported")
else()
    set(CPACK_STRIP_FILES moose)
endif()
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS 
    "openmpi-bin
    , python-numpy, python-matplotlib
    , python-qt4, openscenegraph
    , python-tornado, python-suds, python-nose, python-setuptools
    , libxml2"
)

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_SOURCE_DIR}/debian/postinst;${CMAKE_SOURCE_DIR}/debian/prerm;"
    )
include(CPack)
