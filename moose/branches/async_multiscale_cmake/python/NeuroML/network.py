# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with MOOSE.  If not, see <http://www.gnu.org/licenses/>.


"""network.py: 

Last modified: Mon Jun 30, 2014  02:27AM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import moose
import sys
import os
import inspect 

from . import moose_config
from . import print_utils
from . import globals
from .moose_compartment import MooseCompartment
from .moose_synapse import Synapse
from neuroml import loaders


class MooseNetwork(Synapse):

    def __init__(self, filename):
        Synapse.__init__(self)
        self.cellDefinitions = dict()
        self.modelDir = '.'
        self.doc = None
        self.networkDoc = None
        self.networkFile = filename
        self.networkDir = globals.design_dir
    
    def parseNetwork(self):
        self.populations = self.networkDoc.populations
        self.projections = self.networkDoc.projections
        
    def buildMooseNetwork(self, network):
        """Build Moose network for a given neuroml network """
        
        if network.type == 'networkWithTemperature':
            globals.setTemperature(network.temperature)
        for pop in network.populations:
            self.addPopulation(pop)
        for proj in network.projections:
            self.addProjection(proj)
        
    def createCellDefinition(self, cellId):
        """Read a file containing cell definition """
        print_utils.dump("STEP"
                , "Creating cell-definition: %s " % cellId 
                )

        componentFile = '%s.nml' % cellId
        componentFilePath = os.path.join(self.networkDir, componentFile)
        if not os.path.isfile(componentFilePath):
            print_utils.dump("FATAL"
                    , [ "Can't find definition file for %s " % cellId 
                        , "Expecting a file %s " % componentFilePath 
                        ]
                    , frame = inspect.currentframe()
                    )
            raise IOError("Definition file is not found")
        
        # File is found. Load cell definition into dictionary so that it can be
        # used later.
        cellDef = loaders.NeuroMLLoader.load(componentFilePath)
        for cell in cellDef.cells:
            if cell.biophysical_properties:
                print_utils.dump("TODO"
                        , "Unsupported biophysical_properties"
                        )
                raise UserWarning("Incomplete function") 
            else:
                pass
            self.cellDefinitions[cellId] = cell.morphology.segments
    
    def addCompartments(self, cellPath, component, segments):
        """For a given segment, create a moose compartment. 
        """
        print_utils.dump("INFO"
                , "Creating a moose compartment for segments under %s " % cellPath
                )
        for segment in segments:
            MooseCompartment.segmentToCompartment(cellPath, component, segment)

    def addInstanceOfPopulation(self, populationId, instance, component):
        """Add an instance of population to mooose """
        instancePath = moose_config.cell_path(populationId, instance.id)
        # Create a population instance in moose.
        try:
            moose.Neuron(instancePath)
        except Exception as e:
            print_utils.dump("FATAL"
                    , [ "Failed to instantiate a population in moose"
                        , "@path: {}".format(instancePath)
                        ]
                    )
            raise e

        # Now populate the Neuron with compartment
        cellSegments = self.cellDefinitions[component]
        moose.Neutral('{}/{}'.format(instancePath, component))

        # In neuroml, paths are usually
        # ../instanceName/instanceId/componentName/segmentId . We remove the
        # leading ../ from the path.
        self.addCompartments(instancePath, component, cellSegments)

    def addPopulation(self, population):
        """Add population to moose """
        populationId = population.id
        component = population.component
        if component not in self.cellDefinitions:
            print_utils.dump("INFO"
                    , "Loading cell {} into library".format(populationId)
                    )
            self.createCellDefinition(component)

        populationPath = moose_config.cell_path(populationId)
        moose.Neutral(populationPath)
        for instance in population.instances:
            self.addInstanceOfPopulation(populationId, instance, component)

    def addProjection(self, projection):
        """Add projection in moose """
        
        synapseType = projection.synapse
        # Each project comes with a synapse.
        if synapseType not in self.synapses:
            self.loadSynapse(synapseType)

        for connection in projection.connections:
            connectionId = connection.id
            sourcePath = moose_config.moose_path(
                    connection.pre_cell_id
                    , connection.pre_segment_id
                    )
            targetPath = moose_config.moose_path(
                    connection.post_cell_id
                    , connection.post_segment_id 
                    )
            self.create(synapseType, sourcePath, targetPath)

    # Top-most function in this class.
    def build(self, **kwargs):
        """Build  moose network"""
        try:
            self.doc = loaders.NeuroMLLoader.load(self.networkFile)
        except Exception as e:
            print_utils.dump("ERROR"
                    , ["Failed to load NeuroML model into Moose"
                        , "The error was: %s " % e 
                    ]
                )
            sys.exit(0)
        for network in self.doc.networks:
            self.buildMooseNetwork(network)


