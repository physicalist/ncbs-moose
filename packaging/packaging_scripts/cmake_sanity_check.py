
"""cmake_sanity_check.py: Check if Cmake files are ok.

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

import sys
import os
import re
from collections import defaultdict

makefiles = {}
cmakefiles = {}
makedirs = set()
cmakedirs = set()

def check(d):
    searchMakefiles(d)
    checkMissingCMake()
    checkSrcs()

def checkMissingCMake():
    if (makedirs - cmakedirs):
        print("[Failed] Test 1")
        print("Following directories have Makefile but not a CMakeFiles.txt file.")
        print("%s" % "\t\n".join(makedirs - cmakedirs))

def searchMakefiles(d):
    for d, subd, fs in os.walk(d):
        if "CMakeLists.txt" in fs:
            cmakedirs.add(d)
            cmakefiles[d] = fs
        if "Makefile" in fs:
            makedirs.add(d)
            makefiles[d] = fs
        else: pass

def checkSrcs():
    objPat = re.compile(r"\w+\.o")
    srcPat = re.compile(r"\w+\.cpp")
    srcs = []
    csrcs = []
    for d in makefiles:
        with open(os.path.join(d, "Makefile"), "r") as f:
            txt = f.read()
            srcs = objPat.findall(txt)
        with open(os.path.join(d, "CMakeLists.txt"), "r") as f:
            txt = f.read()
            csrcs = srcPat.findall(txt)
        print("[TEST 2] In dir: %s" % d)
        for csr in csrcs:
            objName = csr.replace(".cpp", ".o")
            if objName in srcs:
                pass
            else:
                print(" Failed: No file to create object {}".format(objName))
    
def main():
    dir = sys.argv[1]
    check(dir)


if __name__ == '__main__':
    main()
