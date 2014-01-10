# This file process mumbl.
#!/usr/bin/env python 

"""mumbl.py: This file reads the mumbl file and load it onto moose. 
This class is entry point of multiscale modelling.

Last modified: Fri Jan 10, 2014  06:02PM

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@iitb.ac.in"
__status__           = "Development"

import debug.debug as debug
import inspect
import os
import sys
import moose
import helper.moose_methods as moose_methods
import debug.logger as logger
import core.types as types
import core.config as config
import logging
import inspect

class Mumble():
    """ Mumble: Class for loading mumble onto moose.
    """
    def __init__(self, mumbl):
        self.mumblElem = mumbl[0]
        self.logger = logging.getLogger('mumble')
        self.rootElem = self.mumblElem.getroot()
        self.mumblPath = mumbl[1]
        self.mumblRootPath = os.path.dirname(self.mumblPath)
        self.global_ = self.rootElem.get('global')
        self.countElecModels = 0
        self.countChemModels = 0
        self.compartmentName = 'Compartment'
        self.mumblPath = config.mumblePath
        self.adaptorPath = os.path.join(self.mumblPath, 'Adaptor')
        self.cellPath = config.cellPath
        self.nmlPath = config.nmlPath
        self.chemPath = os.path.join(self.mumblPath, 'Chemical')
        self.elecPath = os.path.join(self.mumblPath, 'Electrical')
        moose.Neutral(self.mumblPath)
        moose.Neutral(self.chemPath)
        moose.Neutral(self.elecPath)
        moose.Neutral(self.adaptorPath)
        self.adaptorCount = 0

        # clock
        self.mumbleClockId = 3
        self.dt = 1e-03
        moose.setClock(self.mumbleClockId, self.dt)
        # Insert each model to this list, if model is already added then raise a
        # warning and don't insert the model.
        self.modelList = list()
        self.speciesDict = types.DoubleDict()

    def initPaths(self, paths):
        """
        Initialize all parents.
        """
        ps = paths.split('/')
        p = ""
        for i in ps:
            p += ("/"+i)
            moose.Neutral(p)
        
    def load(self):
        """ Lead mumble element tree
        """
        debug.printDebug("INFO", "Loading mumble")
        [self.loadModel(model) for model in self.rootElem.findall('model') ]

        # Mappings from electrical to chemical and vice versa belongs to
        # "domain"
        domains = self.rootElem.findall('domain')
        [ self.mapDomainOntoDomain(d) for d in domains ]


    def loadModel(self, modelXml):
        """
        Load additional model to moose.
        """
        modelId = modelXml.get('id')
        if modelId in self.modelList:
            debug.printDebug("INFO"
                    , "Two models have same id {0}! Ignoring...".format(modelId)
                    )
            return 
        self.modelList.append(modelId)

        # Get the type of model and call appropriate function.
        modelType = modelXml.get('domain_type')
        if modelType == "electrical":
            self.loadElectricalModel(modelXml)
        elif modelType == 'chemical':
            self.loadChemicalModel(modelXml)
        else:
            debug.printDebug("TODO"
                    , "{0} : Un-supported Mumbl model".format(modelType)
                    , frame = inspect.currentframe()
                    )

    def createMoosePathForModel(self, modelNo, modelType, species=None):
        """
        Create moose path for this chemical model.
        """
        if modelType == "chemical":
            modelPath = os.path.join(self.chemPath, '{0}')
            if species:
                return modelPath.format("chemical", self.countChemModels)
            else:
                return modelPath.format("chemical"
                        , species
                        , self.countChemModels
                        )
           
        elif modelType == "electrical":
            modelPath = "/models/electrical/{0}_{1}"
            return modelPath.format("e", self.countElecModels)
        else:
            debug.printDebug("TODO"
                    , "Unsupported model type : {0}".format(modelType)
                    )
            raise UserWarning, "Unsupported model type"
        return None 

    def loadElectricalModel(self, modelXml):
        """
        Load electrical model.
        """
        if modelXml.get('already_loaded') == "true":
            return 
        self.countElecModels += 1
        debug.printDebug("TODO"
                , "Elec model is not in NML. Ignoring for now..."
                , frame = inspect.currentframe()
                )

    def loadChemicalModel(self, modelXml):
        """
        This function load a chemical model described in mumble. Mumble can
        point to a model described in some file. Or the model may have been
        already loaded and one can specify the simulation specific details. We
        only spport Moose simulator.

        @param modelXml: This is xml elements.
        @type  param:  lxml.Elements

        @return:  None
        @rtype : None type.
        """

        if modelXml.get('already_loaded') == "true":
            debug.printDebug("DEBUG"
                    , "This model is alreay loaded. Doing nothing..."
                    )
            return

        # Else load the model onto moose.
        self.countChemModels += 1

        if modelXml.get('load_using_external_script') == "yes":
            debug.printDebug("TODO"
                    , "Loading user external script is not supported yet."
                    , frame = inspect.currentframe()
                    )
            raise UserWarning, "Unimplemented feature"

        # Otherwise load the model.
        modelFilePath = modelXml.get('file_path')
        modelFilePath = os.path.join(self.mumblRootPath, modelFilePath)
        if not os.path.exists(modelFilePath):
            debug.printDebug("ERR"
                    , "File {0} not found.".format(modelFilePath)
                    , frame = inspect.currentframe()
                    )
            raise IOError, "Failed to open a file"

        # get compartments and add species to these compartments.
        compsXml = modelXml.find('compartments')
        comps = compsXml.findall('compartment')
        [ self.addCompartment(compsXml.attrib, c, "chemical") for c in comps ]

    def addCompartment(self, compsAttribs, xmlElem, chemType):
        """Add compartment if not exists and inject species into add.

        Ideally compartment must exist. 
        
        The id of compartment in xmlElement should be compatible with neuroml
        comparment ids.
        """
        if chemType != "chemical":
            raise UserWarning, "Only chemical models are supported"

        compPath = os.path.join(
                self.chemPath
                , moose_methods.moosePath(self.compartmentName , xmlElem.get('id'))
                )
        moose.Neutral(compPath)

        # Add pools to this compartment
        pools = xmlElem.findall('pool')
        for p in pools:
            speciesName = p.get('species')
            self.speciesDict.insertUniqueVal(speciesName)
            pool = os.path.join(compPath, speciesName)
            self.logger.info('Creating pool with path {}'.format(pool))
            poolComp = moose.Pool(pool)
            poolComp.conc = moose_methods.stringToFloat(p.get('conc'))
            poolComp.speciesId = self.speciesDict.get(speciesName)
        
    def mapDomainOntoDomain(self, domain):
        """
        Each <domain> element is essentially a mapping from a compartment to
        another one. 

        """
        debug.printDebug("STEP", "Creating mapping..")
        xmlType = domain.get('xml')
        population = domain.get('population')
        segment = domain.get('segment')
        id = domain.get('instance_id')
        path = os.path.join(self.cellPath, population, segment)

        if xmlType == "neuroml":
            fullpath = moose_methods.moosePath(path, id)
        else:
            debug.printDebug("WARN"
                    , "Unsupported XML type %s" % xmlType)
            fullpath = moose_methods.moosePath(path, id)

        if domain.get('postfix') is not None:
            fullpath = os.path.join(fullpath, domain.get('postfix'))

        mappings = domain.findall('mapping')
        [self.mapping(a, fullpath) for a in mappings]

    def mapping(self, adaptor, moosePath):
        """
        Set up an adaptor for a given moose path
        """
        direction = adaptor.get('direction')
        if direction is None:
            direction = 'out'
        else: pass
        tgts = adaptor.findall('target')
        if direction == "in":
            [self.inTarget(t, moosePath) for t in tgts]
        elif direction == "out":
            [self.outTarget(t, moosePath) for t in tgts]
        else:
            raise UserWarning, "Unsupported type or parameter", direction

    def inTarget(self, tgt, moosePath):
        """Set up incoming targets.
        """
        mooseSrc = moose.Neutral(moosePath)

        # Get the target.
        compType = tgt.get('type')
        if compType == "chemical":
            self.logger.debug("Adding a target of chemtype")
            # in which compartment and which type of pool
            compId = tgt.get('compartment_id')
            if compId is None:
                raise UserWarning, "Missing parament or value"
            species = tgt.get('species')
            poolId = moose_methods.moosePath(self.compartmentName, compId)
            poolPath = os.path.join(self.chemPath, poolId, species)
            mooseTgt = moose.Pool(poolPath)
        elif compType == "electrical":
            mooseTgt = self.createElecCompartment(tgt)
        else: 
            debug.printDebug("TODO", "Unsupported compartment type")
            raise UserWarning("Unsupported compartment type %s" % compType)
       
        # We need to send message now based on relation. This is done using
        # moose.Adaptor class.
        relationXml = tgt.find('relation')
        # Here we modify the mooseSrc according to mooseTgt. MooseTgt is read
        # therefore it is the first arguement in function.
        self.setAdaptor(mooseTgt, mooseSrc, relationXml)
        moose.reinit()

    def outTarget(self, tgt, moosePath):
        """Setup outgoing targets.
        """
        debug.printDebug("TODO", "Implement out-target")
        return


    def setAdaptor(self, src, tgt, relationXml):
        '''
        Construct a message.

        src: It is the moose compartment which is read by moose.
        tgt: It is the moose compartment to which we write.
        '''

        self.adaptorCount += 1
        srcPath = src.path
        tgtPath = tgt.path
        adaptorPath = moose.Adaptor(
                os.path.join(
                    self.adaptorPath
                    , 'adapt{}'.format(self.adaptorCount)
                    )
                )
        self.logger.debug(
                "Adaptor: {} to {}, Encoded {}".format(
                    srcPath
                    , tgtPath
                    , adaptorPath
                    )
                )
        adaptor = moose.Adaptor(adaptorPath)

        # Create a adaptor which reads from src and update tgt.
        lhsXml = relationXml.find('lhs')
        rhsXml = relationXml.find('rhs')
        lhsVar = lhsXml.text
        rhsVar = rhsXml.text
        adaptor.setField('scale', float(rhsXml.get('scale', 1.0)))
        adaptor.setField('inputOffset', - float(rhsXml.get('offset', 0.0)))
        self.logger.info(
                'Setting adaptor between {}/{} and {}/{}'.format(
                    src.path
                    , lhsVar
                    , tgt.path
                    , rhsVar
                    )
                )
        # Connect
        try:
            var = 'get_'+lhsVar
            moose.connect(adaptor, 'requestField', src, var)
        except Exception as e:
            self.logger.error(
                    'Failed to connect var {} of {} with adaptor input'.format(
                        var, src.path
                        )
                    )
            self.logger.info(
                    "Avalilable fields are {}".format(
                        moose.showfield(src)
                        )
                    )
            sys.exit()
        try:
            var = 'set_'+rhsVar
            moose.connect(adaptor, 'outputSrc', tgt, 'set_'+rhsVar, 'OneToOne')
        except Exception as e:
            self.logger.error(
                    'Failed to connect var {} of {} with adaptor input'.format(
                        var, tgt.path
                        )
                    )
            self.logger.info(
                    "Avalilable fields are {}".format(
                        moose.showfield(tgt)
                        )
                    )
            sys.exit()
        moose.useClock(self.mumbleClockId, adaptor.path, 'process')
