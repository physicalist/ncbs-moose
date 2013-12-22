#!/usr/bin/env python

"""moose_methods.py:  Some helper function related with moose to do multiscale
modelling.

Last modified: Mon Dec 23, 2013  01:48AM

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

nameSep = '()'

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
            raise RuntimeError, "Stupid separator. {0}. I want at most 2 char"
    else:
        return baseName + '/' + append


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
    if text:
        return 0.0
    try:
        val = float(text)
        return val
    except Exception:
        raise UserWarning, "Failed to convert {0} to float".format(text)
