# distutils: language = c++
# distutils: include_dirs = ., cymoose
# distutils: extra_compiler_args = -DCYTHON

## This is entry of cmake 

import os
import sys

cdef extern from "./main_cython.hpp":
    cdef unsigned int getNumCores()


include "Compartment.pyx"
include "Neutral.pyx"

if __name__ == "__main__":
    print("testing")
