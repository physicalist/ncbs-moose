# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Wed Mar 30 12:09:38 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 149
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 

# Code:

import _moose

from _moose import copy, move, useClock, start, stop, reinit, isRunning, loadModel, getCwe, setCwe, Element

class Neutral(_moose.Neutral):
    def __init__(self, *args, **kwargs):
        _moose.Neutral.__init__(self, *args, **kwargs)

    def getFieldNames(self, ftype=''):
        return self[0].getFieldNames(ftype)
    
    path = property(lambda self: self.getPath())
    id = property(lambda self: self.getId())
    fieldNames = property(lambda self: self[0].getFieldNames('valueFinfo'))
    className = property(lambda self: self[0].getField('class'))
    name = property(lambda self: self[0].getField('name'))

class IntFire(Neutral):
    def __init__(self, *args, **kwargs):
        kwargs['type'] = 'IntFire'
        Neutral.__init__(self, *args, **kwargs)
    

class Element(_moose.Element):
    def __init__(self, *args, **kwargs):
        _moose.Element.__init__(self, *args, **kwargs)
        
    className = property(lambda self: self.getField('class'))
    fieldNames = property(lambda self: self.getFieldNames())
    
# 
# moose.py ends here
