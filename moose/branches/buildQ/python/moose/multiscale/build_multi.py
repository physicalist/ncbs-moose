#!/usr/bin/env python

# Filename       : build_multi.py 
# Created on     : 2013-10-20
# Author         : Dilawar Singh
# Email          : dilawars@ncbs.res.in
#
# Description    :
#
# Logs           :

import core.load_multi as load_multi
import os 
import argparse
import debug.debug as debug
import logging
import parser.parser as parser
import core.moose_builder as moose_builder
import sys

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


if __name__ == '__main__' :
  
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
      debug.printDebug("INFO", "Started parsing XML models")
      etreeList = parser.parseModels(args)
    else :
      debug.printDebug("FATAL", "One or more model file does not exists.")
      sys.exit()
  else :
    debug.printDebug("FATAL", "Please provide at least one model. None given.")
    sys.exit()

  debug.printDebug("DEVELOP", "Modifying Upi scripts")
  elecDt = 50e-6
  chemDt = 1e-4
  plotDt = 5e-4
  if not os.path.exists('./plots') :
    os.makedirs('./plots')
  load_multi.makeNeuroMeshModel()
  plotname = 'plots/nm.plot'
