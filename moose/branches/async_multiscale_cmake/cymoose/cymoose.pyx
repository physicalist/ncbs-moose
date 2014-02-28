# distutils: language = c++
# distutils: include_dirs = ., cymoose
# distutils: extra_compiler_args = -DCYTHON

## This is entry of cmake 

import os
import sys

include "Compartment.pyx"
include "Neutral.pyx"
include "Shell.pyx"
