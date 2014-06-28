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

Last modified: Sat Jan 18, 2014  05:01PM

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
from neuroml import loaders

class MooseNetwork():

    def __init__(self, filename):
        self.cellDefinitions = dict()
        self.modelDir = '.'
        self.doc = None
        self.networkDoc = None
        self.networkFile = filename
        self.networkDir = os.path.dirname(filename)
    
    def parseNetwork(self):
        self.populations = self.networkDoc.populations
        self.projections = self.networkDoc.projections
        
    def buildMooseNetwork(self, network):
        """Build Moose network for a given neuroml network """
        for pop in network.populations:
            self.addPopulation(pop)
        for proj in network.projections:
            self.addProjection(proj)
        
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
            sys.exit(0)
        
        # File is found. Load cell definition into dictionary so that it can be
        # used later.
        cellDef = loaders.NeuroMLLoader.load(componentFilePath)
        for cell in cellDef.cells:
            if cell.biophysical_properties:
                print_utils.dump("TODO"
                        , "Unsupported biophysical_properties"
                        )
                return 
            self.cellDefinitions[cellId] = cell.morphology.segments
    
    def addCompartments(self, cellPath, component, segments):
        """Add segments to cell """
        print_utils.dump("INFO"
                , "Adding segments under %s " % cellPath
                )
        for segment in segments:
            if not segment.id:
                segment.id = '0'
            compartmentPath = '{}/{}/{}'.format(cellPath, component, segment.id)
            comp = moose.Compartment(compartmentPath)
            if segment.parent:
                parentSegment = segment.parent.segments
                parentPath = '{}/{}/{}'.format(cellPath, component, parentSegment)
                parentComp = moose.Compartment(parentPath)
                parentComp.connect('axial', comp, 'raxial')


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
        # ../instanceName/instanceId/componentName/segmentId 
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


    def synapse(self, sourcePath, targetPath):
        """Add a synapse from sourcePath to targetPath """
        print_utils.dump("SYNAPSE"
                , "Connecting {} and {}".format(sourcePath, targetPath)
                )        
        if not moose.exists(sourcePath):
            print_utils.dump("FATAL"
                    , [ "Can't create synapse"
                        , "Source compartment {} does not exists".format(
                            sourcePath
                            )
                        ]
                    )
            sys.exit(0)
        if not moose.exists(targetPath):
            print_utils.dump("FATAL"
                    , [ "Can't create synapse"
                        , "Target compartment {} does not exists".format(
                            targetPath
                            )
                        ]
                    )
            sys.exit(0)

    def addProjection(self, projection):
        """Add projection in moose """
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
            #self.synapse(sourcePath, targetPath)

