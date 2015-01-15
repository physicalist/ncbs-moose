CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
## Now setting up project.
project(moose)

## Here we check for available cmake, if anything less than 2.8 is found,
## build a cmake 3.0.1 in source tree and use it
#MESSAGE("++ CMAKE VERSION IS : ${CMAKE_VERSION}")
#IF(${CMAKE_VERSION} VERSION_GREATER 2.8.0.0)
    #MESSAGE("++ Cool. CMake 2.8 or higher is found. Continuing ...")
#else()
    #MESSAGE("++Version ${CMAKE_VERSION} found. Not good enough! Building a local copy")
    #SET(CMAKE_LOCAL_INSTALL_PATH ${CMAKE_SOURCE_DIR})
    #SET(CMAKE_LOCAL_SRC_DIR ${CMAKE_SOURCE_DIR}/dependencies/cmake-3.1.0)
    #SET(CMAKE_LOCAL_BUILD_DIR ${CMAKE_BINARY_DIR}/_cmake)
    #FILE(MAKE_DIRECTORY ${CMAKE_LOCAL_BUILD_DIR})
    #MESSAGE("++ Building in ${CMAKE_LOCAL_SRC_DIR}")
    #execute_process(COMMAND ${CMAKE_LOCAL_SRC_DIR}/configure --prefix=${CMAKE_LOCAL_INSTALL_PATH}
        #WORKING_DIRECTORY ${CMAKE_LOCAL_BUILD_DIR}
        #RESULT_VARIABLE CMAKE_BUILD_RESULT
        #)
    #execute_process(COMMAND make 
        #WORKING_DIRECTORY ${CMAKE_LOCAL_BUILD_DIR}
        #)
    #execute_process(COMMAND make install
        #WORKING_DIRECTORY ${CMAKE_LOCAL_BUILD_DIR}
        #)
    #SET(CMAKE_COMMAND ${CMAKE_LOCAL_INSTALL_PATH}/bin/cmake)
    #SET(CPACK_COMMAND ${CMAKE_LOCAL_INSTALL_PATH}/bin/cpack)
    #SET(CTEST_COMMAND ${CMAKE_LOCAL_INSTALL_PATH}/bin/ctest)
#endif()  ## CMAKE_VERSION_CHECK


set(CMAKE_INSTALL_PREFIX /usr)
OPTION(WHOLE_ARCHIVE "Link all libraries in one big package" OFF)

# This variable should point to the source code.
set(MOOSE_SRC_DIRNAME moose_3.0.1)

## Project related macros
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${MOOSE_SRC_DIRNAME})
## Modules are always in branch 
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
MESSAGE("++ Testing for architecture: 64 bit or 32 bit")
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    MESSAGE("++ 64 bit architecture")
    set(LIBSUFFIX "64")
    set(PKGARCH "amd64")
    set(RPMPKGARCH "x86_64")
else()
    MESSAGE("++ 32 bit architecture")
    set(LIBSUFFIX "")
    set(PKGARCH "i386")
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

## Enable 2011 support if available.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


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
if(FOUND_HDF5)
    include_directories(${HDF5_INCLUDE_DIR})
else()
    MESSAGE("++ Using local HDF5")
    SET(LOCAL_HDF 1)
    set(FOUND_HDF5)
endif(FOUND_HDF5)
add_definitions(-DUSE_HDF5)


find_package(GSL 1.16)
# If installed version of gsl is not 1.16 then download and build gsl locally.
# THIS IS USEFULL TO BUILD MOOSE FOR VARIOUS DISTRIBUTIONS.
if(NOT GSL_FOUND)
    MESSAGE("++ No GSL found. Using local gsl")
    set(LOCAL_GSL 1)
