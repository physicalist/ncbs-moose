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

"""moose_synapse.py: 
    
    Synapse in Moose.

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
import os 
import inspect 

from neuroml import loaders
from collections import defaultdict

from . import print_utils
from . import globals


class Synapse():

    def __init__(self):
        self.basePath = '/synapse'
        moose.Neutral(self.basePath)
        self.synapses = defaultdict(list)

    def loadSynapse(self, synName):
        """Load definition file and create synapse """
        filename = os.path.join(globals.design_dir, '{}.nml'.format(synName))
        channel = loaders.NeuroMLLoader.load(filename)
        self.createSyanspeDefinition(synName, channel)

    def addExpOneSynapse(self, synPath, expOneSynapse):
        """Add Exp one synapse """
        syn = moose.SynChan(synPath)
        syn.tau1 = globals.toSIValue(expOneSynapse.tau_decay)
        syn.tau2 = syn.tau1
        syn.Ek = globals.toSIValue(expOneSynapse.erev)
        syn.Gbar = globals.toSIValue(expOneSynapse.gbase)
        syn.synapse.num = 1
        return syn

    def addExpTwoSynapse(self, synPath, expTwoSynapse):
        """Add a two exponential synapse """
        syn = moose.SynChan(synPath)
        syn.tau1 = globals.toSIValue(expTwoSynapse.tau_rise)
        syn.tau2 = globals.toSIValue(expTwoSynapse.tau_decay)
        syn.Ek = globals.toSIValue(expTwoSynapse.erev)
        syn.Gbar = globals.toSIValue(expTwoSynapse.gbase) 
        syn.synapse.num = 1
        return syn

    def createSyanspeDefinition(self, synName, channel):
        """Create Synapse definition from given channel file """
        for i, syn in enumerate(channel.exp_one_synapses):
            synBasePath = '{}/{}'.format(self.basePath, synName)
            moose.Neutral(synBasePath)
            synPath = '{}/{}'.format(synBasePath, i)
            synChan = self.addExpOneSynapse(synPath, syn)
            self.synapses[synName].append(synChan)
        for i, syn in enumerate(channel.exp_two_synapses):
            synBasePath = '{}/{}'.format(self.basePath, synName)
            moose.Neutral(synBasePath)
            synPath = '{}/{}'.format(synBasePath, i)
            synChan = self.addExpTwoSynapse(synPath, syn)
            self.synapses[synName].append(synChan)
        for i, syn in enumerate(channel.exp_cond_synapses):
            print_utils.dump("TODO", "Unsupported exp_cond_synapse")

    def create(self, name, sourcePath, targetPath, **kwargs):
        """Create a synapse in Moose of synType """

        assert name in self.synapses, "Definition of synapse is missing"

        # Get the SynChan.
        synChan = self.synapses[name][-1]
        # Check source and target exists in Moose.
        assert moose.exists(sourcePath)
        assert moose.exists(targetPath)
        # Cool, we have source and target exist in Moose. Now create a synapse.
        sourceComp = moose.Compartment(sourcePath)
        targetComp = moose.Compartment(targetPath)

        ##print_utils.dump("INFO"
        ##        , "Currently only event-based synapse are supported"
        ##        , frame = inspect.currentframe()
        ##        )

        # Souce compartment is fed to a SpikeGen which produces spikes. These
        # spikes are fed to SynChan and SynChan finally makes connection onto
        # target compartment.
        spikeGen = moose.SpikeGen('{}/{}'.format(sourcePath, 'spikegen'))
        spikeGen.threshold = 0.0
        spikeGen.edgeTriggered = True
        sourceComp.connect('VmOut', spikeGen, 'Vm')
        spikeGen.connect('spikeOut', synChan.synapse[-1], 'addSpike', 'Single')

        # Synapse on targetCompartment
        targetComp.connect('channel', synChan, 'channel')
