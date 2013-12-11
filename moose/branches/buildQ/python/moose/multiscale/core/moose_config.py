#!/usr/bin/env python 

"""simulator.py:  This class reads the variables needed for simulation and
prepare moose for simulation.

Last modified: Wed Dec 11, 2013  10:56PM

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
import numpy
import core.dynamic_vars as dynvar

class Simulator(object):

    def __init__(self, arg):
        super(Simulator, self).__init__()
        self.arg = arg
        self.simXml = arg[0]
        self.simXmlPath = arg[1]
        self.simElemString = "element"
        self.cellPath = '/cells'
        self.elecPath = self.simXml.getroot().get('elec_path')
        self.globalVar = self.simXml.getroot().find('global')
        self.simDt = float(self.globalVar.get('sim_dt'))
        self.plotDt = float(self.globalVar.get('plot_dt'))
        self.simMethod = self.globalVar.get('sim_method')

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
        """
        Params is a dictionary having attributes to <element>
        """
        print("Setting up records")
        tableDict = {}
        populationType = params['population']
        variableType = params['type']
        cellGroup = params['cell_group']
        instanceId = int(params['instance_id'])
        
        targetBasePath = self.popDict[populationType][1][instanceId].path

        for variable in recordXml:
            varName = variable.get('name')
            target = variable.find("target_in_simulator")
            targetType = target.get('type')
            rootPath = target.get('path')

            path = targetBasePath + '/' + rootPath

            # If the path is not prefixed by element then take the absolute
            # path.
            if target.get('prefixed_by_element') == "false":
                path = rootPath
            else: 
                pass

            # Path has been set, now attach it to moooooose.
            try:
                path = path.strip()
                targetPath = variableType + varName

                if targetType == "Compartment":
                    tableDict[path] = moose.utils.setupTable(targetPath
                            , moose.Compartment(path)
                            , varName
                            )
                elif targetType == "CaConc":
                    tableDict[path] = moose.utils.setupTable(targetPath
                            , moose.CaConc(path)
                            , varName
                            )
                elif targetType == "HHChannel":
                    tableDict[path] = moose.utils.setupTable(targetPath
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
                print("\t+ while available paths are ")
                print(moose.le(targetPath))
                print("\t list of available paths end. -| ")
                continue
            except Exception as e:
                debug.printDebug("WARN"
                        , "Failed with exception {0}".format(e)
                        , frame = inspect.currentframe()
                        )
                continue

            # If plot element is available then plot it.
            if variable.find('plot') is not None:
                self.addPlot(variable.find('plot'), varName)
        # Now reinitialize moose

        assert self.simDt > 0.0
        assert self.plotDt > 0.0 

        moose.utils.resetSim([self.elecPath, self.cellPath]
                , self.simDt
                , self.plotDt
                , simmethod = self.simMethod 
                )

    def addPlot(self, plot, variableName):
        """
        Add plot to moose
        """
        print("Adding plot for {0}".format(variableName))
        if plot.get('type') is None or plot.get('type') == "2d":
            xXml = plot.find('x')
            yXml = plot.find('y')
            try:
                start = float(xXml.attrib['start'])
            except Exception as e:
                start = 0.0
            assert start >= 0.0

            try:
                step = float(xXml.attrib['step'])
            except Exception as e:
                try:
                    step = self.simDt 
                except Exception as e:
                    debug.printDebug("WARN"
                            , "No step size specified. Using 1e-6"
                            )
                    step = 1e-6
            if step <= 0.0:
                debug.printDebug("ERR", "Non-positive step size.  {0}".format(
                    step))
                raise RuntimeError, "Non-positive step size"
                
            try:
                stop = float(xXml.attrib['stop'])
            except Exception as e:
                debug.printDebug("WARN"
                        , "No stop time is specified. Using default {0}".format(
                            1)
                        )
                stop = 1.0
            assert stop > 0.0

            xvec = numpy.arange(start, stop, step)

            # Start, step and stop are implemented. Now setup plotting. In short
            # x-axis has been initialized.

        elif plot.get('type').lower() == "3d":
            debug.printDebug("TODO", "3d plotting is not implemented yet.")
            raise UserWarning, "Feature not implemented"