else()
    if(${GSL_VERSION} VERSION_LESS 1.16)
        MESSAGE("+ Trouble. Needed version 1.16. I'll now build GSL locally and
        link it with MOOSE")
        SET(LOCAL_GSL 1)
    endif()
endif()

## External projects
OPTION(BUILD_MOOGLI "Build moogli extension." ON)
IF(BUILD_MOOGLI)
    MESSAGE("++ Building MOOGLI")
    SET(MOOGL_SRC_PATH ${PROJECT_SOURCE_DIR}/python/moogli)
    SET(MOOGLI_BUILD_PATH ${CMAKE_BINARY_DIR}/_moogli)
    FILE(MAKE_DIRECTORY ${MOOGLI_BUILD_PATH})
    ADD_CUSTOM_TARGET(moogli ALL
        COMMAND ${CMAKE_COMMAND} -DWHOLE_ARHIVE=${WHOLE_ARCHIVE} -DCMAKE_INSTALL_PREFIX=${MOOGLI_BUILD_PATH} ${MOOGL_SRC_PATH}
        COMMAND make install
        WORKING_DIRECTORY ${MOOGLI_BUILD_PATH}
        COMMENT "Building MOOGLI"
        VERBATIM
    ) 
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/python/moogli/include)
    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/python/moogli)
else()
    MESSAGE("++ NOT building MOOGLI")
endif()

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/dependencies)
if(LOCAL_GSL)
    SET(GSL_TAR_FILE ${CMAKE_SOURCE_DIR}/dependencies/gsl-1.16.tar.gz)
    SET(GSL_BUILD_DIR ${CMAKE_BINARY_DIR}/_libgsl)
    SET(GSL_SRC_DIR ${GSL_BUILD_DIR}/gsl-master)
    set(GSL_INSTALL_DIR ${CMAKE_BINARY_DIR}/_libgsl)
    FILE(MAKE_DIRECTORY ${GSL_BUILD_DIR})

    execute_process(COMMAND tar xzf ${GSL_TAR_FILE} -C ${GSL_BUILD_DIR})

    ADD_CUSTOM_COMMAND(OUTPUT _libgsl_tgt.out
        COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${GSL_INSTALL_DIR}
        -DBUILD_SHARED_LIBRARIES:BOOL=OFF
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
        -DCMAKE_C_FLAGS=-fPIC -O3
        -DCMAKE_CXX_FLAGS=-fPIC -O3
        ${GSL_SRC_DIR}
        COMMAND make install
        WORKING_DIRECTORY ${GSL_SRC_DIR}
        COMMENT("++ Building GSL using cmake.")
        VERBATIM
        )
    ADD_CUSTOM_TARGET(_libgsl_tgt ALL DEPENDS _libgsl_tgt.out)

    include_directories(${GSL_INSTALL_DIR}/include)
    set(LOCAL_GSL_LIBRARIES ${GSL_INSTALL_DIR}/lib/libgsl.a 
        ${GSL_INSTALL_DIR}/lib/libgslcblas.a
        )
endif()

## SBML
find_package(LIBSBML)
if(NOT LIBSBML_FOUND)
    MESSAGE("+ LIBSBML NOT FOUND. Using local SBML")
    SET(LOCAL_SBML 1)
else()
    SET(LOCAL_SBML 0)
    MESSAGE("++ SBML found")
    include_directories(${LIBSBML_INCLUDE_DIR})
endif()
add_definitions(-DUSE_SBML)

if(LOCAL_HDF)
    MESSAGE("++ Building local hdf")
    set(HDF_TAR_FILE ${CMAKE_SOURCE_DIR}/dependencies/hdf5-1.8.14.tar.gz)
    SET(HDF_BUILD_DIR ${CMAKE_BINARY_DIR}/_hdf)
    SET(HDF_SRC_DIR ${CMAKE_BINARY_DIR}/_hdf/hdf5-1.8.14)
    set(HDF_INSTALL_DIR ${CMAKE_BINARY_DIR}/_libhdf)
    FILE(MAKE_DIRECTORY ${HDF_BUILD_DIR})
    execute_process(COMMAND tar xzf ${HDF_TAR_FILE} -C ${HDF_BUILD_DIR})

    ADD_CUSTOM_COMMAND(OUTPUT _libhdf.out
        COMMAND ${HDF_SRC_DIR}/configure --prefix=${HDF_INSTALL_DIR}
        --enable-cxx --enable-static --disable-shared --with-pic --enable-production
        --libdir=${HDF_INSTALL_DIR}/lib${LIBSUFFIX}
        COMMAND make 
        COMMAND make install
        WORKING_DIRECTORY ${HDF_SRC_DIR}
        COMMENT "+++ Building Local HDF"
        VERBATIM
        #CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${HDF_INSTALL_DIR}
        #-DBUILD_SHARED_LIBS:BOOL=OFF
        #-DBUILD_STATIC_EXECS:BOOL=ON
        #-DHDF5_BUILD_CPP_LIB:BOOL=ON
        #-C Release
        #-DWITH_BZIP2:BOOL=ON 
        #-DWITH_LIBXML:BOOL=ON
        )

    ADD_CUSTOM_TARGET(_libhdf ALL DEPENDS _libhdf.out)

    set(HDF5_LIBRARY ${HDF_INSTALL_DIR}/lib${LIBSUFFIX}/libhdf5_cpp.a
        ${HDF_INSTALL_DIR}/lib${LIBSUFFIX}/libhdf5.a
        )
    include_directories(${HDF_INSTALL_DIR}/include)
