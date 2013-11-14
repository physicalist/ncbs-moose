#!/usr/bin/env python

__author__  = "Dilawar Singh <dilawars@ncbs.res.in>"

__log__     = """

Data-types in moose

"""

class Parameter:

    """
    Parameter class.
    """
    def __init__(self) :
        self.name = ""
        self.type = ""
        self.isVariable = False
        self.inhomogenousParam = None

    def __init__(self, paramXml) :
        """
        This is nueroML element
        """
        self.name = paramXml.get('name')
        self.type = paramXml.get('type')
        self.isVariable = False
        self.inhomogenousParam = None


