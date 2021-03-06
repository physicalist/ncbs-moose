"""

 Description: class NetworkML for loading NetworkML from file or xml element
 into MOOSE

 Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE

 Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, ported to parallel
 MOOSE

 Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, further changes for
 parallel MOOSE

 Version 1.7 by Aditya Gilra, NCBS, Bangalore, India, 2013, further support for
 NeuroML 1.8.1

    NeuroML.py is the preferred interface. Use this only if NeuroML L1,L2,L3
    files are misnamed/scattered.  Instantiate NetworlML class, and thence use
    method: readNetworkMLFromFile(...) to load a standalone NetworkML file, OR
    readNetworkML(...) to load from an xml.etree xml element (could be part of a
    larger NeuroML file).

"""

from xml.etree import cElementTree as ET
import string
import os
import sys
import MorphML
import ChannelML
import moose
import moose.neuroml.utils as nmu
import moose.utils as mu
import debug.debug as debug
import inspect
import helper.xml_methods as xml_methods
import helper.moose_methods as moose_methods
import core.stimulus as stimulus
import core.config as config
from debug.logger import *
import debug.bugs as bugs
import debug.debug as debug
import matplotlib.pyplot as plt
import numpy as np

from math import cos, sin
import utils

class NetworkML(object):

    def __init__(self, nml_params):
        self.populationDict = dict()
        self.libraryPath = config.libraryPath
        moose.Neutral(self.libraryPath)
        self.cellPath = self.libraryPath
        moose.Neutral(self.cellPath)
        moose.Neutral(self.libraryPath)
        self.cellDictBySegmentId={}
        self.cellDictByCableId={}
        self.nml_params = nml_params
        self.modelDir = nml_params['model_dir']
        self.elecPath = config.elecPath
        self.dt = 1e-3 # In seconds.
        self.simTime = 1000e-3
        self.plotPaths = 'figs'

    def connectWrapper(self
            , src
            , src_field
            , dest
            , dest_field
            , message_type='Single'):
        """
        Wrapper around moose.connect 
        """
        debug.printDebug("DEBUG"
                    , "Connecting ({4})` {0}/{1}` & `{2}/{3}".format(src.path
                        , src_field
                        , dest.path, dest_field
                        , message_type
                        )
                    , frame = inspect.currentframe()
                )
        try:
            res = moose.connect(src, src_field, dest, dest_field, message_type)
            assert res, "Failed to connect"
        except Exception as e:
            debug.printDebug("ERROR", "Failed to connect.")
            raise e

    def plotVector(self, vector, plotname):
        ''' Saving input vector to a file '''
        name = plotname.replace('/', '_')
        fileName = os.path.join(self.plotPaths, name)+'.eps'
        debug.printDebug("DEBUG"
                , "Saving vector to a file {}".format(fileName)
                )
        plt.vlines(vector, 0, 1)
        plt.savefig(fileName)


    def readNetworkMLFromFile(self, filename, cellSegmentDict, params={}):
    

        """ readNetworkML

        specify tweak
        params = {
            'excludePopulations':[popname1,...]
            , 'excludeProjections':[projname1,...]
            , 'onlyInclude':{'includePopulation':(popname,[id1,...])
            ,'includeProjections':(projname1,...)
            }
        }

        If excludePopulations is present, then excludeProjections must also be
        present. Thus if you exclude some populations, ensure that you exclude
        projections that refer to those populations also!  Though for
        onlyInclude, you may specify only included cells and this reader will
        also keep cells connected to those in onlyInclude.  This reader first
        prunes the exclude-s, then keeps the onlyInclude-s and those that are
        connected.  Use 'includeProjections' if you want to keep some
        projections not connected to the primary 'includePopulation' cells but
        connected to secondary cells that connected to the primary ones: e.g.
        baseline synapses on granule cells connected to 'includePopulation'
        mitrals; these synapses receive file based pre-synaptic events, not
        presynaptically connected to a cell.

        """

        debug.printDebug("INFO", "reading file %s ... " % filename)
        tree = ET.parse(filename)
        root_element = tree.getroot()
        debug.printDebug("INFO", "Tweaking model ... ")
        utils.tweak_model(root_element, params)
        debug.printDebug("INFO", "Loading model into MOOSE ... ")
        return self.readNetworkML(
                root_element
               , cellSegmentDict
               , params
               , root_element.attrib['lengthUnits']
               )

    def readNetworkML(self, network, cellSegmentDict , params={}
        , lengthUnits="micrometer"):

        """
        This returns
         populationDict = {
           'populationName1':(cellname
           , {int(instanceid1):moosecell, ...F}) , ...
           }

         projectionDict = {
            'projectionName1':(source,target
            ,[(syn_name1,pre_seg_path,post_seg_path),...])
            , ...
            }
        """

        if lengthUnits in ['micrometer','micron']:
            self.length_factor = 1e-6
        else:
            self.length_factor = 1.0
        self.network = network
        self.cellSegmentDict = cellSegmentDict
        self.params = params
        debug.printDebug("STEP", "Creating populations ... ")
        self.createPopulations() 

        debug.printDebug("STEP", "Creating connections ... ")
        self.createProjections() 

        # create connections
        debug.printDebug("STEP", "Creating inputs in %s .. " % self.elecPath)

        # create inputs (only current pulse supported)
        self.createInputs() 
        return (self.populationDict, self.projectionDict)

    def createInputs(self):

        """ createInputs

        Create inputs as given in NML file and attach them to moose.
        """

        for inputs in self.network.findall(".//{"+nmu.nml_ns+"}inputs"):
            units = inputs.attrib['units']

            # This dict is to be passed to function which attach an input
            # element to moose.
            factors = {}

            # see pg 219 (sec 13.2) of Book of Genesis
            if units == 'Physiological Units':
                factors['Vfactor'] = 1e-3 # V from mV
                factors['Tfactor'] = 1e-3 # s from ms
                factors['Ifactor'] = 1e-6 # A from microA
            else:
                debug.printDebug("NOTE", "We got {0}".format(units))
                factors['Vfactor'] = 1.0
                factors['Tfactor'] = 1.0
                factors['Ifactor'] = 1.0

            for inputElem in inputs.findall(".//{"+nmu.nml_ns+"}input"):
                self.attachInputToMoose(inputElem, factors)

    def attachInputToMoose(self, inElemXml, factors, savePlot=True):

        """attachInputToMoose
        This function create StimulousTable in moose

        There can be two type of stimulous: random_stim or pulse_input.
        """

        # If /elec doesn't exists it creates /elec and returns a reference to
        # it. If it does, it just returns its reference.
        moose.Neutral(self.elecPath)
        inputName = inElemXml.get('name')

        random_stim = inElemXml.find('.//{'+nmu.nml_ns+'}random_stim')
        pulse_stim = inElemXml.find('.//{'+nmu.nml_ns+'}pulse_input')

        if random_stim is not None:
            debug.printDebug("INFO", "Generating random stimulous")
            debug.printDebug("TODO", "Test this Poission spike train table")
    
            # Get the frequency of stimulus
            frequency = moose_methods.toFloat(random_stim.get('frequency'
                    , '1.0')) / factors['Tfactor']
            amplitude = random_stim.get('amplitude', 1.0)
            synpMechanism = random_stim.get('synaptic_mechanism')

            # Create random stimulus
            vec = stimulus.generateSpikeTrainPoission(frequency
                    , dt=self.dt
                    , simTime=self.simTime
                    )
            # Stimulus table
            tablePath = os.path.join(self.elecPath, "Stimulus")
            moose.Neutral(tablePath)
            stimPath = os.path.join(tablePath, inputName)
            stim = moose.TimeTable(stimPath)
            stim.vec = vec

            if savePlot:
                self.plotVector(vec, tablePath)

            target = inElemXml.find(".//{"+nmu.nml_ns+"}target")
            population = target.get('population')
            for site in target.findall(".//{"+nmu.nml_ns+"}site"):
               cell_id = site.attrib['cell_id']
               if 'segment_id' in site.attrib:
                   segment_id = site.attrib['segment_id']
               else:
                   # default segment_id is specified to be 0
                   segment_id = 0 

               # To find the cell name fetch the first element of tuple.
               cell_name = self.populationDict[population][0]
               if cell_name == 'LIF':
                   debug.printDebug("NOTE",
                           "LIF cell_name. Partial implementation"
                           , frame = inspect.currentframe()
                           )

                   LIF = self.populationDict[population][1][int(cell_id)]
                   self.connectWrapper(stim, "event", LIF, 'addSpike')
               else:
                    segId = '{0}'.format(segment_id)
                    segment_path = self.populationDict[population][1]\
                           [int(cell_id)].path + '/' + \
                            self.cellSegmentDict[cell_name][segId][0]
                    compartment = moose.Compartment(segment_path)
                    synchan = moose.SynChan(
                        os.path.join(compartment.path
                                     , '/synchan'
                                     )
                    )
                    synchan.Gbar = 1e-6
                    synchan.Ek = 0.0
                    debug.printDebug("INFO"
                            , "Connecting {0} -> {0}".format(synchan.path
                                , compartment.path)
                            )
                    self.connectWrapper(synchan, 'channel', compartment, 'channel')
                    synchan.synapse.num = 1
                    self.connectWrapper(
                        stim
                        , "event"
                        , moose.element(synchan.path+'/synapse')
                        , "addSpike"
                    )
        elif pulse_stim is not None:
            Ifactor = factors['Ifactor']
            Tfactor = factors['Tfactor']
            pulseinput = inElemXml.find(".//{"+nmu.nml_ns+"}pulse_input")
            if pulseinput is not None:
                self.pulseGenPath = os.path.join(self.elecPath, 'PulseGen')
                moose.Neutral(self.pulseGenPath)
                pulseGenPath = os.path.join(self.pulseGenPath, inputName)
                pulsegen = moose.PulseGen(pulseGenPath)
                icClampPath = os.path.join(self.elecPath, 'iClamp')
                moose.Neutral(icClampPath)
                iclamp = moose.DiffAmp(os.path.join(icClampPath, inputName))
                iclamp.saturation = 1e6
                iclamp.gain = 1.0

                # free run
                pulsegen.trigMode = 0 
                pulsegen.baseLevel = 0.0
                pulsegen.firstDelay = float(pulseinput.attrib['delay'])*Tfactor
                pulsegen.firstWidth = float(pulseinput.attrib['duration'])*Tfactor
                pulsegen.firstLevel = float(pulseinput.attrib['amplitude'])*Ifactor

                # to avoid repeat
                pulsegen.secondDelay = 1e6 
                pulsegen.secondLevel = 0.0
                pulsegen.secondWidth = 0.0

                # do not set count to 1, let it be at 2 by default else it will
                # set secondDelay to 0.0 and repeat the first pulse!
                #pulsegen.count = 1
                self.connectWrapper(pulsegen,'outputOut', iclamp, 'plusIn')

                # Attach targets
                target = inElemXml.find(".//{"+nmu.nml_ns+"}target")
                population = target.attrib['population']
                for site in target.findall(".//{"+nmu.nml_ns+"}site"):
                    cell_id = site.attrib['cell_id']
                    if 'segment_id' in site.attrib: 
                        segment_id = site.attrib['segment_id']
                    else: 
                        segment_id = 0 # default segment_id is specified to be 0

                    # population is populationName,
                    # self.populationDict[population][0] is cellname
                    cell_name = self.populationDict[population][0]
                    if cell_name == 'LIF':
                        LIF = self.populationDict[population][1][int(cell_id)]
                        self.connectWrapper(iclamp,'outputOut',LIF,'injectDest')
                    else:
                        segment_path = self.populationDict[population][1]\
                                [int(cell_id)].path+'/'+\
                                 self.cellSegmentDict[cell_name][segment_id][0]
                        compartment = moose.Compartment(segment_path)
                        debug.printDebug("DEBUG"
                                , "Adding a pulse generator {} -> {}".format(
                                    iclamp.path
                                    , segment_path
                                    )
                                )
                        self.connectWrapper(iclamp
                                ,'outputOut'
                                , compartment
                                ,'injectMsg'
                                )
            else:
                debug.printDebug("WARN"
                        , "This type of stimulous is not supported."
                        , frame = inspect.currentframe()
                        )
                return 

    def createPopulations(self):
        """
        Create population dictionary.
        """
        populations =  self.network.findall(".//{"+nmu.nml_ns+"}population")
        if not populations:
            debug.printDebug("WARN"
                    , [ 
                        "No population find in model"
                        , "Searching in namespace {}".format(nmu.nml_ns)
                        ]
                    , frame = inspect.currentframe()
                    )

        for population in populations:
            cellname = population.attrib["cell_type"]
            populationName = population.attrib["name"]
            debug.printDebug("INFO"
                    , "Loading population `{0}`".format(populationName)
                    )
            # if cell does not exist in library load it from xml file
            if not moose.exists(self.libraryPath+'/'+cellname):
                debug.printDebug("DEBUG"
                        , "Searching in subdirectories for cell types" + 
                        " in `{0}.xml` and `{0}.morph.xml` ".format(cellname)
                        )
                mmlR = MorphML.MorphML(self.nml_params)
                model_filenames = (cellname+'.xml', cellname+'.morph.xml')
                success = False
                for modelFile in model_filenames:
                    model_path = nmu.find_first_file(modelFile
                            , self.modelDir
                            )
                    if model_path is not None:
                        cellDict = mmlR.readMorphMLFromFile(model_path)
                        success = True
                        break
                if not success:
                    raise IOError(
                        'For cell {0}: files {1} not found under {2}.'.format(
                            cellname, model_filenames, self.modelDir
                        )
                    )
                self.cellSegmentDict.update(cellDict)
            if cellname == 'LIF':
                cellid = moose.LeakyIaF(self.libraryPath+'/'+cellname)
            else:
                # added cells as a Neuron class.
                cellid = moose.Neuron(self.libraryPath+'/'+cellname) 

            self.populationDict[populationName] = (cellname,{})

            for instance in population.findall(".//{"+nmu.nml_ns+"}instance"):
                instanceid = instance.attrib['id']
                location = instance.find('./{'+nmu.nml_ns+'}location')
                rotationnote = instance.find('./{'+nmu.meta_ns+'}notes')
                if rotationnote is not None:
                    # the text in rotationnote is zrotation=xxxxxxx
                    zrotation = float(string.split(rotationnote.text,'=')[1])
                else:
                    zrotation = 0
                if cellname == 'LIF':
                    cell = moose.LeakyIaF(cellid)
                    self.populationDict[populationName][1][int(instanceid)] = cell
                else:
                    # No Cell class in MOOSE anymore! :( addded Neuron class -
                    # Chaitanya
                    cell = moose.Neuron(cellid) 
                    self.populationDict[populationName][1][int(instanceid)] = cell
                    x = float(location.attrib['x']) * self.length_factor
                    y = float(location.attrib['y']) * self.length_factor
                    z = float(location.attrib['z']) * self.length_factor
                    self.translate_rotate(cell, x, y, z, zrotation)

    # recursively translate all compartments under obj
    def translate_rotate(self,obj,x,y,z,ztheta): 
        for childId in obj.children:
            childobj = moose.Neutral(childId)
            # if childobj is a compartment or symcompartment translate, else
            # skip it
            if childobj.className in ['Compartment','SymCompartment']:
                # SymCompartment inherits from Compartment, so below wrapping by
                # Compartment() is fine for both Compartment and SymCompartment
                child = moose.Compartment(childId)
                x0 = child.x0
                y0 = child.y0
                x0new = x0 * cos(ztheta) - y0 * sin(ztheta)
                y0new = x0 * sin(ztheta) + y0 * cos(ztheta)
                child.x0 = x0new + x
                child.y0 = y0new + y
                child.z0 += z
                x1 = child.x
                y1 = child.y
                x1new = x1 * cos(ztheta) - y1 * sin(ztheta)
                y1new = x1 * sin(ztheta) + y1 * cos(ztheta)
                child.x = x1new + x
                child.y = y1new + y
                child.z += z
            if len(childobj.children)>0:
                # recursive translation+rotation
                self.translate_rotate(childobj
                        , x
                        , y
                        , z
                        , ztheta
                        )

    def addConnection(self, connection, projection, options):

        """
        This function adds connection
        """
        synName = options['syn_name']
        source = options['source']
        target = options['target']
        weight = options['weight']
        threshold = options['threshold']
        propDelay = options['prop_delay']

        self.synapseType = list()
        projectionName = projection.attrib['name']
        for syn in projection.findall('./{'+nmu.nml_ns+'}synapse_props'):
            self.synapseType.append(syn.get('synapse_type'))

        pre_cell_id = connection.attrib['pre_cell_id']
        post_cell_id = connection.attrib['post_cell_id']

        if 'file' not in pre_cell_id:
            # source could be 'mitrals', self.populationDict[source][0] would be
            # 'mitral'
            pre_cell_name = self.populationDict[source][0]
            if 'pre_segment_id' in connection.attrib:
                pre_segment_id = connection.attrib['pre_segment_id']
            # assume default segment 0, usually soma
            else: 
                pre_segment_id = "0"
            pre_segment_path = self.populationDict[source][1]\
                    [int(pre_cell_id)].path+'/'+ \
                    self.cellSegmentDict[pre_cell_name][pre_segment_id][0]
        else:
            # I've removed extra excitation provided via files, so below comment
            # doesn't apply.  'file[+<glomnum>]_<filenumber>' # glomnum is for
            # mitral_granule extra excitation from unmodelled sisters.
            pre_segment_path = pre_cell_id + '_' \
                    + connection.attrib['pre_segment_id']

        # target could be 'PGs', self.populationDict[target][0] would be 'PG'
        post_cell_name = self.populationDict[target][0]
        if 'post_segment_id' in connection.attrib:
            post_segment_id = connection.attrib['post_segment_id']
        # assume default segment 0, usually soma
        else: post_segment_id = "0" 

        post_segment_path = self.populationDict[target][1]\
                [int(post_cell_id)].path+'/'+\
                self.cellSegmentDict[post_cell_name][post_segment_id][0]

        try:
            self.projectionDict[projectionName][2].append(
                    (synName , pre_segment_path, post_segment_path)
                    )
        except KeyError as e:
            debug.printDebug("ERR", "Failed find key {0}".format(e)
                    , frame = inspect.currentframe())
            print self.projectionDict.keys()
            sys.exit(0)

        properties = connection.findall('./{'+nmu.nml_ns+'}properties')
        if len(properties) == 0:
            self.connect(synName, pre_segment_path, post_segment_path
                    , weight, threshold, propDelay)
        else:
            for props in properties:
                synapse = props.attrib.get('synapse_type')
                if not synapse: 
                    debug.printDebug("WARN"
                            , "Synapse type {} not found.".format(synapse)
                            , frame = inspect.currentframe()
                            )
                else:
                    synName = synapse
                    if synName not in self.synapseType:
                        debug.printDebug("WARN"
                                , [ "Unknown synapse type : {}".format(synName)
                                    , "Doing nothing, returning from function ...."
                                ]
                                , frame = inspect.currentframe()
                                )
                        return 
                    weight_override = float(props.attrib['weight'])
                    if 'internal_delay' in props.attrib:
                        delay_override = float(props.attrib['internal_delay'])
                    else: delay_override = propDelay
                    if weight_override != 0.0:
                        self.connect(synName
                                , pre_segment_path
                                , post_segment_path
                                , weight_override
                                , threshold, delay_override
                                )
                    else: pass


    def createProjections(self):
        self.projectionDict={}
        projections = self.network.find(".//{"+nmu.nml_ns+"}projections")
        if projections is not None:
            if projections.attrib["units"] == 'Physiological Units':
                # see pg 219 (sec 13.2) of Book of Genesis
                self.Efactor = 1e-3 # V from mV
                self.Tfactor = 1e-3 # s from ms
            else:
                self.Efactor = 1.0
                self.Tfactor = 1.0
        [self.createProjection(p) for p in
                self.network.findall(".//{"+nmu.nml_ns+"}projection")]


    def createProjection(self, projection):
        ''' Create a single projection '''
        projectionName = projection.attrib["name"]
        debug.printDebug("INFO", "Projection {0}".format(projectionName))
        source = projection.attrib["source"]
        target = projection.attrib["target"]
        self.projectionDict[projectionName] = (source,target,[])

        synProps = projection.findall(".//{"+nmu.nml_ns+"}synapse_props")
        [self.addSyapseProperties(projection, p, source, target) for 
                p in synProps]


    def addSyapseProperties(self, projection,  syn_props, source, target):
        '''Add Synapse properties'''
        synName = syn_props.attrib['synapse_type']
        ## if synapse does not exist in library load it from xml file
        if not moose.exists(os.path.join(self.libraryPath, synName)):
            cmlR = ChannelML.ChannelML(self.nml_params)
            modelFileName = synName+'.xml'
            model_path = nmu.find_first_file(modelFileName
                    , self.modelDir
                    )
            if model_path is not None:
                cmlR.readChannelMLFromFile(model_path)
            else:
                msg = 'For mechanism {0}: files {1} not found under {2}.'.format(
                        synName, modelFileName, self.modelDir
                    )
                raise UserWarning(msg)

        weight = float(syn_props.attrib['weight'])
        threshold = float(syn_props.attrib['threshold']) * self.Efactor
        if 'prop_delay' in syn_props.attrib:
            propDelay = float(syn_props.attrib['prop_delay']) * self.Tfactor
        elif 'internal_delay' in syn_props.attrib:
            propDelay = float(syn_props.attrib['internal_delay']) * self.Tfactor
        else:
            propDelay = 0.0

        options = { 'syn_name' : synName , 'weight' : weight
                   , 'source' : source , 'target' : target
                   , 'threshold' : threshold , 'prop_delay' : propDelay 
                   }
        connections = projection.findall(".//{"+nmu.nml_ns+"}connection")
        [self.addConnection(c, projection, options) for c in connections]

    def connect(self, synName, pre_path, post_path, weight, threshold, delay):
        """
        NOTE: This connects two compartment. Not to be confused with moose.connect
        """

        postcomp = moose.Compartment(post_path)

        # We usually try to reuse an existing SynChan - event based SynChans
        # have an array of weights and delays and can represent multiple
        # synapses i.e.  a new element of the weights and delays array is
        # created every time a 'synapse' message connects to the SynChan (from
        # 'event' of spikegen) BUT for a graded synapse with a lookup table
        # output connected to 'activation' message, not to 'synapse' message, we
        # make a new synapse everytime ALSO for a saturating synapse i.e.
        # KinSynChan, we always make a new synapse as KinSynChan is not meant to
        # represent multiple synapses
        libsyn = moose.SynChan(self.libraryPath+'/'+synName)
        gradedchild = mu.get_child_Mstring(libsyn, 'graded')

        # create a new synapse
        if libsyn.className == 'KinSynChan' or gradedchild.value == 'True': 
            syn_name_full = moose_methods.moosePath(synName
                    , mu.underscorize(pre_path)
                    )
            self.make_new_synapse(synName, postcomp, syn_name_full)
        else:
            # See debug/bugs for more details.
            # NOTE: Change the debug/bugs to enable/disable this bug.
            if bugs.BUG_NetworkML_500:
                syn_name_full = moose_methods.moosePath(synName
                        , mu.underscorize(pre_path)
                        )
                self.make_new_synapse(synName, postcomp, syn_name_full)

            else: # If the above bug is fixed.
                syn_name_full = synName
                if not moose.exists(post_path+'/'+syn_name_full):
                    self.make_new_synapse(synName, postcomp, syn_name_full)

        # wrap the synapse in this compartment
        synPath = moose_methods.moosePath(post_path, syn_name_full)
        syn = moose.SynChan(synPath)

        gradedchild = mu.get_child_Mstring(syn, 'graded')

        # weights are set at the end according to whether the synapse is graded
        # or event-based


        # connect pre-comp Vm (if graded) OR spikegen/timetable (if event-based)
        # to the synapse

        # graded synapse
        if gradedchild.value=='True': 
            table = moose.Table(syn.path+"/graded_table")
            # always connect source to input - else 'cannot create message'
            # error.
            precomp = moose.Compartment(pre_path)
            self.connectWrapper(precomp, "VmOut", table, "msgInput")

            # since there is no weight field for a graded synapse
            # (no 'synapse' message connected),
            # I set the Gbar to weight*Gbar
            syn.Gbar = weight*syn.Gbar

        # Event based synapse
        else: 
            # synapse could be connected to spikegen at pre-compartment OR a
            # file!
            if 'file' not in pre_path:
                precomp = moose.Compartment(pre_path)
                # if spikegen for this synapse doesn't exist in this
                # compartment, create it spikegens for different synapse_types
                # can have different thresholds
                if not moose.exists(pre_path+'/'+synName+'_spikegen'):
                    spikegen = moose.SpikeGen(pre_path+'/'+synName+'_spikegen')
                    # connect the compartment Vm to the spikegen
                    self.connectWrapper(precomp, "VmOut", spikegen, "Vm")
                    # spikegens for different synapse_types can have different
                    # thresholds
                    spikegen.threshold = threshold
                    # This ensures that spike is generated only on leading edge.
                    spikegen.edgeTriggered = 1 

                    # usually events are raised at every time step that Vm >
                    # Threshold, can set either edgeTriggered as above or
                    # refractT
                    #spikegen.refractT = 0.25e-3 

                # wrap the spikegen in this compartment
                spikegen = moose.SpikeGen(pre_path+'/'+synName+'_spikegen') 

                # connect the spikegen to the synapse note that you need to use
                # Synapse (auto-created) under SynChan to get/set weights ,
                # addSpike-s etc.  can get the Synapse element by
                # moose.Synapse(syn.path+'/synapse') or syn.synapse Synpase is
                # an array element, first add to it, to addSpike-s, get/set
                # weights, etc.
                syn.synapse.num += 1

                # BUG BUG BUG in MOOSE:
                # Subhasis said addSpike always adds to the first element in
                # syn.synapse Create a new synapse above everytime
                self.connectWrapper(spikegen, "event", syn.synapse[-1], "addSpike")
            else:
                # if connected to a file, create a timetable,
                # put in a field specifying the connected filenumbers to this segment,
                # and leave it for simulation-time connection
                ## pre_path is 'file[+<glomnum>]_<filenum1>[_<filenum2>...]' i.e. glomnum could be present
                filesplit = pre_path.split('+')
                if len(filesplit) == 2:
                    glomsplit = filesplit[1].split('_', 1)
                    glomstr = '_'+glomsplit[0]
                    filenums = glomsplit[1]
                else:
                    glomstr = ''
                    filenums = pre_path.split('_', 1)[1]
                tt_path = postcomp.path+'/'+syn_name_full+glomstr+'_tt'
                if not moose.exists(tt_path):
                    # if timetable for this synapse doesn't exist in this
                    # compartment, create it, and add the field 'fileNumbers'
                    tt = moose.TimeTable(tt_path)
                    tt.addField('fileNumbers')
                    tt.setField('fileNumbers',filenums)

                    # Be careful to connect the timetable only once while
                    # creating it as below: note that you need to use Synapse
                    # (auto-created) under SynChan to get/set weights ,
                    # addSpike-s etc.  can get the Synapse element by
                    # moose.Synapse(syn.path+'/synapse') or syn.synapse Synpase
                    # is an array element, first add to it, to addSpike-s,
                    # get/set weights, etc.
                    syn.synapse.num += 1

                    # BUG BUG BUG in MOOSE:
                    # Subhasis said addSpike always adds to the first element in
                    # syn.synapse Create a new synapse above everytime
                    m = self.connectWrapper(tt,"event", syn.synapse[-1], "addSpike")
                else:
                    # if it exists, append file number to the field 'fileNumbers'
                    tt = moose.TimeTable(tt_path)
                    # append filenumbers from
                    # 'file[+<glomnum>]_<filenumber1>[_<filenumber2>...]'
                    filenums = moose_methods.moosePath(tt.getField('fileNumbers')
                            , filenums)
                    tt.setField('fileNumbers', filenums)

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
            #print 'len = ',len(syn.synapse)
            #for i,syn_syn in enumerate(syn.synapse):
            #    print i,'th weight =',syn_syn.weight,'\n'

    def make_new_synapse(self, synName, postcomp, syn_name_full):
        # if channel does not exist in library load it from xml file
        if not moose.exists(self.libraryPath + '/' + synName):
            cmlR = ChannelML.ChannelML(self.nml_params)
            cmlR.readChannelMLFromFile(synName+'.xml')

        # deep copies the library synapse to an instance under postcomp named as
        # <arg3>
        synid = moose.copy(moose.Neutral(self.libraryPath+'/'+synName)
                , postcomp
                , syn_name_full
                )
        syn = moose.SynChan(synid)
        childmgblock = mu.get_child_Mstring(syn,'mgblock')

        # connect the post compartment to the synapse
        # If NMDA synapse based on mgblock, connect to mgblock
        if childmgblock.value=='True': 
            mgblock = moose.Mg_block(syn.path+'/mgblock')
            self.connectWrapper(postcomp,"channel", mgblock, "channel")
        # if SynChan or even NMDAChan, connect normally
        else: 
            self.connectWrapper(postcomp,"channel", syn, "channel")
