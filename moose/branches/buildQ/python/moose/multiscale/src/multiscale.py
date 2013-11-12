# Basic imports
import os 
import sys 
import logging 
import debug 
import inspect

logger = logging.getLogger('multiscale')
from lxml import etree

from lxml.builder import E
import networkx as nx
import nml_parser
import cStringIO
import sys
import unittest as ut
sys.path.append("../../../python")
import moose
import collections 
import os
import helper.graph_methods as graph_helper
import helper.xml_methods as xml_helper
class Multiscale :
  
  def __init__(self, xmlDict) :
    self.xmlDict = xmlDict 
    # This expression is useful for getting an element which is not sensitive to
    # name-space.
    self.xmlExpr = "//*[local-name() = $name]"
    self.rootModelPath = '/model'


    # Initialize some attributes also.
    self.mooseG = nx.MultiDiGraph(gProp={'segmentGroup' : set()})
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
    self.nml = nmlElem
    networkElem = xml_helper.getElement(self.nml, self.xmlExpr, "network")
    if not  networkElem :
      debug.printDebug("WARN", "No network found in model. Assuming a single"+
        " cell model. ", inspect.currentframe())
      cellList = xml_helper.getElement(self.nml, self.xmlExpr, "cell")
      for cellXml in cellList :
        self.insertCellIntoNetwork(cellXml) 
    else :
      debug.printDebug("INFO", "Build network")


    debug.printDebug("INFO", "Dumping graph into a dot file")
    graph_helper.write_dot(self.mooseG, 'graphs/moose.dot')
  def insertCellIntoNetwork(self, cell, test=False) :
    '''
    This function takes a XML representation of cell and insert in onto a graph.
    '''
    cellId = cell.get('id')
    cellMetaId = cell.get('metaid')
    # get morphology and build the network 
    for elem in cell :
      # Keep the type of segments in a dictionary.
      if xml_helper.isTaggedWith(elem, "morphology") :
        for e in elem :
          if xml_helper.isTaggedWith(e, "segment") :
            nodeId = (cellId, e.get('id'))
            self.mooseG.add_node(nodeId, label=e.get('name'))
            # Attach the segment to the node
            self.mooseG.node[nodeId]['segment'] = e
            parents = e.xpath("./*[local-name()='parent'][position()=1]") 
            if parents :
              if test :
                assert len(parents) == 1
              parent = parents[0]
              parentNodeId =(cellId, parent.get('segment'))
              try :
                self.mooseG.add_edge(parentNodeId, nodeId)
              except :
                debug.printDebug("ERR", sys.exc_info())
              
          elif xml_helper.isTaggedWith(e, "segmentGroup") :
            groupId = e.get('id')
            self.mooseG.graph['gProp']['segmentGroup'].add(groupId)
            for sg in e :
              if xml_helper.isTaggedWith(sg, "member") :
                nodeId = (cellId, sg.get('segment'))
                self.mooseG.node[nodeId]['segmentGroup'] = groupId
              else :
                debug.printDebug("NOTE", "This tag {0} is not supported.".format(sg.tag))

      elif xml_helper.isTaggedWith(elem, "biophysicalProperties") :
        for prop in elem :
          if xml_helper.isTaggedWith(prop, "membraneProperties") :
            debug.printDebug("INFO", "Attaching properties to membrane")
            for e in prop :
              propName = xml_helper.getTagName(e.tag)
              if e.get('segment') and e.get('segment') != 'all' :
                nodeId = (cellId, e.get('segment'))
                self.mooseG.node[nodeId][('membraneProperties', propName)] = e
              elif e.get('segmentGroup') :
                group = e.get('segmentGroup')
                for n in self.mooseG.nodes() :
                  if self.mooseG.node[n]['segmentGroup'] == group :
                    self.mooseG.node[n][('membraneProperties', propName)] = e
                  else : pass
              else :
                for n in self.mooseG.nodes() :
                  self.mooseG.node[n][('membraneProperties', propName)] = e
          elif  xml_helper.isTaggedWith(prop, "intracellularProperties") :
            debug.printDebug("INFO", "Attaching intracellular properties")
            for e in prop :
              propId = xml_helper.getTagName(e.tag)
              for n in self.mooseG.nodes() :
                if n[0] == cellId :
                  self.mooseG.node[n][('intracellularProperties', propId)] = e

          elif xml_helper.isTaggedWith(prop, "extracellularProperties") :
            print "Extra-cellular properties"
      elif xml_helper.isTaggedWith(elem, "notes") : pass
      elif xml_helper.isTaggedWith(elem, "annotation") : pass
      else :
        debug.printDebug("WARN", "Element {0} not supported yet.".format(elem.tag)
          , inspect.currentframe())

  def attachToCompartment(self, cellId, xmlElem) :
    debug.printDebug("INFO", "Attaching properties to element")
     

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
  


