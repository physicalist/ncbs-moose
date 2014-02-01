#!/usr/bin/env python 

"""simulator.py:  This class reads the variables needed for simulation and
prepare moose for simulation.

Last modified: Thu Jan 23, 2014  10:18PM

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
import logging 
import core.config as config
import numpy as np
import logging

class Simulator:

    def __init__(self, arg):
        self.arg = arg
        self.simXml = arg[0]
        self.simXmlPath = arg[1]
        self.simElemString = "element"
        self.cellPath = config.cellPath
        self.libraryPath = config.libraryPath
        self.rootElem = self.simXml.getroot()
        self.elecPath = self.rootElem.get('elec_path')
        self.globalVar = self.rootElem.find('global')
        self.simDt = float(self.globalVar.get('sim_dt'))
        self.plotDt = float(self.globalVar.get('plot_dt'))
        self.simMethod = self.globalVar.get('sim_method')
        self.simulate = True
        self.totalPlots = 0
        self.currentCompartmentPath = ''

        self.figurePath = self.rootElem.get('save_path', '.')
        if not os.path.isdir(self.figurePath):
            os.makedirs(self.figurePath)

        if self.rootElem.get('simulate') == "false":
            self.simulate = False
        if self.rootElem.get('runtime'):
            self.runtime = float(self.rootElem.get('runtime'))
        else:
            logging.info( "No run-time for simulation is given." +
                    " Using default 10.0"
                    )
            self.runtime = 10.0
        if self.elecPath is not None:
            self.elecPath = '/neuroml/electrical'
            logging.info("Using default library path %s" % self.elecPath)

 
    def updateMoose(self, populationDict, projectionDict):
        """Update the moose, as per the simulation specific settings.
        """
        self.popDict = populationDict
        self.projDict = projectionDict

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
                [ self.setupRecord(x, xmlElem.attrib) for x in records ]
            else: 
                logging.warning("No record elements found in config.xml file.")
                return 
        else:
            logging.warning("Not implemented {0}".format(xmlElem))

    def setupRecord(self, recordXml, params):
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
            logging.info("Target path is %s " % targetBasePath)
        except KeyError as e:
            debug.printDebug("ERROR"
                    , [ "Population type `{0}` not found".format(populationType)
                        , "Availale population in network are "
                        , self.popDict.keys()
                    ]
                    )
            raise LookupError("Missing population type")
        except Exception as e:
            raise e

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
                    self.tableDict[targetPath] = (moose.utils.setupTable(
                            targetPath
                            , moose.Compartment(path)
                            , varName
                            ), path)
                elif targetType == "CaConc":
                    self.tableDict[targetPath] = (moose.utils.setupTable(
                            targetPath
                            , moose.CaConc(path)
                            , varName
                            ), path)
                elif targetType == "HHChannel":
                    self.tableDict[targetPath] = (moose.utils.setupTable(
                            targetPath
                            , moose.HHChannel(path)
                            , varName
                            ), path)
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
            if recordXml.get('single_plot', 'false') == 'true':
                self.plotAllOnOne(variablesToPlot)
            else:
                [self.addPlot(pXml) for pXml in variablesToPlot] 

    def plotAllOnOne(self, variables):
        """
        Plot all plots on one file
        """
        figname = os.path.join(self.figurePath
                , ''.join([y for x, y, z in variables])
                )+'.eps'
        logging.info("Saving all plots to %s" % figname)

        self.totalPlots += len(variables)

        for i, var in enumerate(variables):
            varType, varName, plotXml = var
            varName = varType + varName
            assert varName in self.tableDict.keys()
            configDict = self.configurePlot(plotXml)
            yvec, xvec = self.getPlotData(varName)
            pylab.subplot(self.totalPlots, 1, i)
            self.plotVar(xvec, yvec, configDict)
        if figname:
            pylab.savefig(figname)
        else:
            pylab.show()
    
    def plotVar(self, xvec, yvec, plotParams):

        plotArgs = plotParams.get('plot_args', '')
        pylab.plot(xvec, yvec, plotArgs)
        pylab.xlabel(plotParams.get('xlabel', ''), fontsize=8)
        pylab.ylabel(plotParams.get('ylabel', ''), fontsize=8)
        pylab.title(plotParams.get('title', ''), fontsize=8)
        pylab.tick_params(labelsize='6')
        
    def configurePlot(self, plotXml):
        '''
        Configure plot according to plotXml
        '''
        plotType = plotXml.get('type', '2d')
        if plotType == "2d":
            return self.configure2DPlot(plotXml)
        else:
            logging.warning("Plot type %s is not supported yet." % plotType)
            return dict()
        

    def configure2DPlot(self, plotXml):
        """
        Configure 2D plots
        """

        plotParams = dict()
        xXml = plotXml.find('x')
        yXml = plotXml.find('y')
        try:
            self.start = float(xXml.attrib['start'])
        except Exception as e:
            self.start = 0.0
        assert self.start >= 0.0, "Must start at >= 0.0 time. Right?"

        try:
            self.step = float(xXml.attrib['step'])
        except Exception as e:
            try:
                self.step = self.simDt 
            except Exception as e:
                self.step = 10e-6
                logging.warning(
                        "No step size specified. Using {0}".format(self.step)
                        )
        if self.step <= 0.0:
            logging.critical("Non-positive step size.  {0}".format(self.step))
            raise RuntimeError, "Non-positive step size"
            
        try:
            self.stop = float(xXml.attrib['stop'])
        except Exception as e:
            self.stop = self.runtime
            logging.debug(
                    "No stop time is specified. Using default %s" % self.stop
                    )
        assert self.stop > 0.0

        # Label and title
        labelXml = plotXml.find('label')
        plotParams['output_file'] = plotXml.get('output', None)
        if labelXml is not None:
            plotParams['xlabel'] = labelXml.get('x')
            plotParams['ylabel'] = labelXml.get('y')
            plotParams['title'] = "{}:{}".format(
                    self.currentCompartmentPath
                    , ':' 
                    , labelXml.get('title', '')
                    )

        return plotParams

    def getPlotData(self, varName):
        """Get the plot data from dictionary
        """
        assert varName in self.tableDict.keys()
        tableObj, self.currentCompartmentPath = self.tableDict[varName]
        
        # If there is nothing to plot, quit
        if len(tableObj.vec) == 0:
            logging.warning("Empty vec. Nothing to plot.")
            return None, None

        xvec = numpy.arange(self.start, self.stop*2, self.step)

        # Resize xvec to fit the simulation size.
        logging.debug(
                "Resizing vector of {} to fit in simulation size {}".format(
                    varName
                    , tableObj.vec.size
                    )
                )
        xvec = xvec[ : tableObj.vec.size ]
        return tableObj.vec, xvec

    def addPlot(self, varsToPlot):
        """
        Draws given variables.
        """

        varType, varName, plotXml = varsToPlot
        varName = varType + varName 

        plotParams = self.configurePlot(plotXml)
        yvec, xvec = self.getPlotData(varName)

        if xvec is None or yvec is None:
            return
        if plotXml.get('plot', 'true') == "false":
            return 

        self.plotVar(xvec, yvec, plotParams)
        outputFile = plotParams.get('output_file', None)
        if outputFile:
            debug.printDebug("USER"
                    , "Saving figure to {0}".format(outputFile)
                    )
            pylab.savefig(outputFile.strip())
        else:
            pylab.show()