endif()


if(LOCAL_SBML)
    MESSAGE("+++ Building local sbml")
    SET(SBML_TAR_FILE ${CMAKE_SOURCE_DIR}/dependencies/libSBML-5.10.2-core-src.tar.gz)
    SET(SBML_INSTALL_DIR ${CMAKE_BINARY_DIR}/_libsbml)
    FILE(MAKE_DIRECTORY ${SBML_INSTALL_DIR})

    execute_process(COMMAND tar xzf ${SBML_TAR_FILE} -C ${SBML_INSTALL_DIR})

    SET(SBML_SRC_DIR ${SBML_INSTALL_DIR}/libsbml-5.10.2)
    ADD_CUSTOM_COMMAND(OUTPUT _libsbml.out
        COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${SBML_INSTALL_DIR}
        -DBUILD_SHARED_LIBS=OFF
        -DWITH_BZIP2=ON
        -DWITH_LIBXML=ON ${SBML_SRC_DIR}
        COMMAND make install
        WORKING_DIRECTORY ${SBML_INSTALL_DIR}
        VERBATIM
        )
        
    ADD_CUSTOM_TARGET(_libsml ALL DEPENDS _libsbml.out)
    MESSAGE("+++ LIBSBML_LIBRARY: ${LIBSBML_LIBRARY}")
    set(LIBSBML_LIBRARY ${SBML_INSTALL_DIR}/lib/libsbml-static.a) 
    include_directories(${SBML_INSTALL_DIR}/include)
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
        COMMAND mkdir -p ${API_DOC_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${DOXYGEN_INPUT}
        )

    ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

    ADD_CUSTOM_TARGET(apidoc_forced
        COMMAND rm -rf ${API_DOC_DIR}
        COMMAND mkdir -p ${API_DOC_DIR}
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

if(LOCAL_GSL)
    #MESSAGE("++ Adding dependencies to moose-bin and moose: _libgsl_tgt")
    add_dependencies(moose-bin _libgsl_tgt)
    add_dependencies(moose _libgsl_tgt)
endif()

if(LOCAL_HDF)
    #MESSAGE(" ++ Adding dependencies to moose-bin and moose: _libhdf")
    add_dependencies(moose-bin _libhdf)
    add_dependencies(moose _libhdf)
endif()

if(LOCAL_SBML)
    add_dependencies(moose-bin _libsml)
    add_dependencies(moose _libsml)
endif()

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
if(MPI_FOUND)
    MESSAGE("++ OpenMPI is found: ${MPI_INCLUDE_DIR}")
    INCLUDE_DIRECTORIES(${MPI_INCLUDE_DIR})
    ADD_DEFINITIONS(-DUSE_MPI)
else()
    MESSAGE("++ OpenMPI not found")
    ADD_DEFINITIONS(-UUSE_MPI)
endif()



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

if(MPI_FOUND)
    list(APPEND LIBRARIES ${MPI_CXX_LIBRARY})
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
    COMPONENT moose
    )

install(TARGETS moose-bin
    DESTINATION bin
    COMPONENT moose
    )

