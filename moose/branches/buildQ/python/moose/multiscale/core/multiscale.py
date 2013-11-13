# Basic imports
import os 
import sys 
import logging 
import debug.debug as debug
import inspect
from lxml import etree
sys.path.append("../../../python")
import moose

logger = logging.getLogger('multiscale')

import networkx as nx
import parser.nml_parser as nml_parser
import collections 
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

  def neuroml2Network(self, nmls, test=False) :
    debug.printDebug("INFO", "Converting neuroml to netowrk graph")
    assert len(nmls) == 1
    for nmlElem, nmlPath in nmls :
      for elem in nmlElem.getroot() :
        if xml_helper.isTaggedWith(elem, "cells") :
          for cell in elem :
            print self.cellToGraph(cell)
        elif xml_helper.isTaggedWith(elem, "populations") :
          print "Population of cell is found"
          for population in elem :
            self.insertInstanceOfCell(population)
        else :
          debug.printDebug("WARN"
              , "Not supported yet {0}".format(elem))
    debug.printDebug("INFO", "Dumping graph into a dot file")
    graph_helper.write_dot(self.mooseG, 'graphs/moose.dot')


  def cellToGraph(self, cell) :
    '''
    Convert a cell to graph. 
    '''
    cellName = cell.get('name')
    cellG = nx.DiGraph(name=cellName)
    for a in cell :
      if xml_helper.isTaggedWith(a, "segments") :
        for segment in a :
          # Segment has a id, name and cable id. Cable is essentially
          # segmentGroup in nml2. If no parent is specified then segment is
          # parent.
          segId = segment.get('id')
          segName = segment.get('name')
          nodeId = (cellName, segId)   # This will be unique
          cellG.add_node(nodeId)

          # Empty bio-physical properties.
          cellG.node[nodeId]['name'] = cellName
          cellG.node[nodeId]['biophysics'] = dict()

          if segment.get('cable') :
            cableId = segment.get('cable') 
            cellG.node[nodeId]['cable'] = cableId
          if segment.get('parent') :
            # Add an edge from parent to child
            parentNodeId = (cellName, segment.get('parent'))
            cellG.add_edge(parentNodeId, nodeId)
          # Attach proximal and distal information.
          for ch in segment :
            if xml_helper.isTaggedWith(ch, "proximal") :
              cellG.node[nodeId]['proximal'] = ch
            elif xml_helper.isTaggedWith(ch, "distal") :
              cellG.node[nodeId]['distal'] = ch
            else :
              debug.printDebug("WARN"
                  , "This element {0} is not supported yet".format(ch.tag)
                  , frame=inspect.currentframe())
      elif xml_helper.isTaggedWith(a, "notes") : pass
      elif xml_helper.isTaggedWith(a, "cables") :
        for cable in a :
          cableId = cable.get('id')
          cableName = cable.get('name')
          # Also find the name of groups this cable can belong to.
          groups = [x.text for x in cable.xpath("./*[local-name()='group']")]
          # Find the nodes which are part of this cable and attach the cable to
          # these nodes.
          for n in cellG.nodes() :
            if cellG.node[n]['cable'] == cableId :
              # This cable (and this node) belongs to that many groups.
              cellG.node[n]['belongsTo'] = groups
            else : pass
      elif xml_helper.isTaggedWith(a, "biophysics") :
        for elem in a :
          if xml_helper.isTaggedWith(elem, "mechanism") :
            mechName = elem.get('name')
            mechType = elem.get('type')
            mech = (mechType, mechName)
            params = elem.xpath("./*[local-name()='parameter']")
            for p in params :
              # This mechanism belong to many groups of cable. We have already
              # attached groups to segments. Get these groups and attach these
              # mechanism to nodes.
              pName = p.get('name')
              pValue = p.get('value')
              grps = [x.text for x in p.xpath("./*[local-name()='group']")]
              for g in grps :
                for n in cellG.nodes() :
                  if g in cellG.node[n]['belongsTo'] :
                    cellG.node[n]['biophysics'][mech] = p
            varParams = elem.xpath("./*[local-name()='variable_parameter']")
            if varParams :
              debug.printDebug("TODO", "Variable parameters are not implemented"
                  , frame=inspect.currentframe())

          else :
            debug.printDebug("TODO", "Not implemented : {0}".format(elem)
                , frame=inspect.currentframe())
      else :
        debug.printDebug("WARN", "{0} not supported yet".format(a)
            , frame=inspect.currentframe())
    return cellG

  def insertInstanceOfCell(self, p) :
    '''
    Insert a instance of cell into the network of cell; the graph is called
    cellG.
    '''
    instanceOf = p.get('cell_type')
    name = p.get('name')
    for cell in p :
      if xml_helper.isTaggedWith(cell, "properties") :
        print "Properties are found in cell"
      elif xml_helper.isTaggedWith(cell, "instances") :
        print "Instance of cell is found."
      else :
        debug.printDebug("WARN"
            , "This element {0} is not supported yet.".format(cell.tag))


 
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


  # This is the entry point of this class.
  def buildMultiscaleModel(self) :
      debug.printDebug("INFO", "Starting to build multiscale model")   
      if self.xmlDict['nml'] :
        self.neuroml2Network(self.xmlDict['nml'], test=False)


  def exit(self) :
    # Clean up before you leave
    debug.printDebug("WARN", "Cleaning up before existing..")
    sys.exit(0)

  


