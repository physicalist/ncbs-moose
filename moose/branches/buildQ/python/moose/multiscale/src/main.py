# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
from lxml import etree

def ifPathsAreValid(paths) :
  ''' Verify if path exists and are readable. '''
  if paths.nml :
    if os.path.isfile(paths.nml) : pass
    else :
      debug.printDebug("ERROR", "Filepath {0} is not valid".format(paths.nml))
      return False
  if paths.sbml :
    if os.path.isfile(paths.sbml) : pass 
    else :
      debug.printDebug("ERROR", "Filepath {0} is not valid".format(paths.sbml))
      return False
  return True


'''
command line parser
'''

# standard module for building a command line parser.

import argparse

# This section build the command line parser
argParser = argparse.ArgumentParser(description= 'Mutiscale modelling of neurons')
argParser.add_argument('--nml', metavar='nmlpath'
    , help = 'File having neuron described in neuroML'
    )
argParser.add_argument('--sbml', metavar='sbmlpath'
    , help = 'File having neuron described in SBML'
    , required = False
    )
args = argParser.parse_args()

# command line parser ends here.


# There must be at least one model specified by user at command line.
import parser
if args.nml or args.sbml : 
  if ifPathsAreValid(args) :
    logger.info("Started parsing XML models")
    debug.printDebug("INFO", "Started parsing XML models")
    etreeDict = parser.parseModels(args, validate=False)
    print etreeDict
  else :
    debug.printDebug("FATAL", "One or more model file does not exists.")
    sys.exit()
else :
  debug.printDebug("FATAL", "Please provide at least one model. None given.")
  sys.exit()
debug.printDebug("INFO", "Parsing of models is done")