install(PROGRAMS ${PROJECT_SOURCE_DIR}/scripts/moose
    DESTINATION bin
    COMPONENT moose
    )

install(FILES ${PROJECT_SOURCE_DIR}/scripts/moose
    DESTINATION bin
    COMPONENT moose
    )

install(PROGRAMS ${PROJECT_SOURCE_DIR}/scripts/moosegui
    DESTINATION bin
    COMPONENT moose
    )

## Install the icon
install(FILES ${PROJECT_SOURCE_DIR}/gui/icons/moose_icon.png 
    DESTINATION share/icons
    COMPONENT moose
    RENAME moose.png
    )

# And the desktop file.
install(FILES ${PROJECT_SOURCE_DIR}/scripts/moose.desktop
    DESTINATION share/applications
    COMPONENT moose
    )

# And the gui
install(DIRECTORY ${PROJECT_SOURCE_DIR}/gui
    DESTINATION lib/moose
    COMPONENT moose
    )

# and the python dependencies
SET(PYMOOSE_TEMP_DIR "/usr/lib/moose")

install(DIRECTORY ${PROJECT_SOURCE_DIR}/python/moose
    DESTINATION ${PYMOOSE_TEMP_DIR}
    COMPONENT moose
    )
install(DIRECTORY ${PROJECT_SOURCE_DIR}/python/libmumbl
    DESTINATION ${PYMOOSE_TEMP_DIR}
    COMPONENT moose
    )
install(FILES ${PROJECT_SOURCE_DIR}/python/setup.py
    DESTINATION ${PYMOOSE_TEMP_DIR}
    COMPONENT moose
    )

## The demos goes here. The ending backslash after Demos/ allows us to change
## the name of target.
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Demos/
    DESTINATION share/moose/demos
    COMPONENT moose
    )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/docs
    DESTINATION share/doc/moose
    COMPONENT moose
    )

if(BUILD_MOOGLI)
### And here is moogli
SET(MOOGLI_TEMP_DIR "/usr/lib/moogli")

SET(DEBIAN_POSTINST_PATH ${CMAKE_SOURCE_DIR}/debian-scripts/postinst)
SET(DEBIAN_PRERM_PATH ${CMAKE_SOURCE_DIR}/debian-scripts/prerm)

MESSAGE("++ Generating postinst file ${DEBIAN_POSTINST_PATH} for MOOSE/MOOGLI DEB Package")

FILE(WRITE ${DEBIAN_POSTINST_PATH} "#!/bin/sh
# This script is auto-generated by cmake. Don't modify it here
set -e
mkdir -p /etc/moose
mkdir -p /etc/moogli

case \"$1\" in
    configure)
        ( cd ${PYMOOSE_TEMP_DIR} && python setup.py install --record /etc/moose/installed_files.txt  )       
        ( cd ${MOOGLI_TEMP_DIR} && python setup-moogli.py install --record /etc/moogli/installed_files.txt )
    ;;

    abort-upgrade|abort-remove|abort-deconfigure)
    ;;

    *)
        echo \"postinst called with unknown argument \\`$1'\" >&2
        exit 1
    ;;
esac
exit 0
"
)

SET(DETELE_PACKAGE_DIR_STEPS "
        if [ -d /usr/share/doc/moose ]; then 
            rm -R /usr/share/doc/moose
        fi

        if [ -d /usr/share/moose ]; then
            rm -R /usr/share/moose 
        fi

        if [ -d /etc/moose ]; then
            rm -R /etc/moose
        fi

        if [ -d /etc/moogli ]; then
            rm -R /etc/moogli 
        fi

        if [ -f /usr/share/icons/moose_icon.png ]; then
            rm -f /usr/share/icons/moose_icon.png 
        fi
        "
        )

