# Basic imports
import os
import sys
import logging
import debug.debug as debug
import inspect
import lxml.etree as etree

sys.path.append("../../../python")
import moose

logger = logging.getLogger('multiscale')

import networkx as nx
import helper.graph_methods as graph_helper
import helper.xml_methods as xml_helper
import core.dtype as dtype

class Multiscale:

    """
    Class which do multiscale modelling.

    """
    def __init__(self, xmlDict):
        self.xmlDict = xmlDict
        # This expression is useful for getting an element which is not sensitive to
        # name-space.
        self.rootModelPath = '/model'
        # Initialize some attributes also.
        self.mooseG = nx.MultiDiGraph(gProp={'segmentGroup': set()
                                             , 'channels':dict()
                                             , 'synapses':dict()
                                             , 'properties':dict()
                                             , 'inputs':dict()
                                             , 'populations':dict()
                                             }
                                      )
        self.totalSegments = 0
        self.propDict = self.mooseG.graph['gProp']
        self.model = moose.Neutral(self.rootModelPath)
        debug.printDebug("INFO", "Object of class Multiscale intialized ...")

    def neuroml2Network(self, nml, test=False):
        """
        Convert a neuroML file to a graph.
        """
        debug.printDebug("INFO", "Converting neuroml to netowrk graph")
        nmlElem, nmlPath = nml
        cellGraphDict = dict()
        for elem in nmlElem.getroot():
            if xml_helper.isTaggedWith(elem, "cells"):
                for cell in elem:
                    cellName = cell.get('name')
                    cellGraphDict[cellName] = self.cellToGraph(cell)
            elif xml_helper.isTaggedWith(elem, "populations"):
                [self.addPopulation(p, cellGraphDict) for p in elem]
            elif xml_helper.isTaggedWith(elem, "notes"):
                pass
            elif xml_helper.isTaggedWith(elem, "channels"):
                for c in elem:
                    if xml_helper.isTaggedWith(c, "channel_type"):
                        self.addChannel(c)
                    elif xml_helper.isTaggedWith(c, "synapse_type"):
                        self.addSynapse(c)
                    else:
                        debug.printDebug("WARN", "Unknown type {0}".format(c)
                                         , frame=inspect.currentframe())
            elif xml_helper.isTaggedWith(elem, "projections"):
                [self.addProjection(p) for p in elem]
            else:
                debug.printDebug("WARN"
                                , "Not supported yet {0}".format(elem)
                                , frame = inspect.currentframe()
                                )
                debug.printDebug("INFO", "Dumping graph into a dot file")
        graph_helper.write_dot(self.mooseG, 'graphs/moose.dot')


    def addChannel(self, channel):

        """
        Add channel to graph.
        """
        chName = channel.get('name')
        self.mooseG.graph['gProp']['channels'][chName] = channel


    def addSynapse(self, syn):

        """
        Add synapses to graph
        """
        synpName = syn.get('name')
        self.mooseG.graph['gProp']['synapses'][synpName] = syn


    def addPopulation(self, population, cellGDict):
        """
        Add instances of cell to self.mooseG
        """
        popName = population.get('name')
        pType = population.get('cell_type')

        # This cell type belongs to this population.
        self.mooseG.graph['gProp']['populations'][popName] = pType
        cellG = cellGDict[pType]
        assert cellG != None, "Empty cell graph"
        for elem in population:
            if xml_helper.isTaggedWith(elem, "properties"):
                [self.attachProperty(cellG, i) for i in elem]
            elif xml_helper.isTaggedWith(elem, "instances"):
                [self.addInstance(cellG, pType, popName, i) for i in elem]
            else:
                debug.printDebug("TODO", "Population: {0}".format(elem)
                                 , frame = inspect.currentframe()
                                 )

    def attachLabel(self, graph, nodePath):
        """
        attach label to a node when path is given.
        """
        label = "_"+("_".join(nodePath.split('/')))
        graph.node[nodePath]['label'] = label


    def attachProperty(self, graph, prop):
        """
        Attach properties to a  cell graph.
        """
        if prop.get('tag') == "color":
            colorName = prop.get('value')
            for n in graph.nodes():
                graph.node[n]['color'] = colorName
        else: pass

    def addInstance(self, cellG, cellType, populationName, instance):

        """
        Add instance of cellG to self.mooseG

        Node name is (id of instance of cell, name of the cell, id of segment)
        """

        self.totalSegments += cellG.number_of_nodes()
        instId = instance.get('id')
        for n in cellG.nodes():
            cellT, segmentId = n
            assert cellT == cellType
            segName = os.path.join(instId, cellType, segmentId)
            self.mooseG.add_node(segName)
            self.mooseG.node[segName]['population'] = populationName
            for k in cellG.node[n].keys():
                self.mooseG.node[segName][k] = cellG.node[n][k]
            self.attachLabel(self.mooseG, segName)

        for (frm, to) in cellG.edges():
            fromType, fromSegId = frm
            fromPath = os.path.join(instId, fromType, fromSegId)
            toType, toSegId = to
            toPath = os.path.join(instId, toType, toSegId)
            self.mooseG.add_edge(fromPath, toPath)

    def cellToGraph(self, cell):

        """
        Convert a cell to graph.
        """

        cellName = cell.get('name')
        cellG = nx.DiGraph(name=cellName)
        self.propPath = '/properties'
        self.bioPhyPropPath = os.path.join(self.propPath, "biophysics")

        # Attach elements of cell to node in cellG
        for a in cell:
            # Create a path for biophysical properties
            if xml_helper.isTaggedWith(a, "segments"):
                [self.addSegment(cellG, cellName, s) for s in a]
            elif xml_helper.isTaggedWith(a, "notes"):
                pass
            elif xml_helper.isTaggedWith(a, "cables"):
                [self.addCable(cellG, cellName, c) for c in a]
            elif xml_helper.isTaggedWith(a, "biophysics"):
                [self.addBioPhysics(cellG, cellName, b) for b in a]
            elif xml_helper.isTaggedWith(a, "connectivity"):
                [self.addConnectivity(cellG, cellName, con) for con in a]
            else:
                debug.printDebug("TODO"
                                 , "{0} not yet implemented".format(a.tag)
                                 , frame = inspect.currentframe()
                                 )
        return cellG

    def addConnectivity(self, cellG, cellName, elem):
        """
        Add connectivity i.e. synapses to the cellGraph.
        """
        if xml_helper.isTaggedWith(elem, "potential_syn_loc"):
            synType = elem.get('synapse_type')
            grps = elem.xpath("./*[local-name()='group']")
            for n in cellG.nodes():
                if cellG.node[n]['belongsTo'] in grps :
                    cellG.node[n]['synapse'] = synType
        else:
            debug.printDebug("TODO"
                             , "Synapse : {0} not implemented".format(elem.tag)
                             , frame = inspect.currentframe()
                             )

    def addBioPhysics(self, cellG, cellName, elem):
        """
        Add bio-physics to the cell.
        """
        if xml_helper.isTaggedWith(elem, "mechanism"):
            mechName = elem.get('name')
            mechType = elem.get('type')
            mech = os.path.join(self.bioPhyPropPath, mechName, mechType)
            params = elem.xpath("./*[local-name()='parameter']")
            for p in params:
                grps = [x.text for x in p.xpath("./*[local-name()='group']")]
                graph_helper.attachToNodesInGroup(cellG, grps, mech
                                                    , dtype.Parameter(p))
            varParams = elem.xpath("./*[local-name()='variable_parameter']")
            if varParams:
                debug.printDebug("TODO", "Variable parameters are not implemented"
                                , frame=inspect.currentframe())
        elif xml_helper.isTaggedWith(elem, "spec_capacitance"):
            propName = os.path.join(self.bioPhyPropPath
                                    , xml_helper.getTagName(elem.tag))
            for param in elem:
                if xml_helper.isTaggedWith(param, "parameter"):
                    grps = [x.text for x in param.xpath("./*[local-name()='group']")]
                    graph_helper.attachToNodesInGroup(cellG, grps, propName
                                              , dtype.Parameter(param))
                else:
                    debug.printDebug("TODO", "Not implemented: {0}".format(elem)
                                    , frame=inspect.currentframe())
        elif(xml_helper.isTaggedWith(elem, "spec_axial_resistance") or
             xml_helper.isTaggedWith(elem, "init_memb_potential")):
            propPath = os.path.join(self.bioPhyPropPath
                                    , xml_helper.getTagName(elem.tag))
            params = elem.xpath("./*[local-name()='parameter']")
            for p in params :
                grps = p.xpath("./*[local-name()='group']")
                graph_helper.attachToNodesInGroup(cellG, grps, propPath
                                                  , dtype.Parameter(p))
        else:
            debug.printDebug("WARN", "{0} not supported yet".format(elem)
                            , frame=inspect.currentframe())

    def addCable(self, cellG, cellName, cable):
        """
        If a cell belong to this cable then populate belongsTo attribute.
        """
        cableId = cable.get('id')
        cableName = cable.get('name')
        # Also find the name of groups this cable can belong to.
        groups = [x.text for x in cable.xpath("./*[local-name()='group']")]
        # Find the nodes which are part of this cable and attach the cable to
        # these nodes.
        for n in cellG.nodes():
            if cellG.node[n]['cable'] == cableId:
                # This cable (and this node) belongs to that many groups.
                cellG.node[n]['belongsTo'] = groups

    def addSegment(self, cellG, cellName, segment):

        """
        Add segment to cellGraph.
        """
        segId = segment.get('id')
        segName = segment.get('name')
        nodeId = (cellName, segId)   # This will be unique
        cellG.add_node(nodeId)
        cellG.node[nodeId]['name'] = cellName

        if segment.get('cable'):
            cableId = segment.get('cable')
            cellG.node[nodeId]['cable'] = cableId
            if segment.get('parent'):
                # Add an edge from parent to child
                parentNodeId = (cellName, segment.get('parent'))
                cellG.add_edge(parentNodeId, nodeId)
                # Attach proximal and distal information.
                for ch in segment:
                    if xml_helper.isTaggedWith(ch, "proximal"):
                        cellG.node[nodeId]['proximal'] = ch
                    elif xml_helper.isTaggedWith(ch, "distal"):
                        cellG.node[nodeId]['distal'] = ch
                    else:
                        debug.printDebug("WARN"
                            , "This element {0} is not supported yet".format(ch.tag)
                            , frame=inspect.currentframe())

    def addProjection(self, projection):
        projName = projection.get('name')
        source = projection.get('source')
        sourceType = self.propDict['populations'][source]
        assert sourceType
        target = projection.get('target')
        targetType = self.propDict['populations'][target]
        assert targetType

        print "Adding projection {name} : from {} to {} ".format(source
                                                                 , target
                                                                 , name=projName)
        synProps = projection.xpath("./*[local-name()='synapse_props']")
        # There must be only one synapse property in a projection.
        assert len(synProps) == 1
        syp = synProps[0]
        synName = syp.get('synapse_type')
        if synName not in self.mooseG.graph['gProp']['synapses']:
            debug.printDebug("ERR"
                                , "Synapse {0}: Does not exists".format(synName)
                                , frame = inspect.currentframe()
                                )
        # Now attach edges to network
        connections = []
        cns = projection.xpath("./*[local-name()='connections']")[0]
        for con in cns:
            conId = con.get('id')
            preCellInstId = con.get('pre_cell_id')
            assert preCellInstId, "Source cell is None"
            postCellInstId = con.get('post_cell_id')
            assert postCellInstId, "Target cell is None"
            preSegId = con.get('pre_segment_id')
            postSegId = con.get('post_segment_id')
            if preSegId and postSegId :
                preNodeId = os.path.join(preCellInstId, sourceType, preSegId)
                postNodeId = os.path.join(postCellInstId, targetType, postSegId)
                assert self.mooseG.node[preNodeId], "Node not found in graph"
                assert self.mooseG.node[postNodeId], "Node not found in graph"
                self.mooseG.add_edge(preNodeId, postNodeId)
                self.mooseG.edge[preNodeId][postNodeId]['synapse'] = syp
                self.mooseG.edge[preNodeId][postNodeId]['id'] = conId
            else:
                #print "Population -> population"
                pass

    def runTest(self):
        # Check if total no of added segments are equal to no of nodes in graph
        if self.totalSegments != self.mooseG.number_of_nodes():
            debug.printDebug("TEST FAILED", "Node in graphs are either less or more")
            print("+ Expected {0}, exists {1}".format(self.totalSegments
                                                      , self.mooseG.number_of_nodes())
                  )
        else:
            debug.printDebug("TEST PASSED"
                             , "Total {0} nodes added".format(self.totalSegments))
        # Check for nodes with duplicate lables
        nodes = set()
        for n in self.mooseG.nodes():
            nLabel = self.mooseG.node[n]['label']
            if nLabel in nodes:
                debug.printDebug("TEST FAILED"
                                 , "Duplicate node with label {0}".format(nLabel))
            else:
                nodes.add(nLabel)

        # Check for parallel edges in mooseG
        edges = set()
        for e in self.mooseG.edges():
            if e in edges:
                debug.printDebug("TEST FAILED", "Parallel edge : {}".format(e))
            else:
                edges.add(e)

    def buildMultiscaleModel(self):
        debug.printDebug("INFO", "Starting to build multiscale model")
        if self.xmlDict['nml']:
            for nmlXml in self.xmlDict['nml']:
                 self.neuroml2Network(nmlXml, test=False)
        self.runTest()

    def exit(self):
        # Clean up before you leave
        debug.printDebug("WARN", "Cleaning up before existing..")
        sys.exit(0)




