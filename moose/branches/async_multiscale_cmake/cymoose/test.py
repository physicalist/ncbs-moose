#!/usr/bin/env python
# This is topmose file of this excercise.
import sys
sys.path.append(".")
import cymoose

if __name__ == "__main__":
    c = cymoose.Compartment()
    c.Vm = 1.2
    print(c.Vm)
