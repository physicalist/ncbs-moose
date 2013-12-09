#!/usr/bin/env python 

"""simulator.py:  This class reads the variables needed for simulation and
prepare moose for simulation.

Last modified: Sat Dec 07, 2013  09:22AM

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@iitb.ac.in"
__status__           = "Development"

import moose 
import debug.debug as debug
import inspect

class Simulator(object):

    def __init__(self, arg):
        print arg
        super(Simulator, self).__init__()
        self.arg = arg
        self.simXml = arg[0]
        self.simXmlPath = arg[1]
        self.simElemString = "element"
 
    def updateMoose(self, populationDict, projectionDict):
        """Update the moose, as per the simulation specific settings.
        """
        self.popDict = populationDict
        self.projDict = projectionDict

        # Fist setup simulation time  and plot variable etc.
        # TODO: Add them here.

        # Add simulation element e
        [self.mapSimulationElement(x) for x in 
                self.simXml.findall(self.simElemString) ]

#        print populationDict['CellGroupA']
#        print populationDict.keys(), projectionDict.keys()
#        b = populationDict['CellGroupA'][1][0]
#        print moose.le(b.path)


    def mapSimulationElement(self, simXml):
        debug.printDebug("STEP", "Mapping simulation element")
        if simXml.get('type') == 'soma':
            pop = simXml.get('population')
            cellT = simXml.get('cell_group')
            cellId = simXml.get('cell_id')
            print pop, cellT, cellId 
        else:
            debug.printDebug("TODO"
                    , "Not implemented {0}".format(simXml)
                    , frame = inspect.currentframe()
                    )
