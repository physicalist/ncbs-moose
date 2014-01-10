"""
NeuroML.py is the preferred interface to read NeuroML files.

    Instantiate NeuroML class, and thence use method: readNeuroMLFromFile(...)
    to load NeuroML from a file:

 - The file could contain all required levels 1, 2 and 3 - Network , Morph and
 Channel.

 - The file could have only L3 (network) with L1 (channels/synapses) and L2
 (cells) spread over multiple files; these multiple files should be in the same
 directory named as <chan/syn_name>.xml or <cell_name>.xml or
 <cell_name>.morph.xml (essentially as generated by neuroConstruct's export).

 - But, if your lower level L1 and L2 xml files are elsewise, use the separate
 Channel, Morph and NetworkML loaders in moose.neuroml.<...> .

For testing, you can also call this from the command line with a neuroML file
as argument.

CHANGE LOG:

 Description: class NeuroML for loading NeuroML from single file into MOOSE

 Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE

 Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, ported to parallel

 MOOSE Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, further
 changes for parallel MOOSE.

 Dilawar Singh; Fixed parsing errors when parsing some standard models.

"""


from xml.etree import cElementTree as ET
import sys
import MorphML
import NetworkML
import ChannelML
import moose
import moose.utils
import moose.neuroml.utils as mnu
import debug.debug as debug
import core.config as config

from os import path


class NeuroML(ChannelML.ChannelML, NetworkML.NetworkML, MorphML.MorphML):

    """
    This class parses neuroml models and build moose-data structures.

    """
    def __init__(self):
        self.model_dir = ""
        self.lengthUnits = ""
        self.temperature = 25
        self._CELSIUS_default = ""
        self.temperature_default = True
        self.nml_params = None
        self.channelUnits = "Physiological Units"
        moose.Neutral('/neuroml')
        self.libraryPath = config.libraryPath
        self.cellPath = config.cellPath

    def readNeuroMLFromFile(self, filename, params=dict()):

        """
        For the format of params required to tweak what cells are loaded,
        refer to the doc string of NetworkML.readNetworkMLFromFile().
        Returns (populationDict,projectionDict),
        see doc string of NetworkML.readNetworkML() for details.
        """

        debug.printDebug("STEP"
                , "Loading neuroml file {0} ... ".format(filename))
        # creates /library in MOOSE tree; elif present, wraps
        tree = ET.parse(filename)
        root_element = tree.getroot()
        self.model_dir = path.dirname(path.abspath(filename))
        try:
            self.lengthUnits = root_element.attrib['lengthUnits']
        except KeyError as e:
            # This is preferred in NeuroML v2
            self.lengthUnits = root_element.attrib['length_units']
        except Exception as e:
            debug.printDebug("WARN", "Failed to get length_unit", e)

        # gets replaced below if tag for temperature is present
        self.temperature = self._CELSIUS_default

        for mp in root_element.findall('.//{'+mnu.meta_ns+'}property'):
            tagname = mp.attrib['tag']
            if 'temperature' in tagname:
                self.temperature = float(mp.attrib['value'])
                self.temperature_default = False
        if self.temperature_default:
            debug.printDebug( "INFO"
                , "Using default temperature of % C".format(self.temperature)
                )
        self.nml_params = {
                'temperature': self.temperature
                , 'model_dir': self.model_dir
        }

        #print "Loading cell definitions into MOOSE /library ..."
        mmlR = MorphML.MorphML(self.nml_params)
        self.cellsDict = {}
        for cells in root_element.findall('.//{'+mnu.neuroml_ns+'}cells'):
            for cell in cells.findall('.//{'+mnu.neuroml_ns+'}cell'):
                cellDict = mmlR.readMorphML(
                        cell
                        , params={}
                        , lengthUnits=self.lengthUnits
                        )
                self.cellsDict.update(cellDict)

        #print "Loading individual cells into MOOSE root ... "
        nmlR = NetworkML.NetworkML(self.nml_params)
        return nmlR.readNetworkML(
                root_element
                , self.cellsDict
                , params=params
                , lengthUnits=self.lengthUnits
                )

        # Loading channels and synapses into MOOSE into neuroml library
        cmlR = ChannelML.ChannelML(self.nml_params)

    def channelToMoose(self, cmlR, channels):
        for channel in channels.findall('.//{'+mnu.cml_ns+'}channel_type'):
            # ideally I should read in extra params
            # from within the channel_type element and put those in also.
            # Global params should override local ones.
            cmlR.readChannelML(channel, params={}, units=self.channelUnits)
        for synapse in channels.findall('.//{'+mnu.cml_ns+'}synapse_type'):
            cmlR.readSynapseML(synapse, units=self.channelUnits)
        for ionConc in channels.findall('.//{'+mnu.cml_ns+'}ion_concentration'):
            cmlR.readIonConcML(ionConc, units=self.channelUnits)


    def loadNML(self, nml):
        neuromlR = NeuroML()
        xml, filename = nml
        return neuromlR.readNeuroMLFromFile(filename)
