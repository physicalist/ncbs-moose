# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
from lxml import etree

def ifPathsAreValid(paths) :
  ''' Verify if path exists and are readable. '''
  if paths.xml :
    if os.path.isfile(paths.xml) : pass
    else :
      debug.printDebug("ERROR", "Filepath {0} is not valid".format(paths.nml))
      return False
  return True


'''
command line parser
'''

# standard module for building a command line parser.

import argparse

# This section build the command line parser
argParser = argparse.ArgumentParser(description= 'Mutiscale modelling of neurons')
argParser.add_argument('--xml', metavar='nmlpath'
    , required = True
    , help = 'multiscale model in XML'
    )
argParser.add_argument('--adaptor', metavar='nmlpath'
    , required = True
    , help = 'AdaptorML for moose'
    )
args = argParser.parse_args()

# command line parser ends here.


# There must be at least one model specified by user at command line.
import parser
if args.xml : 
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


