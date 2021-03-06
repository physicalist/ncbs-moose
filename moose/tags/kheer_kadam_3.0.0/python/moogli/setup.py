#!/usr/bin/env python

from distutils.core import setup, Extension
import sipdistutils
import sys
import os
# print(sys.argv)
# RESOURCES -
# https://docs.python.org/2.7/distutils/apiref.html?highlight=setup#distutils.core.setup
# http://pyqt.sourceforge.net/Docs/sip4/distutils.html#ref-distutils
# https://wiki.python.org/moin/EmbedingPyQtTutorial


# list of object files to be passed to the linker.
# These files must not have extensions, as the default extension for the compiler is used.
os.environ["CC"]="g++"
extra_objects           =   [
                            ]

# list of libraries to link against
libraries               =   [ "QtCore"
                            , "QtGui"
                            , "QtOpenGL"
                            , "osg"
                            , "osgFX"
                            , "osgUtil"
                            , "osgFX"
                            , "osgGA"
                            , "osgQt"
                            , "osgAnimation"
                            , "osgViewer"
                            , "osgQt"
                            , "osgManipulator"
                            , "osgText"
                            ]

# list of directories to search for libraries at link-time
library_dirs = []
library_dirs            =   [ "/home/aviral/OpenSceneGraph-3.2.1-rc2/build/lib"
                            ]

# # list of directories to search for shared (dynamically loaded) libraries at run-time
runtime_library_dirs = []
runtime_library_dirs    =   [ "/home/aviral/OpenSceneGraph-3.2.1-rc2/build/lib/"
                            , "/home/aviral/OpenSceneGraph-3.2.1-rc2/build/lib/osgPlugins-3.2.1/"
                            ]

# additional command line options for the compiler command line
extra_compile_args      =   [ "-O3"
                            , "-std=c++0x"
                            , "-Wno-reorder"
                            ]

# additional command line options for the linker command line
extra_link_args         =   [ "-fPIC"
                            , "-shared"
                            ]

#specify include directories to search
include_dirs            =   [ "."
                            , "include"
                            , "/usr/share/sip/PyQt4/"
                            , "/usr/include/qt4/"
                            , "/usr/include/qt4/QtCore/"
                            , "/usr/include/qt4/QtGui/"
                            , "/usr/include/qt4/QtOpenGL/"
                            , "/usr/share/sip/PyQt4/QtCore/"
                            , "/usr/share/sip/PyQt4/QtGui/"
                            , "/usr/share/sip/PyQt4/QtOpenGL/"
                            , "/home/aviral/OpenSceneGraph-3.2.1-rc2/include"
                            ]

# define pre-processor macros
define_macros           =   [
                            ]

# undefine pre-processor macros
undef_macros            =   [
                            ]


moogli = Extension( name                  =   "_moogli"
                  , sources               =   [ "src/core/Morphology.cpp"
                                              , "src/core/Compartment.cpp"
                                              , "src/core/SelectInfo.cpp"
                                              , "src/core/MorphologyViewer.cpp"
                                              , "moc/MorphologyViewer.moc.cpp"
                                              , "src/core/Selector.cpp"
                                              , "src/mesh/CylinderMesh.cpp"
                                              , "src/mesh/SphereMesh.cpp"
                                              , "src/utility/conversions.cpp"
                                              , "src/utility/record.cpp"
                                              , "src/utility/stringutils.cpp"
                                              , "src/utility/utilities.cpp"
                                              , "src/constants.cpp"
                                              , "src/globals.cpp"
                                              , "sip/_moogli.sip"
                                              ]
                  , include_dirs          =   include_dirs
                  , extra_compile_args    =   extra_compile_args
                  , extra_link_args       =   extra_link_args
                  , library_dirs          =   library_dirs
                  , libraries             =   libraries
                  , extra_objects         =   extra_objects
                  , runtime_library_dirs  =   runtime_library_dirs
                  , define_macros         =   define_macros
                  , undef_macros          =   undef_macros
                  )


extensions = [ moogli
             ]


setup( name             =   'moogli'
     , version          =   '1.0'
     , author           =   'Aviral Goel'
     , author_email     =   'aviralg@ncbs.res.in'
     , maintainer       =   'Aviral Goel'
     , maintainer_email =   'aviralg@ncbs.res.in'
     , url              =   ''
     , download_url     =   ''
     , description      =   'A Neuronal Simulator'
     , long_description =   ''
     , classifiers      =   [ 'Development Status :: Beta'
                            , 'Environment :: GUI'
                            , 'Environment :: Desktop'
                            , 'Intended Audience :: End Users/Desktop'
                            , 'Intended Audience :: Computational Neuroscientists'
                            , 'License :: GPLv3'
                            , 'Operating System :: Linux :: Ubuntu'
                            , 'Programming Language :: Python'
                            , 'Programming Language :: C++'
                            ]
     , platforms        =   ["Ubuntu"]
     , license          =   'GPLv3'
     , ext_modules      =   extensions
     , cmdclass         =   { 'build_ext': sipdistutils.build_ext
                            }

     )


