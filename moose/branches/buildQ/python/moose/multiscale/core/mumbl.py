# This file process mumbl.
#!/usr/bin/env python 

"""mumbl.py: This file reads the mumbl file and load it onto moose. 
This class is entry point of multiscale modelling.

Last modified: Mon Dec 23, 2013  01:39AM

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
import moose
import helper.moose_methods as moose_methods

class Mumble(object):
    """ Mumble: Class for loading mumble onto moose.
    """
    def __init__(self, mumbl):
        self.mumblElem = mumbl[0]
        self.rootElem = self.mumblElem.getroot()
        self.mumblPath = mumbl[1]
        self.mumblRootPath = os.path.dirname(self.mumblPath)
        self.global_ = self.rootElem.get('global')
        self.countElecModels = 0
        self.countChemModels = 0
        self.mumblPath = '/mumbl'
        self.chemPath = os.path.join(self.mumblPath, 'Chemical')
        self.elecPath = os.path.join(self.mumblPath, 'Electrical')
        self.poolPath = os.path.join(self.mumblPath, 'Pool')
        moose.Neutral(self.mumblPath)
        moose.Neutral(self.chemPath)
        moose.Neutral(self.elecPath)
        moose.Neutral(self.poolPath)



        # Insert each model to this list, if model is already added then raise a
        # warning and don't insert the model.
        self.modelList = list()

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
        Load chemical model.
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
        else:
            # load here 
            modelFilePath = modelXml.get('file_path')
            modelFilePath = os.path.join(self.mumblRootPath, modelFilePath)
            if not os.path.exists(modelFilePath):
                debug.printDebug("ERR"
                        , "File {0} not found.".format(modelFilePath)
                        , frame = inspect.currentframe()
                        )
                raise RuntimeError, "Failed to open a file"

            # get compartments and add species to these compartments.
            compsXml = modelXml.find('compartments')
            comps = compsXml.findall('compartment')
            [ self.addCompartment(compsXml.attrib, c) for c in comps ]

    def addCompartment(self, compsAttribs, xmlElem):
        """Add compartment if not exists and inject species into add.

        Ideally compartment must exist. The id of compartment in xmlElement
        should be compatible with neuroml comparment ids.
        """
        compPath = os.path.join(self.chemPath
                , moose_methods.moosePath("Compartment" , xmlElem.get('id'))
                )
        pools = xmlElem.findall('pool')
        for p in pools:
            pool = os.path.join(self.poolPath, p.get('species'))
            poolComp = moose.ChemCompt(pool)

            # Species path.
            speciesPath = os.path.join(self.chemPath, p.get('species'))
            speciesComp = moose.Pool(speciesPath)
            speciesComp.speciesId = poolComp.getId().getValue()
            speciesComp.concInit = moose_methods.stringToFloat(p.get('conc'))
