#!/usr/bin/env python

__author__  = "Dilawar Singh <dilawars@ncbs.res.in>"

__log__     = """


"""

def segGrp2Color(grpName, grps):
    """
    Convert a  segmenet group name to an appropriate color.
    """
    index = 1
    for i in grps:
        if i != grpName:
            index += 1
        else:
            break
    level = format((index * 16777215)/len(grps), '06x')
    return "color=\"#"+level+"\""

def attachToNodesInGroup(cellG, groups, pName, pValue):
    """
    Attach a key=vlaue pair to node; these nodes belong to certain groups.
    """
    for g in groups:
        for n in cellG.nodes():
            if g in cellG.node[n]['belongsTo']:
                cellG.node[n][pName] = pValue


def write_dot(g, filename):
    """
    Write this graph to a graphviz file.
    """
    dotFile = []
    dotFile.append("digraph moose {")
    #dotFile.append("dim=3")
    #dotFile.append("node[shape=box]")
    segmentGroups = g.graph['gProp']
    for n in g.nodes():
        nodeLabel = "["
        if 'proximal' in g.node[n].keys():
            x = (g.node[n]['proximal'][0] + g.node[n]['distal'][0])/2
            y = (g.node[n]['proximal'][1] + g.node[n]['distal'][1])/2
            z = (g.node[n]['proximal'][2] + g.node[n]['distal'][2])/2
            nodeLabel += "pos=\"{0},{1},{2}!\,".format(x,y,z)

        # color the segment groups
        segG = g.node[n].get('segmentGroup')
        if segG:
            colorName = segGrp2Color(segG, g.graph['gProp']['segmentGroup'])
            nodeLabel += "style=filled,"+colorName+","

        nodeLabel += "]"
        dotFile.append(g.node[n]['label'] + " " + nodeLabel)
        for (f, t) in g.edges():
            dotFile.append(g.node[f]['label'] + " -> " + g.node[t]['label'])

    # End the file
    dotFile.append("}")
    txt = "\n".join(dotFile)
    with open(filename, "w") as outFile:
        outFile.write(txt)
