# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
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
argParser.add_argument('--sbml', metavar='nmlpath'
    , nargs = '*'
    , help = 'sbml model'
    )
argParser.add_argument('--mechml', metavar='mechml'
    , nargs = '*'
    , help = 'mechml model'
    )
argParser.add_argument('--chml', metavar='channelml'
    , nargs = '*'
    , help = 'Channelml model'
    )
argParser.add_argument('--3dml', metavar='3dml'
    , nargs = '*'
    , help = '3DMCML model'
    )
argParser.add_argument('--meshml', metavar='meshml'
    , nargs = '*'
    , help = 'MeshML model'
    )
argParser.add_argument('--adaptor', metavar='adaptor'
    , required = True
    , nargs = '+'
    , help = 'AdaptorML for moose'
    )
args = argParser.parse_args()

import parser
if args : 
  if ifPathsAreValid(args) :
    logger.info("Started parsing XML models")
    debug.printDebug("INFO", "Started parsing XML models")
    etreeDict = parser.parseModels(args, validate=False)
    debug.printDebug("INFO", "Parsing of models is done")
    import multiscale
    multiScaleObj = multiscale.Multiscale(etreeDict)
    multiScaleObj.buildMultiscaleModel()
    print("Done!")
  else :
    debug.printDebug("FATAL", "One or more model file does not exists.")
    sys.exit()
else :
  debug.printDebug("FATAL", "Please provide at least one model. None given.")
  sys.exit()


