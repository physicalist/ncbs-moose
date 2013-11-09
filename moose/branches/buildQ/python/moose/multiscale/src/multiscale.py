# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
from lxml import etree

from lxml.builder import E
import networkx as nx
import nml_parser
import cStringIO
import sys
sys.path.append("../../../python")
import moose
import collections 
import os
import helper.graph_methods as helper
class Multiscale :
  
  def __init__(self, xmlDict) :
    self.xmlDict = xmlDict 
    self.xmlExpr = "//*[local-name() = $name]"
    self.rootModelPath = '/model'
    # To hold the topology of the network 
    self.mooseG = nx.MultiDiGraph()
    self.model = moose.Neutral(self.rootModelPath)
    
    debug.printDebug("INFO", "Object of class Multiscale intialized ...")

  def neuroml2Network(self, test=False) :
    debug.printDebug("INFO", "Converting neuroml to netowrk graph")
    nmlElems = self.xmlDict['nml']
    if len(nmlElems) > 1 :
      debug.printDebug("ERR", "More than one nueroml file is not supported yet.")
      self.exit()
    else : 
      nmlElem, nmlPath = nmlElems[0]
    self.nml = nml_parser.parse(nmlPath, silence=True)
    if not self.nml.network :
      debug.printDebug("WARN", "No network found in model. Assuming a single"+
        " cell model. ")
      for cell in self.nml.cell :
        self.insertCellIntoNetwork(cell) 
    else :
      debug.printDebug("INFO", "Build network")


    debug.printDebug("INFO", "Dumping graph into a dot file")
    helper.write_dot(self.mooseG, 'graphs/moose.dot')
  def insertCellIntoNetwork(self, cell, test=False) :
    '''
    This function maps a cell onto Moose simulator.
    '''
    cellId = cell.get_id()
    cellMetaId = cell.get_metaid()
    # get morphology and build the network 
    for segment in cell.morphology.segment :
      # Add this segment to graph, segment ids are unique within a cell.
      nodeId = (cellId, segment.id)
      self.mooseG.add_node(nodeId, label=segment.name)
      if segment.proximal :
        self.mooseG.node[nodeId]['proximal'] = (segment.proximal.x
          , segment.proximal.y, segment.proximal.z, segment.proximal.diameter)
      if segment.distal :
        self.mooseG.node[nodeId]['distal'] = (segment.distal.x
          , segment.distal.y, segment.distal.z, segment.distal.diameter)
      if segment.parent :
        # if parent is given and proximal is not specified, then use distal 
        # of the parent as proximal of child.
        parentNodeId = (cellId, segment.parent.segment)
        self.mooseG.node[nodeId]['proximal'] = self.mooseG.node[parentNodeId]['distal']
        self.mooseG.add_edge(parentNodeId, nodeId)

    # Groups the segment 
    if cell.morphology.segmentGroup :
      for grp in cell.morphology.segmentGroup :
        for member in grp.member :
          nodeId = (cellId, member.segment)
          self.mooseG.node[nodeId]['segmentGroup'] = grp.id 

    if cell.biophysicalProperties :
      prop = cell.biophysicalProperties 
      if prop.membraneProperties :
        memProp = prop.membraneProperties 
        if memProp.channelDensity :
          for cd in memProp.channelDensity :
            cdp = {}
            # fill dict here 
            cdp['id'] = cd.get_id()
            cdp['ionChannel'] = cd.get_ionChannel()
            cdp['condDensity'] = cd.get_condDensity()
            self.attachToCompartment(cellId, cd, cdp)
            
        if memProp.channelPopulation :
          for cp in memProp.channelPopulation :
            chp = {}
            chp['id'] = cp.get_id()
            chp['ionChannel'] = cp.get_ionChannel()
            chp['number'] = cp.get_number()
            self.attachToCompartment(cellId, cp, chp)

        if memProp.specificCapacitance :
          for sc in memProp.specificCapacitance :
            self.attachToCompartment(cellId, sc, sc.value)


  def attachToCompartment(self, cellId, element, value) :
    if element.segment :
      # only for a segment 
      self.mooseG.node[(cellId, int(element.segment))]['channelPopulation'] = \
        value
    elif element.segmentGroup and element.segmentGroup != "all" :
      for n in self.mooseG.nodes() :
        if self.mooseG.node[n]['segmentGroup'] == element.segmentGroup :
          self.mooseG.node[n]['channelPopulation'] = value 
    else : # Attach on all nodes 
      for n in self.mooseG.nodes() :
        self.mooseG.node[n]['channelPopulation'] = value
 

  # This is the entry point of this class.
  def buildMultiscaleModel(self) :
      debug.printDebug("INFO", "Starting to build multiscale model")   
      # NeuroML to moose
      self.neuroml2Network(test=False)





  
  def exit(self) :
    # Clean up before you leave
    debug.printDebug("WARN", "Cleaning up before existing..")
    
    sys.exit(0)

  # Write down the tests, whenever needed.
  




