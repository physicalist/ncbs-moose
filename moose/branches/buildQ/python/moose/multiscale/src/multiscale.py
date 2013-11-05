# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
from lxml import etree


from lxml.builder import E
import nml_parser
import cStringIO
import sys
sys.path.append("../../../python")
import moose

class Multiscale :
  
  def __init__(self, xmlDict) :
    self.xmlDict = xmlDict 
    self.xmlExpr = "//*[local-name() = $name]"
    self.rootModelPath = '/model'
    self.model = moose.Neutral(self.rootModelPath)
    
    debug.printDebug("INFO", "Object of class Multiscale intialized ...")

  def mapIonChannel(self, ionChannelXml) :
    debug.printDebug("INFO"
      , "Mapping ion-channel {0} onto moose".format(ionChannelXml.tag))
    type = ionChannelXml.get('type')
    id = ionChannelXml.get('id')

  def mapIncludedFile(self, filePath) :
    debug.printDebug("INFO", "Mapping included file {0}".format(filePath))
    incXml = etree.parse(filePath)
    for ionChannel in incXml.xpath(self.xmlExpr, name="ionChannel") :
      self.mapIonChannel(ionChannel)

  def neuroML2Moose(self) :
    debug.printDebug("INFO", "Mapping neuroML to moose")
    nmlElems = self.xmlDict['nml']
    if len(nmlElems) > 1 :
      debug.printDebug("ERR", "More than one nueroml file is not supported yet.")
      self.exit()
    else : 
      nmlElem, nmlPath = nmlElems[0]
    self.nml = nml_parser.parse(nmlPath, silence=True)
    for cell in self.nml.cell :
      morphology = cell.morphology 
      compartmentDict = dict()
      for segment in morphology.segment :
        if not segment.parent :
          compartmentPath = os.path.join(self.rootModelPath, segment.name)
          comp = moose.Compartment(compartmentPath)
          compartmentDict[segment.id] = comp
        else  :
          parentPath = compartmentDict[segment.parent.segment].path
          comp = moose.Compartment(os.path.join(parentPath, segment.name))
          compartmentDict[segment.id] = comp
        proximalDiameter = 0.0
        distalDiameter = 0.0
        if segment.proximal :
          comp.x0 = segment.proximal.x
          comp.y0 = segment.proximal.y
          comp.z0 = segment.proximal.z 
          proximalDiameter = segment.proximal.diameter
        if segment.distal :
          comp.x = segment.distal.x
          comp.y = segment.distal.y
          comp.z = segment.distal.z 
          distalDiameter = segment.distal.diameter 
        
        if proximalDiameter > 0.0 and distalDiameter > 0.0 :
          comp.diameter = (proximalDiameter + distalDiameter) / 2
        elif proximalDiameter > 0.0 :
          comp.distalDiameter = proximalDiameter 
        else : 
          comp.diameter = distalDiameter 
        # check if this comparment has a parent 
       
    expr = self.xmlExpr
    modelDir = os.path.dirname(nmlPath)
    for i in  nmlElem.xpath(expr, name="include") :
      incFile = os.path.join(modelDir, i.get('href'))
      if os.path.isfile(incFile) :
        self.mapIncludedFile(incFile)
      else :
        debug.printDebug("WARN", "Included file {0} does not exists".format(incFile))
 

  # This is the entry point of this class.
  def buildMultiscaleModel(self) :
      debug.printDebug("INFO", "Starting to build multiscale model")   
      # NeuroML to moose
      self.neuroML2Moose()


  
  def exit(self) :
    # Clean up before you leave
    debug.printDebug("WARN", "Cleaning up before existing..")
    
    sys.exit(0)

  # Write down the tests, whenever needed.
  




