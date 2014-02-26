## This is entry of cmake 

import os
import sys

cdef extern from "main_cython.hpp":
    unsigned int getNumCores()

def PyGetNumCores():
    return getNumCores()

def main():
    print(PyGetNumCores())