MESSAGE("++ Generating prerm script to ${DEBIAN_PRERM_PATH} for MOOSE/MOOGLI DEB package")
FILE(WRITE ${DEBIAN_PRERM_PATH} "#!/bin/sh
# prerm script for moose and moogli. This script is autogenerated using cmake.
# Don't modify it here.

set -e
case \"$1\" in
    remove|upgrade|deconfigure)
        set +e 
        pip uninstall -y pymoose
        pip uninstall -y moogli
        set -e
        if [ -d /etc/moose ]; then
            cd /etc/moose && cat installed_files.txt | xargs rm -rf
        fi
        if [ -d /etc/moogli ]; then
            cd /etc/moogli && cat installed_files.txt | xargs rm -rf 
        fi
        ${DETELE_PACKAGE_DIR_STEPS}
    ;;

    failed-upgrade)
    ;;

    *)
        echo \"prerm called with unknown argument \\`$1'\" >&2
        exit 1
    ;;
esac
exit 0
"
)

SET(RPM_POSTINST_PATH ${CMAKE_SOURCE_DIR}/rpm-scripts/postinst)
SET(RPM_PRERM_PATH ${CMAKE_SOURCE_DIR}/rpm-scripts/prerm)
MESSAGE("++ Generating postinnst ${RPM_POSTINST_PATH} and prerm ${RPM_PRERM_PATH} for RPM")

FILE(WRITE ${RPM_POSTINST_PATH} "#!/bin/sh
set -e
mkdir -p /etc/moose
mkdir -p /etc/moogli
( cd ${PYMOOSE_TEMP_DIR} && python setup.py install --record /etc/moose/installed_files.txt )
( cd ${MOOGLI_TEMP_DIR} && python setup-moogli.py install --record /etc/moogli/installed_files.txt )
"
)

FILE(WRITE ${RPM_PRERM_PATH} "#!/bin/sh
set +e
if [ -d /etc/moose ]; then
    cd /etc/moose && cat installed_files.txt | xargs rm -rf 
fi
if [ -d /etc/moogli ]; then
    cd /etc/moogli && cat installed_files.txt | xargs rm -rf 
fi
"
)

install(FILES ${MOOGLI_BUILD_PATH}/_moogli/moogli.so
    DESTINATION ${MOOGLI_TEMP_DIR}/moogli
    COMPONENT moose
    )

install(FILES ${PROJECT_SOURCE_DIR}/python/moogli/__init__.py
    DESTINATION ${MOOGLI_TEMP_DIR}/moogli
    COMPONENT moose
    )

install(FILES ${PROJECT_SOURCE_DIR}/python/setup-moogli.py
    DESTINATION ${MOOGLI_TEMP_DIR}
    COMPONENT moose
    )

endif(BUILD_MOOGLI)

########################## PACKAGING #########################################

set(CPACK_GENERATOR "DEB;RPM;STGZ")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

#  We need to compile python scripts on the installation  host.
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh")
set(CPACK_PACKAGE_CONTACT "dilawars@ncbs.res.in")
#set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MOOSE, The Neural Simulator")
set(CPACK_PACKAGE_VERSION "3.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VENDOR "NCBS Bangalore")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh <dilawars@ncbs.res.in>")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${PKGARCH}")
set(CPACK_PACKAGE_FILE_NAME "moose-${CPACK_PACKAGE_VERSION}-Linux-${PKGARCH}")
set(CPACK_STRIP_FILES moose)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS 
    "openmpi-bin
    , python-matplotlib
    , python-qt4, openscenegraph
    , python-tornado, python-suds, python-nose, python-setuptools
    , libxml2"
)

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${DEBIAN_POSTINST_PATH};${DEBIAN_PRERM_PATH};"
    )

## RPM 
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${RPMPKGARCH}")
set(CPACK_RPM_SPEC_INSTALL_POST "/bin/true")
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${RPM_POSTINST_PATH}")
set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${RPM_PRERM_PATH}")

# This has to be a single line.
set(CPACK_RPM_PACKAGE_REQUIRES
    "openmpi-devel python-matplotlib-qt4 python-qt4  python-tornado python-suds python-nose python-setuptools libxml2"
    )

# A workaround in RPM
set(CPACK_RPM_SPEC_MORE_DEFINE "%define ignore \#")
set(CPACK_RPM_USER_FILELIST "%ignore /" "%ignore /usr" "%ignore /usr/share" 
    "%ignore /usr/share/applications"
    "%ignore /tmp"
    )


include(CPack)
