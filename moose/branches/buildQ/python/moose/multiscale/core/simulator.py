#!/usr/bin/env python 

"""simulator.py:  This class reads the variables needed for simulation and
prepare moose for simulation.

Last modified: Mon Dec 30, 2013  02:43AM

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
import pylab
import os

class Simulator:

    def __init__(self, arg):
        self.arg = arg
        self.simXml = arg[0]
        self.simXmlPath = arg[1]
        self.simElemString = "element"
        self.cellPath = '/cells'
        self.libraryPath = '/neuroml/library'
        self.rootElem = self.simXml.getroot()
        self.elecPath = self.rootElem.get('elec_path')
        self.globalVar = self.rootElem.find('global')
        self.simDt = float(self.globalVar.get('sim_dt'))
        self.plotDt = float(self.globalVar.get('plot_dt'))
        self.simMethod = self.globalVar.get('sim_method')
        self.simulate = True

        if self.rootElem.get('simulate') == "false":
            self.simulate = False
        if self.rootElem.get('runtime'):
            self.runtime = float(self.rootElem.get('runtime'))
        else:
            debug.printDebug("INFO"
                    , "No run-time for simulation is given. Using default 10.0"
                    )
            self.runtime = 10.0
        if self.elecPath is not None:
            self.elecPath = '/neuroml/electrical'
            debug.printDebug("INFO"
                    , "Using default library path %s" % self.elecPath 
                    )

 
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

        # Setup clocks and then run the simulation.


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
        debug.printDebug("STEP"
                , "Setting up records during simulation. "
                )

        # this dictionary keeps the object created by function setupTable in a
        # variable name. This object of setupTable can be accessed later 
        self.tableDict = {}

        populationType = params['population']
        variableType = params['type']

        # NOTE: This is useless.
        cellGroup = params.get('cell_group')

        instanceId = int(params['instance_id'])
        
        try:
            targetBasePath = self.popDict[populationType][1][instanceId].path
            debug.printDebug("INFO", "Target path is %s " % targetBasePath)
        except KeyError as e:
            debug.printDebug("ERR", "Key {0} not found".format(populationType))
            print("\t|- Available population in this model are following. ")
            print("\t+ {0} -| ".format(self.popDict.keys()))
            raise UserWarning, "Unknown population type."

        variablesToPlot = list()

        for variable in recordXml:
            varName = variable.get('name')
            target = variable.find("target_in_simulator")
            targetType = target.get('type')
            rootPath = target.get('path')

            path = os.path.join(targetBasePath, rootPath)

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
                debug.printDebug("DEBUG"
                        , "Target path : {0} <= {1}".format(targetPath
                            , path)
                        )

                if targetType == "Compartment":
                    self.tableDict[targetPath] = moose.utils.setupTable(
                            targetPath
                            , moose.Compartment(path)
                            , varName
                            )
                elif targetType == "CaConc":
                    self.tableDict[targetPath] = moose.utils.setupTable(
                            targetPath
                            , moose.CaConc(path)
                            , varName
                            )
                elif targetType == "HHChannel":
                    self.tableDict[targetPath] = moose.utils.setupTable(
                            targetPath
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
                variablesToPlot.append(
                        (variableType, varName, variable.find('plot'))
                        )
        # Now reinitialize moose
        assert self.simDt > 0.0
        assert self.plotDt > 0.0 

        debug.printDebug("STEP", "Simulation : %s %s".format(self.simDt
                , self.plotDt)
                )
        moose.utils.resetSim([self.elecPath, self.cellPath]
                , self.simDt
                , self.plotDt
                , simmethod = self.simMethod 
                )

        if self.simulate:
            debug.printDebug("USER"
                    , "Simulating for {0} seconds.".format(self.runtime)
                    )

            moose.start(self.runtime)

            # After simulation, plot user-requested variables.
            [ self.addPlot(pXml) for pXml in variablesToPlot ] 

    def addPlot(self, plotVar):
        """
        Draws given variables.
        """

        varType, variableName, plotXml = plotVar
        plotVarName = varType + variableName 

        # We must get a table object. If object is not available the program
        # must terminate here.
        assert plotVarName in self.tableDict.keys()
        tableObj = self.tableDict[plotVarName]

        debug.printDebug("INFO"
                , "Adding plotXml for {0}".format(plotVarName)
                )
        if plotXml.get('type') is None or plot.get('type') == "2d":
            xXml = plotXml.find('x')
            yXml = plotXml.find('y')
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
                    step = 10e-6
                    debug.printDebug("WARN"
                            , "No step size specified. Using {0}".format(step)
                            )
            if step <= 0.0:
                debug.printDebug("ERR", "Non-positive step size.  {0}".format(
                    step))
                raise RuntimeError, "Non-positive step size"
                
            try:
                stop = float(xXml.attrib['stop'])
            except Exception as e:
                stop = self.runtime
                debug.printDebug("WARN"
                        , "No stop time is specified. Using default %s" % stop
                        )
            assert stop > 0.0

            xvec = numpy.arange(start, stop*2, step)

            # Resize xvec to fit the simulation size.
            xvec = xvec[ : tableObj.vec.size ]

            if plotXml.get('plot') == "false":
                return 

            if plotXml.get('output'):
                filename = plotXml.get('output')

            if plotXml.get('plot_args') is None:
                plotArgs = ''
            else:
                plotArgs = plotXml.get('plot_args')

            pylab.figure()
            debug.printDebug("DEBUG", "Plot args: {0}".format(plotArgs))
            pylab.plot(xvec, tableObj.vec, plotArgs)
            
            label = plotXml.find('label')
            if label is not None:
                pylab.xlabel(label.get('x'))
                pylab.ylabel(label.get('y'))
                pylab.title(label.get('title'))

            outputFile = plotXml.get('output')
            if outputFile is not None and len(outputFile.strip()) > 0:
                debug.printDebug("USER"
                        , "Saving figure to {0}".format(outputFile)
                        )
                pylab.savefig(outputFile.strip())
            else:
                pylab.show()
            

        elif plotXml.get('type') in ["3d", "3D"]:
            debug.printDebug("TODO", "3d plotXmlting is not implemented yet.")
            raise UserWarning, "Feature not implemented"

