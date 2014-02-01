#!/usr/bin/env python

"""moose_methods.py:  Some helper function related with moose to do multiscale
modelling.

Last modified: Thu Jan 23, 2014  04:53PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@iitb.ac.in"
__status__           = "Development"

import re
import os
import moose

nameSep = '()'

def commonPath(pathA, pathB):
    ''' Find common path at the beginning of two paths. '''
    a = pathA.split('/')
    b = pathB.split('/')
    common = []
    for (i, p) in enumerate(a):
        if a[i] == b[i]: 
            common.append(p)
        else: 
            return '/'.join(common)
    return '/'.join(common)
            


def moosePath(baseName, append):
    """ 
    Append instance index to basename

    TODO: Ideally - should be replace with [ and ]

    """
    if append.isdigit():
        if len(nameSep) == 1:
            return baseName + nameSep + append 
        elif len(nameSep) == 2:
            return baseName + nameSep[0] + append + nameSep[1]
        else:
            raise UserWarning, "Not more than 2 characters are not supported"
    else:
        return os.path.join(baseName, append)


def splitComparmentExpr(expr):
    """ Breaks compartment expression into name and id.
    """
    if len(nameSep) == 1:
        p = re.compile(r'(?P<name>[\w\/\d]+)\{0}(?P<id>\d+)'.format(nameSep[0]))
    else:
        # We have already verified that nameSep is no longer than 2 characters.
        a, b = nameSep 
        p = re.compile(r'(?P<name>[\w\/\d]+)\{0}(?P<id>\d+)\{1}'.format(a, b))
    m = p.match(expr)
    assert m.group('id').isdigit() == True
    return m.group('name'), m.group('id')



def getCompartmentId(compExpr):
    """Get the id of compartment.
    """
    return splitComparmentExpr(compExpr)[1]

def getCompName(compExpr):
    """Get the name of compartment.
    """
    return splitComparmentExpr(compExpr)[0]

def stringToFloat(text):
    text = text.strip()
    if not text:
        return 0.0
    try:
        val = float(text)
        return val
    except Exception:
        raise UserWarning, "Failed to convert {0} to float".format(text)


def dumpMoosePaths(pat, isRoot=True):
    ''' Path is pattern '''
    if not isRoot:
        moose_paths = [x.getPath() for x in moose.wildcardFind(pat)]
    else:
        try:
            moose_paths = [x.getPath() for x in moose.wildcardFind(pat+'/##')]
        except TypeError as e:
            moose_paths = [x.getPath() for x in 
                    moose.wildcardFind(pat.path+'/##')]
    moose_paths = "\n".join(moose_paths)
    print("Moose paths: {0}".format(moose_paths))

def dumpMatchingPaths(path, pat='/##'):
    ''' return the name of path which the closely matched with given path 
    pattern pat is optional.
    '''
    a = path.split('/')
    start = a.pop(0)
    p = moose.wildcardFind(start+'/##')
    common = []
    while len(p) > 0:
        common.append(p)
        start = start+'/'+a.pop(0)
        p = moose.wildcardFind(start+'/##')
        
    matchedPaths = [x.getPath() for x in common[-1]]
    return '\n\t'+('\n\t'.join(matchedPaths))


def dumpFieldName(path, whichInfo='valueF'):
    print path.getFieldNames(whichInfo+'info')
