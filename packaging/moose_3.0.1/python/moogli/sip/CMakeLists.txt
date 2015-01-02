cmake_minimum_required(VERSION 2.8)
FILE(GLOB SIP_SRC *.sip)

FIND_PACKAGE(PyQt4 REQUIRED)
FIND_PACKAGE(PythonLibs REQUIRED)

SET(SIP_INCLUDES ${PYQT4_SIP_DIR})

SEPARATE_ARGUMENTS(PYQT4_SIP_FLAGS) # convert space separated values to a list
SET(SIP_CONCAT_PARTS 4)
SET(SIP_EXTRA_OPTIONS ${PYQT4_SIP_FLAGS})
IF (NOT BINDINGS_GLOBAL_INSTALL)
    SET(PYTHON_SITE_PACKAGES_DIR ${QGIS_DATA_DIR}/python)
ENDIF (NOT BINDINGS_GLOBAL_INSTALL)
IF (WITH_CUSTOM_WIDGETS)
    SET(PYUIC_WIDGET_PLUGIN_DIRECTORY ${PYQT4_MOD_DIR}/uic/widget-plugins/)
ENDIF (WITH_CUSTOM_WIDGETS)

FIND_PACKAGE(Qt4 REQUIRED COMPONENTS QtCore QtGui QtOpenGL)
FIND_PACKAGE(OpenSceneGraph REQUIRED)
FIND_PACKAGE(osgAnimation REQUIRED)

INCLUDE(${QT_USE_FILE})

FIND_PROGRAM(SIP_EXECUTABLE sip)
MESSAGE("++ SIP ${SIP_EXECUTABLE}")

include_directories(./include ${CMAKE_SOURCE_DIR} "."
    ${CMAKE_CURRENT_SOURCE_DIR}/moc
    )
include_directories(${OSGQT_INCLUDE_DIR})
include_directories(${SIP_INCLUDES} ${PYTHON_INCLUDE_DIRS})
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
include_directories(${OSGANIMATION_INCLUDE_DIR})

SET(GENERATED_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sip_mooglicmodule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sip_moogliMorphology.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sip_moogliMorphologyViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sip_moogliSelectInfo.cpp
    )

add_custom_command(OUTPUT ${GENERATED_SRCS}
    COMMAND ${SIP_EXECUTABLE} -c ${CMAKE_CURRENT_SOURCE_DIR} -I ${SIP_INCLUDES} 
        -e -g -x VendorID -t WS_X11 -x PyQt_NoPrintRangeBug -t Qt_4_8_0 -x Py_v3 -g
        ${CMAKE_CURRENT_SOURCE_DIR}/_moogli.sip
    DEPENDS ${SIP_SRC}
    COMMENT "Processing ${CMAKE_CURRENT_SOURCE_DIR}/_moogli.sip"
    VERBATIM
    )

ADD_LIBRARY(_moogli SHARED
    ${GENERATED_SRCS}
)

SET_TARGET_PROPERTIES(_moogli PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/python/moogli"
    PREFIX ""
    )

target_link_libraries(_moogli
    moc
    src
    QtCore
    QtGui
    QtOpenGL           
    osg
    osgFX
    osgUtil
    osgFX
    osgGA
    osgQt
    osgAnimation
    osgViewer
    osgQt
    osgManipulator
    osgText
    )
