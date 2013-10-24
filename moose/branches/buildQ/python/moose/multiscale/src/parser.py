# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
from lxml import etree

def parseAndValidateWithSchema(modelName, modelPath) :
    
    prefixPath = ''
    if modelName == 'xml' :
      schemaPath = os.path.join(prefixPath, 'moose_xml/moose.xsd')

    try :
      schemaH = open(schemaPath, "r")
      schemaText = schemaH.read()
      schemaH.close()
    except Exception as e :
      debug.printDebug("WARN", "Error reading schema for validation."+
        " Falling back to validation-disabled parser."
        + " Failed with error {0}".format(e))
      return parseWithoutValidation(modelName, modelPath)
    # Now we have the schema text 
    schema = etree.XMLSchema(etree.XML(schemaText))
    xmlParser = etree.XMLParser(schema=schema)
    with open(modelPath, "r") as xmlTextFile :
        return etree.parse(xmlTextFile, xmlParser)

def parseWithoutValidation(modelName, modelPath) :
    try :
      xmlRootElem = etree.parse(modelPath)
    except Exception as e :
      debug.printDebug("ERROR", "Parsing failed. {0}".format(e))
      return 
    return xmlRootElem 

def parseModels(commandLineArgs, validate=False) :
    '''
    Parse given models.
   
    '''
    xmlRootElemDict = dict()

    models = vars(commandLineArgs)
    for model in models :
      if models[model] :
        modelPath = models[model]
        debug.printDebug("INFO", "Parsing {0}".format(models[model]))
        if validate :
          # parse model and valid it with schama
          modelXMLRootElem = parseAndValidateWithSchema(model, modelPath)
        else :
          # Simple parse the model without validating it with schema.
          modelXMLRootElem = parseWithoutValidation(model, modelPath)
        xmlRootElemDict[model] = modelXMLRootElem 
    return xmlRootElemDict 

