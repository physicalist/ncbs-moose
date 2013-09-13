#!/usr/bin/env python2.7

# Filename       : main.py
# Created on     : Fri 06 Sep 2013 08:20:50 PM IST
# Author         : Dilawar Singh
# Email          : dilawars@ncbs.res.in
#
# Description    : Entry point if this application is to be run in stand-alone
#   mode.
#
# Logs           :

import os
import sys
import argparse
import debug.debug as debug
import logging
import parser.parser as parser
import core.moose_builder as moose_builder

def pathsAreOk(paths) :
  ''' Verify if path exists and are valid. '''
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


logger = logging.getLogger('multiscale')

if __name__ == "__main__" :
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

  # There must be at least one model present
  if args.nml or args.sbml : 
    if pathsAreOk(args) :
      logger.info("Started parsing XML models")
      debug.printDebug("INFO", "Started parsing XML models")
      etreeList = parser.parseModels(args)

      # Build the storehouse so that moose can simulate it.
      moose_builder.buildMooseObjects(etreeList)
    else :
      debug.printDebug("FATAL", "One or more model file does not exists.")
      sys.exit()
  else :
    parser.print_help()
    debug.printDebug("FATAL", "Please provide at least one model. None given.")
    sys.exit()