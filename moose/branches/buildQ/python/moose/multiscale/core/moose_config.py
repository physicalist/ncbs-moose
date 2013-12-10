#!/usr/bin/env python 

"""simulator.py:  This class reads the variables needed for simulation and
prepare moose for simulation.

Last modified: Tue Dec 10, 2013  07:32PM

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
        super(Simulator, self).__init__()
        self.arg = arg
        self.simXml = arg[0]
        self.simXmlPath = arg[1]
        self.simElemString = "element"
        self.cellPath = '/cells'
        self.elecPath = self.simXml.getroot().get('elecPath')
        if self.elecPath is None:
            debug.printDebug("WARN"
                    , "elecPath is not specified. Using default /elec"
                    )
        else:
            pass

 
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

    def mapSimulationElement(self, xmlElem):
        debug.printDebug("STEP", "Mapping simulation element")
        if xmlElem.get('type') == 'soma':
            # Now get all records and setup moose for them.
            records = xmlElem.findall('record')
            if records:
                [ self.setupRecored(x, xmlElem.attrib) for x in records ]
            else: 
                debug.printDebug("WARN"
                        , "No record elements found in config.xml file." 
                        )

        else:
            debug.printDebug("TODO"
                    , "Not implemented {0}".format(simXml)
                    , frame = inspect.currentframe()
                    )

    def setupRecored(self, recordXml, params):
        print("Setting up records")
        tableDict = {}
        populationType = params['population']
        variableType = params['type']
        cellGroup = params['cell_group']
        instanceId = int(params['instance_id'])
        
        targetPath = self.popDict[populationType][1][instanceId].path

        for variable in recordXml:
            varName = variable.get('name')
            target = variable.find("target_in_simulator")
            targetType = target.get('type')
            rootPath = target.get('path')

            path = targetPath + '/' + rootPath

            # If the path is not prefixed by element then take the absolute
            # path.
            if target.get('prefixed_by_element') == "false":
                path = rootPath
            else: 
                pass

            # Path has been set, now attach it to moooooose.
            try:
                if targetType == "Compartment":
                    tableDict[path] = moose.utils.setupTable(targetType+varName
                            , moose.Compartment(path)
                            , varName
                            )
                elif targetType == "CaConc":
                    tableDict[path] = moose.utils.setupTable(targetType+varName
                            , moose.CaConc(path)
                            , varName
                            )
                elif targetType == "HHChannel":
                    tableDict[path] = moose.utils.setupTable(targetType+varName
                            , moose.HHChannel(path)
                            , varName
                            )
                else:
                    debug.printDebug("WARN"
                            , "Unsupported type {0}".format(targetType)
                            , frame = inspect.currentframe()
                            )
            except NameError as e:
                debug.printDebug("WARN"
                        , "Can not find element you are trying to connect"
                        , frame = inspect.currentframe()
                        )
                print("\t|- Which is {0}".format(path))
                print("\t|- Available paths")
                print(moose.le(targetPath))
            except Exception as e:
                debug.printDebug("WARN"
                        , "Failed with exception {0}".format(e)
                        , frame = inspect.currentframe()
                        )

        # Now reinitialize moose






