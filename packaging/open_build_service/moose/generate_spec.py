#!/usr/bin/env python
"""generate_spec.py: This script generates spec file for various distributions.
Last modified: Sat Jan 18, 2014  05:01PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import re
import os
import sys

repository = { "CentOS" : [5, 6, 7]
        , "Fedora" : [18, 19, 20 ]
        , "RHEL" : [ 6, 7 ]
        , "SLE" : [ "11_SP3", 12 ]
        , "ScientificLinux" : [6, 7]
        , "Arch" : ["Core"]
        }

buildRequires = {}
libmooseRequires = {}
moose_pythonRequires = {}
moose_guiRequires = {}

commonBuildRequires = ["gcc-c++", "cmake", "python-devel", "python-setuptools"
        , "libxml2-devel", "libbz2-devel", "doxygen", "openmpi-devel"
        ]
commonlibmooseRequires = ["bzip2", "libxml2" ]
commonmoose_pythonRequires = ["python-matplotlib-tk", "libxml2", "bzip2"
        , "python-networkx"
        ]
commonmoose_guiRequires = [ "PyQt4", "PyOpenGL", "moose-python"
        , "OpenSceneGraph" 
        ]

buildRequires["default"] = commonBuildRequires + [ "numpy" ]
libmooseRequires["default"] = commonlibmooseRequires + ["numpy"]
moose_pythonRequires["default"] = commonmoose_pythonRequires + ["numpy"]
moose_guiRequires["default"] = commonmoose_guiRequires 

#buildRequires["CentOS"] = commonBuildRequires + [ "python-numpy"]
#moose_pythonRequires["CentOS"] = commonmoose_pythonRequires + [ "python-numpy"]

buildRequires["RHEL"] = buildRequires["default"] + [ "atlas" ]
moose_pythonRequires["RHEL"] = moose_pythonRequires["default"] + [ "atlas"]

buildRequires["CentOS"] = buildRequires["RHEL"]
moose_pythonRequires["CentOS"] = moose_pythonRequires["RHEL"]

buildRequires["ScientificLinux" ] = buildRequires["RHEL"] + [ "libibverbs"]
moose_pythonRequires["ScientificLinux" ] = moose_pythonRequires["RHEL"]

class SpecFile():

    def __init__(self, repository, version):
        self.repository = repository
        self.version = version
        self.architecture = "i586"
        self.url = None
        self.specfileName = "moose-{}_{}.spec".format(self.repository, version)
        self.templateText = None
        with open("moose.spec.template", "r") as f:
            self.templateText = f.read()

    def writeSpecFile(self, **kwargs):
        builds = buildRequires.get(self.repository, buildRequires["default"])
        builtText = "\n".join([ "BuildRequires: {}".format(x) for x in builds ])
        self.templateText = self.templateText.replace("<<BuildRequires>>", builtText)

        libmoose = libmooseRequires.get(self.repository,
                libmooseRequires["default"])

        libmooseText = "\n".join(["Require: {}".format(x) for x in libmoose])

        # Adding require text.
        self.templateText = self.templateText.replace("<<libmooseRequires>>"
                , libmooseText
                )

        # moose-python
        moose_python = moose_pythonRequires.get(self.repository
                , moose_pythonRequires["default"]
                )
        moose_pythonText = "\n".join("Require: {}".format(x) for x in moose_python)
        self.templateText = self.templateText.replace("<<moose-pythonRequires>>"
                , moose_pythonText
                )

        # moose-gui
        moose_gui = moose_guiRequires.get(self.repository
                , moose_guiRequires["default"]
                )
        moose_guiText = "\n".join(["Require: {}".format(x) for x in moose_gui])
        self.templateText = self.templateText.replace("<<moose-guiRequires>>"
                , moose_guiText
                )

        print("Writing specfile: {}".format(self.specfileName))
        with open(self.specfileName, "w") as specFile:
            specFile.write(self.templateText)
        
def main():
    for r in repository:
        repo, versions = r, repository[r]
        for version in versions:
            sl = SpecFile(repo, version)
            sl.writeSpecFile()
    
if __name__ == '__main__':
    main()
