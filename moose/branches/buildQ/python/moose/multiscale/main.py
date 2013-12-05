# This is our main script.
import os
import sys
import debug.debug as debug
import inspect
import parser.NeuroML as nml_loader
import core.mumbl as mumbl
import core.moose_config as moose_config

from lxml import etree

def ifPathsAreValid(paths) :
  ''' Verify if path exists and are readable. '''
  if paths :
    paths = vars(paths)
    for p in paths :
      if not paths[p] : continue
      for path in paths[p] :
        if not path : continue
        if os.path.isfile(path) : pass
        else :
          debug.printDebug("ERROR"
            , "Filepath {0} does not exists".format(path))
          return False
      # check if file is readable
      if not os.access(path, os.R_OK) :
        debug.printDebug("ERROR", "File {0} is not readable".format(path))
  return True

# standard module for building a command line parser.
import argparse

# This section build the command line parser
argParser = argparse.ArgumentParser(description= 'Mutiscale modelling of neurons')
argParser.add_argument('--nml', metavar='nmlpath'
    , required = True
    , nargs = '+'
    , help = 'nueroml model'
    )
argParser.add_argument('--mumbl', metavar='mumbl', required = True, nargs = '+'
    , help = 'Lanaguge to do multi-scale modelling in moose'
    )
argParser.add_argument('--config', metavar='config', required = True
    , nargs = '+'
    , help = 'Simulation specific settings for moose in a xml file'
    )
args = argParser.parse_args()

import parser.parser as parser

if args:
    if ifPathsAreValid(args):
        debug.printDebug("INFO", "Started parsing NML models")
        etreeDict = parser.parseModels(args, validate=False)
        debug.printDebug("INFO", "Parsing of models is done")
        nmlObj = nml_loader.NeuroML()

        simObj = moose_config.Simulator(etreeDict['config'][0])

        nml = etreeDict['nml'][0]
        populationDict, projectionDict = nmlObj.loadNML(nml)

        debug.printDebug("STEP", "Updating moose for simulation")
        simObj.updateMoose(populationDict, projectionDict)

        # Start processing mumbl
        mumblObj = mumbl.Mumble(etreeDict['mumbl'])
        mumblObj.load()

        sys.exit()
    else:
        debug.printDebug("FATAL", "One or more model file does not exists.")
        sys.exit()
else:
    debug.printDebug("FATAL", "Please provide at least one model. None given.")
    sys.exit()


