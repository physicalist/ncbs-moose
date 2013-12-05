# This file process mumbl.
#!/usr/bin/env python 

"""mumbl.py: This file reads the mumbl file and load it onto moose. 
This class is entry point of multiscale modelling.

Last modified: Thu Dec 05, 2013  03:33PM

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@iitb.ac.in"
__status__           = "Development"

import debug.debug as debug


class Mumble(object):
    """ Mumble: Class for loading mumble onto moose.
    """
    def __init__(self, mumbl):
        self.mumblElem = mumbl[0][0]
        self.path = mumbl[0][1]

    def load(self):
        """ Lead mumble element tree
        """
        debug.printDebug("INFO", "Loading mumble")


        
     
