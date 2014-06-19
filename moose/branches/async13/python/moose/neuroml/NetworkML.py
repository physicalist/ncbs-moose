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
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


"""

NeuroML.py is the preferred interface. Use this only if NeuroML L1,L2,L3 files
are misnamed/scattered.  

Instantiate NetworlML class, and thence use method:

    readNetworkMLFromFile(...) to load a standalone NetworkML file, OR
    readNetworkML(...) to load from an xml.etree xml element (could be part of a
    larger NeuroML file).

Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE

Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, ported to parallel
MOOSE

Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, further changes for
parallel MOOSE

Version 1.7 by Aditya Gilra, NCBS, Bangalore, India, 2013, further support for
NeuroML 1.8.1

"""

from xml.etree import cElementTree as ET
import string
import os
from math import cos, sin
from MorphML import MorphML
from ChannelML import ChannelML

import moose
import moose.utils as utils 

import utils as nml_utils
from .. import moose_config as config

class NetworkML():

    def __init__(self, nml_params):
        """Initialize 
        """
        self.cellDictBySegmentId={}
        self.cellDictByCableId={}
        self.nml_params = nml_params
        self.model_dir = nml_params['model_dir']
        self.Efactor = 1.0
        self.Tfactor = 1.0
        self.Ifactor = 1.0

    def cell_path(self, cellName, cellId):
        """Get path of a cell from cell dictionary """
        return self.populationDict[cellName][1][cellId].path

    def segment_path(self, cellName, segmentId):
        """Get path of a segment of a cell """
        return self.cellSegmentDict[cellName][segmentId][0]


    def readNetworkMLFromFile(self, filename, cellSegmentDict, params={}):

        """ 
        specify tweak params = {'excludePopulations':[popname1,...]
            , 'excludeProjections':[projname1,...]
            , 'onlyInclude':{'includePopulation':(popname,[id1,...])
            ,'includeProjections':(projname1,...)} 
            }


        Notes by Aditya:

        If excludePopulations is present, then excludeProjections must also be
        present:

        Thus if you exclude some populations, ensure that you exclude
        projections that refer to those populations also!  Though for
        `onlyInclude`, you may specify only included cells and this reader will
        also keep cells connected to those in `onlyInclude`.  
        
        This reader first prunes the exclude-s, then keeps the onlyInclude-s and
        those that are connected.  Use 'includeProjections' if you want to keep
        some projections not connected to the primary 'includePopulation' cells
        but connected to secondary cells that connected to the primary ones:
        e.g.  baseline synapses on granule cells connected to
        'includePopulation' mitrals; these synapses receive file based
        pre-synaptic events, not presynaptically connected to a cell.  
        
        """

        utils.dump("NEUROML", "Reading file %s " % filename)
        tree = ET.parse(filename)
        root_element = tree.getroot()

        utils.dump("INFO", "Tweaking model")
        nml_utils.tweak_model(root_element, params)

        utils.dump("NEUROML", "Loading NeuroML model into MOOSE.")
        network = self.readNetworkML(root_element
                , cellSegmentDict
                , params
                )
        return network

    def readNetworkML(self, network, cellSegmentDict, params={}):
        """
        This returns populationDict = {
            'populationname1':(cellName,{(instanceid1):moosecell, ... }) 
            , ... 
            }

        and projectionDict = { 
            'projName1':(source,target,[(syn_name1,pre_seg_path,post_seg_path)
            ,...]) 
            , ... 
            }
        """

        lengthUnits = network.attrib.get('lengthUnits', 'micron')
        if lengthUnits in ['micrometer','micron']:
            self.length_factor = 1e-6
        else:
            self.length_factor = 1.0

        self.network = network
        self.cellSegmentDict = cellSegmentDict
        self.params = params

        utils.dump("NEUROML", "Creating populations ... ")
        self.populationDict = {}
        populations = self.network.findall(".//{"+nml_utils.nml_ns+"}population")
        [self.insertPopulation(p) for p in populations]

        utils.dump("NEUROML", "Creating connections ... ")
        self.projectionDict={}
        projections = self.network.find(".//{"+nml_utils.nml_ns+"}projections")
        if projections is not None:
            # see pg 219 (sec 13.2) of Book of Genesis
            if projections.attrib["units"] == 'Physiological Units': 
                self.Efactor = 1e-3 # V from mV
                self.Tfactor = 1e-3 # s from ms
        [self.addProjection(proj) for proj in projections] 

        utils.dump("NEUROML", "Creating inputs in %s ..." % nml_utils.elecPath)
        inputs = self.network.findall(".//{"+nml_utils.nml_ns+"}inputs")
        [self.addInputs(input) for input in inputs]

        return (self.populationDict, self.projectionDict)

    def sanitizieUnits(self, unit):
        """If unit are given as Physiological units then we need to use a factor
        of 1e3.
        """
        if unit == 'Physiological Units': 
            self.Vfactor = 1e-3 # V from mV
            self.Tfactor = 1e-3 # s from ms
            self.Ifactor = 1e-6 # A from microA
        else:
            self.Vfactor = 1.0 
            self.Tfactor = 1.0 
            self.Ifactor = 1.0 

    def addInputs(self, inputs):
        """Add a given input """
        units = inputs.attrib['units']
        self.sanitizieUnits(units)
        for inputElem in inputs.findall(".//{"+nml_utils.nml_ns+"}input"):
            pulseInput = inputElem.find(".//{"+nml_utils.nml_ns+"}pulse_input")
            if pulseInput is not None:
                self.addPulseGen(pulseInput, inputElem, diffAmp=True)


    def addPulseGen(self, pulseInput, inputElem, diffAmp=False):
        """ Add a PulseGen to a compartment. If diffAmp is True then insert the
        DiffAmp between pulseGen and compartment.

        """
        moose.Neutral(nml_utils.elecPath)
        inputName = inputElem.attrib['name']
        pulsegen = moose.PulseGen(nml_utils.elecPath+'/pulsegen_'+inputName)
        pulsegen.trigMode = 0 # free run
        pulsegen.baseLevel = 0.0
        pulsegen.firstDelay = float(pulseInput.attrib['delay']) * self.Tfactor
        pulsegen.firstWidth = float(pulseInput.attrib['duration']) * self.Tfactor
        pulsegen.firstLevel = float(pulseInput.attrib['amplitude']) * self.Ifactor
        pulsegen.secondDelay = 1e6 # to avoid repeat
        pulsegen.secondLevel = 0.0
        pulsegen.secondWidth = 0.0

        # If diffAmp is True then attach a differential amplifier to pulseGen
        if diffAmp:
            iclamp = moose.DiffAmp(nml_utils.elecPath+'/iclamp_'+inputName)
            iclamp.saturation = 1e6
            iclamp.gain = 1.0
            moose.connect(pulsegen, 'output', iclamp, 'plusIn')

        target = inputElem.find(".//{"+nml_utils.nml_ns+"}target")
        population = target.attrib['population']

        for site in target.findall(".//{"+nml_utils.nml_ns+"}site"):
            cell_id = site.attrib['cell_id']
            if site.attrib.has_key('segment_id'): 
                segment_id = site.attrib['segment_id']
            else: 
                # default segment_id is specified to be 0
                segment_id = '0'

            cell_name = self.populationDict[population][0]
            cellPath = self.populationDict[population][1][(cell_id)].path
            segmentName = self.cellSegmentDict[cell_name][segment_id][0]
            segment_path = cellPath+'/'+segmentName
            comp = moose.Compartment(segment_path)
            if diffAmp:
                utils.dump("DEBUG"
                        , "Connecting %s and %s" % (iclamp.path, comp.path)
                        )
                moose.connect(iclamp,'output', comp,'injectMsg')
            else:
                utils.dump("DEBUG"
                        , "Connecting %s and %s" % (pulsegen.path, comp.path)
                        )
                moose.connect(pulsegen, 'output', comp, 'injectMsg')


    def readCellFromXMLFile(self, cellName):
        """Read cell morphology from an XML file. """
        utils.dump("DEBUG"
                , "Reading morphology of cell %s from a file" % cellName
                )
        mmlR = MorphML(self.nml_params)
        model_filenames = (cellName+'.xml', cellName+'.morph.xml')
        for model_filename in model_filenames:
            model_path = nml_utils.find_first_file(model_filename,self.model_dir)
            if model_path is not None:
                cellDict = mmlR.readMorphMLFromFile(model_path)
                self.cellSegmentDict.update(cellDict)
                return True

        msg = "I can not find cell morphology. I search for files "
        msg += "{}".format(",".join(model_filenames))
        msg += " in directory {}.".format(self.model_dir)
        utils.dump("FATAL", msg)
        raise IOError("Unable to find morphology file")

    def insertPopulation(self, population):
        """Insert a population entry into moose """

        cellName = population.attrib["cell_type"]
        popName = population.attrib["name"]
        utils.dump("DEBUG"
                , "Inserting cell %s of population %s" % (cellName, popName)
                )

        ## if cell does not exist in library load it from xml file
        cellInLibrary = nml_utils.libraryPath+'/'+cellName
        if not moose.exists(cellInLibrary):
            self.readCellFromXMLFile(cellName)

        #added cells as a Neuron class.
        libcell = moose.Neuron(nml_utils.libraryPath+'/'+cellName) 
        self.populationDict[popName] = (cellName, {})
        moose.Neutral(config.cellPath)
        for instance in population.findall(".//{"+nml_utils.nml_ns+"}instance"):
            instanceid = instance.attrib['id']
            location = instance.find('./{'+nml_utils.nml_ns+'}location')
            rotationnote = instance.find('./{'+nml_utils.meta_ns+'}notes')
            if rotationnote is not None:
                ## the text in rotationnote is zrotation=xxxxxxx
                zrotation = float(string.split(rotationnote.text,'=')[1])
            else:
                zrotation = 0

            # deep copies the library cell to an instance under '/cells' named
            # as <arg3> /cells is useful for scheduling clocks as all sim
            # elements are in /cells
            utils.dump("INFO"
                    , "Copying {} from library to path {} recursively".format(
                        libcell.path
                        , '%s/%s' % (config.cellPath, popName+'_'+instanceid)
                        )
                    )
            cellid = moose.copy(libcell
                    , moose.Neutral(config.cellPath)
                    , popName+"_"+instanceid
                    )
            cell = moose.Neuron(cellid)
            self.populationDict[popName][1][instanceid] = cell
            x = float(location.attrib['x']) * self.length_factor
            y = float(location.attrib['y']) * self.length_factor
            z = float(location.attrib['z']) * self.length_factor
            nml_utils.translate_rotate(cell, x, y, z, zrotation)
            

    def addProjection(self, projection):
        """Add a given projection """

        projName = projection.attrib["name"]
        utils.dump("INFO", "Adding a projection {}".format(projName))
        source = projection.attrib["source"]
        target = projection.attrib["target"]
        self.projectionDict[projName] = (source,target,[])
        synapseProps = projection.findall(".//{"+nml_utils.nml_ns+"}synapse_props")
        [self.addSynapseProps(projection, synProp, source, target) 
                for synProp in synapseProps
                ]

    def addSynapseProps(self, projection, synProps, source, target):
        """Add a synpase properties """
        projName = projection.attrib["name"]
        syn_name = synProps.attrib['synapse_type']
        ## if synapse does not exist in library load it from xml file
        if not moose.exists("/library/"+syn_name):
            cmlR = ChannelML(self.nml_params)
            model_filename = syn_name+'.xml'
            model_path = nml_utils.find_first_file(model_filename,self.model_dir)
            if model_path is not None:
                cmlR.readChannelMLFromFile(model_path)
            else:
                raise IOError(
                    'For mechanism {0}: files {1} not found under {2}.'.format(
                        "", model_filename, self.model_dir
                    )
                )
        weight = float(synProps.attrib['weight'])
        threshold = float(synProps.attrib['threshold']) * self.Efactor

        if 'prop_delay' in synProps.attrib:
            prop_delay = float(synProps.attrib['prop_delay'])*self.Tfactor
        elif 'internal_delay' in synProps.attrib:
            prop_delay = float(synProps.attrib['internal_delay'])*self.Tfactor
        else: 
            prop_delay = 0.0
        for connection in projection.findall(".//{"+nml_utils.nml_ns+"}connection"):
            pre_cell_id = connection.attrib['pre_cell_id']
            post_cell_id = connection.attrib['post_cell_id']
            if 'file' not in pre_cell_id:
                # source could be 'mitrals', self.populationDict[source][0]
                # would be 'mitral'
                pre_cell_name = self.populationDict[source][0]
                if 'pre_segment_id' in connection.attrib:
                    pre_segment_id = connection.attrib['pre_segment_id']
                else:
                    # assume default segment 0, usually soma
                    pre_segment_id = "0" 
                pre_segment_path = '{}/{}'.format( 
                        self.cell_path(source, pre_cell_id)
                        , self.segment_path(pre_cell_name, pre_segment_id)
                        )
            else:
                # I've removed extra excitation provided via files, so below
                # comment doesn't apply.  'file[+<glomnum>]_<filenumber>' #
                # glomnum is for mitral_granule extra excitation from unmodelled
                # sisters.
                pre_segment_path = "{}_{}".format(
                        pre_cell_id
                        , connection.attrib['pre_segment_id']
                        )

            # target could be 'PGs', self.populationDict[target][0] would be 'PG'
            post_cell_name = self.populationDict[target][0]
            if 'post_segment_id' in connection.attrib:
                post_segment_id = connection.attrib['post_segment_id']
            else: 
                # assume default segment 0, usually soma
                post_segment_id = '0' 

            post_segment_path = "{}/{}".format(
                    self.cell_path(target, post_cell_id)
                    , self.segment_path(post_cell_name, post_segment_id)
                    )
            self.projectionDict[projName][2].append(
                    (syn_name, pre_segment_path, post_segment_path)
                    )
            properties = connection.findall('./{'+nml_utils.nml_ns+'}properties')
            if len(properties)==0:
                self.connectSyn(syn_name
                        , pre_segment_path
                        , post_segment_path
                        , weight
                        , threshold
                        , prop_delay
                        )
            else:
                for props in properties:
                    synapse_type = props.attrib['synapse_type']
                    if syn_name in synapse_type:
                        weight_override = float(props.attrib['weight'])
                        if 'internal_delay' in props.attrib:
                            delay_override = float(props.attrib['internal_delay'])
                        else: delay_override = prop_delay
                        if weight_override != 0.0:
                            self.connectSyn(syn_name
                                    , pre_segment_path
                                    , post_segment_path
                                    , weight_override
                                    , threshold
                                    , delay_override
                                    )


    def connectSyn(self, syn_name, pre_path, post_path, weight, threshold, delay):
        """Connect a synapse """
        postcomp = moose.Compartment(post_path)
        # We usually try to reuse an existing SynChan - event based SynChans
        # have an array of weights and delays and can represent multiple
        # synapses i.e.  a new element of the weights and delays array is
        # created every time a 'synapse' message connects to the SynChan (from
        # 'event' of spikegen). BUT for a graded synapse with a lookup table
        # output connected to 'activation' message, not to 'synapse' message, we
        # make a new synapse everytime ALSO for a saturating synapse i.e.
        # KinSynChan, we always make a new synapse as KinSynChan is not meant to
        # represent multiple synapses
        libsyn = moose.SynChan(nml_utils.libraryPath+'/'+syn_name)
        gradedchild = utils.get_child_Mstring(libsyn,'graded')
        # create a new synapse
        if libsyn.className == 'KinSynChan' or gradedchild.value == 'True': 
            syn_name_full = syn_name+'_'+utils.underscorize(pre_path)
            self.make_new_synapse(syn_name, postcomp, syn_name_full)
        else:
            ## if syn doesn't exist in this compartment, create it
            syn_name_full = syn_name
            if not moose.exists(post_path+'/'+syn_name_full):
                self.make_new_synapse(syn_name, postcomp, syn_name_full)

        # moose.element is a function that checks if path exists, and returns
        # the correct object, here SynChan
        # wrap the synapse in this compartment
        syn = moose.element(post_path+'/'+syn_name_full) 

        gradedchild = utils.get_child_Mstring(syn,'graded')

        # weights are set at the end according to whether the synapse is graded
        # or event-based

        # connect pre-comp Vm (if graded) OR spikegen/timetable (if event-based)
        # to the synapse I rely on second term below not being evaluated if
        # first term is None; else None.value gives error.
        if gradedchild is not None and gradedchild.value == 'True':
            table = moose.Table(syn.path+"/graded_table")

            # always connect source to input - else 'cannot create message'
            # error.
            precomp = moose.Compartment(pre_path)
            moose.connect(precomp,"VmOut",table,"msgInput")

            # since there is no weight field for a graded synapse (no 'synapse'
            # message connected), I set the Gbar to weight*Gbar
            syn.Gbar = weight*syn.Gbar
        else: 
            # Event based synapse: synapse could be connected to spikegen at
            # pre-compartment OR a file!
            if 'file' not in pre_path:
                # element() can return either Compartment() or IzhikevichNrn(),
                # since it queries and wraps the actual object
                precomp = moose.element(pre_path)

                # if spikegen for this synapse doesn't exist in this
                # compartment, create it spikegens for different synapse_types
                # can have different thresholds but an integrate and fire
                # spikegen supercedes all other spikegens
                if not moose.exists(pre_path+'/IaF_spikegen'):
                    if not moose.exists(pre_path+'/'+syn_name+'_spikegen'):
                        # create new spikegen
                        spikegen = moose.SpikeGen(pre_path+'/'+syn_name+'_spikegen')
                        # connect the compartment Vm to the spikegen
                        moose.connect(precomp,"VmOut",spikegen,"Vm")
                        # spikegens for different synapse_types can have
                        # different thresholds
                        spikegen.threshold = threshold

                        # This ensures that spike is generated only on leading edge.
                        spikegen.edgeTriggered = 1 

                    # wrap the existing or newly created spikegen in this
                    # compartment
                    spikegen = moose.SpikeGen(pre_path+'/'+syn_name+'_spikegen')
                else:
                    spikegen = moose.SpikeGen(pre_path+'/IaF_spikegen')

                # connect the spikegen to the synapse note that you need to use
                # Synapse (auto-created) under SynChan to get/set weights ,
                # addSpike-s etc.  can get the Synapse element by
                # moose.Synapse(syn.path+'/synapse') or syn.synapse Synpase is
                # an array element, first add to it, to addSpike-s, get/set
                # weights, etc.
                syn.numSynapses += 1

                # above works, but below gives me an error sayin getNum_synapse
                # not found but both work in Demos/snippets/lifcomp.py see
                # Demos/snippets/synapse.py for an example of how to connect
                # multiple SpikeGens to the same SynChan
                m = moose.connect(spikegen, 'spikeOut',
                                    syn.synapse[-1], 'addSpike', 'Single')
            else:
                # if connected to a file, create a timetable, put in a field
                # specifying the connected filenumbers to this segment, and
                # leave it for simulation-time connection
                filesplit = pre_path.split('+')
                if len(filesplit) == 2:
                    glomsplit = filesplit[1].split('_',1)
                    glomstr = '_'+glomsplit[0]
                    filenums = glomsplit[1]
                else:
                    glomstr = ''
                    filenums = pre_path.split('_',1)[1]
                tt_path = postcomp.path+'/'+syn_name_full+glomstr+'_tt'
                if not moose.exists(tt_path):
                    # if timetable for this synapse doesn't exist in this
                    # compartment, create it, and add the field 'fileNumbers'
                    tt = moose.TimeTable(tt_path)
                    tt_filenums = moose.Mstring(tt_path+'/fileNumbers')
                    tt_filenums.value = filenums

                    # Be careful to connect the timetable only once while
                    # creating it as below: note that you need to use Synapse
                    # (auto-created) under SynChan to get/set weights ,
                    # addSpike-s etc.  can get the Synapse element by
                    # moose.Synapse(syn.path+'/synapse') or syn.synapse Synpase
                    # is an array element, first add to it, to addSpike-s,
                    # get/set weights, etc.
                    syn.numSynapses += 1

                    # above works, but below gives me an error sayin
                    # getNum_synapse not found but both work in
                    # Demos/snippets/lifcomp.py
                    m = moose.connect(tt,"eventOut",syn.synapse[-1],"addSpike","Single")
                else:
                    # if it exists, append file number to the field
                    # 'fileNumbers' append filenumbers from
                    # 'file[+<glomnum>]_<filenumber1>[_<filenumber2>...]'
                    tt_filenums = moose.Mstring(tt_path+'/fileNumbers')
                    tt_filenums.value += '_' + filenums

            # syn.Gbar remains the same, but we play with the weight which is a
            # factor to Gbar The delay and weight can be set only after
            # connecting a spike event generator.  delay and weight are arrays:
            # multiple event messages can be connected to a single synapse first
            # argument below is the array index, we connect to the latest
            # synapse created above But KinSynChan ignores weight of the
            # synapse, so set the Gbar for it
            if libsyn.className == 'KinSynChan':
                syn.Gbar = weight*syn.Gbar
            else:
                # note that you need to use Synapse (auto-created) under SynChan
                # to get/set weights , addSpike-s etc.  can get the Synpase
                # element by moose.Synapse(syn.path+'/synapse') or syn.synapse
                syn.synapse[-1].weight = weight
            syn.synapse[-1].delay = delay # seconds

    def make_new_synapse(self, syn_name, postcomp, syn_name_full):
        # if channel does not exist in library load it from xml file
        if not moose.exists(nml_utils.libraryPath+'/'+syn_name):
            cmlR = ChannelML(self.nml_params)
            cmlR.readChannelMLFromFile(syn_name+'.xml')

        # deep copies the library synapse to an instance under postcomp named as
        # <arg3>
        synid = moose.copy(
                moose.Neutral(nml_utils.libraryPath+'/'+syn_name)
                , postcomp
                , syn_name_full
                )
        syn = moose.SynChan(synid)
        childmgblock = utils.get_child_Mstring(syn,'mgblockStr')

        # connect the post compartment to the synapse
        # If NMDA synapse based on mgblock, connect to mgblock
        if childmgblock.value == 'True': 
            mgblock = moose.Mg_block(syn.path+'/mgblock')
            moose.connect(postcomp,"channel", mgblock, "channel")
        # if SynChan or even NMDAChan, connect normally
        else: 
            moose.connect(postcomp, "channel", syn, "channel")
