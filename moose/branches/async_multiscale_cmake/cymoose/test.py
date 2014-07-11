#!/usr/bin/env python
# This is topmose file of this excercise.
import sys
import os
os.environ['LD_LIBRARY_PATH'] = '.'
sys.path.append(".")

import cymoose

if __name__ == "__main__":
    c = cymoose.PyShell()
    a = c.create("Neutral", "/comp", 1)
    a = c.create("Neutral", "/comp/comp1", 1)
    print c.wildcardFind("/##")
