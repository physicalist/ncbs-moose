#!/usr/bin/env python 

"""simulator.py:  This class reads the variables needed for simulation and
prepare moose for simulation.

Last modified: Thu Dec 05, 2013  04:44PM

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@iitb.ac.in"
__status__           = "Development"

class Simulator(object):

    def __init__(self, arg):
        print arg
        super(Simulator, self).__init__()
        self.arg = arg
        self.simXml = arg[0]
        self.simXmlPath = arg[1]
        
