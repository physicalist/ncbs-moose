#!/usr/bin/env python
# This is topmose file of this excercise.
import sys
sys.path.append(".")
import moose_cython

if __name__ == "__main__":
    c = moose_cython.Compartment()
    c.setVm(1.2)
    print(c.getVm())
