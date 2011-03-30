# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Wed Mar 30 16:24:09 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 197
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
from _moose import useClock, setClock, start, reinit, stop, isRunning, loadModel


class Neutral(object):
    def __init__(self, *args, **kwargs):
        kwargs['type'] = 'Neutral'
        self._id = _moose.Id(*args, **kwargs)

    def getFieldNames(self, ftype=''):
        return self._id[0].getFieldNames(ftype)

    def getFieldType(self, field):
        return self._id[0].getFieldType(field)

    def __getitem__(self, index):
        objid = self._id.getItem(index)
        ret = Element(0, 0, 0)
        ret._objid = objid
        return ret

    def __len__(self):
        return len(self._id)    
    
    path = property(lambda self: self._id.getPath())
    id = property(lambda self: self._id)
    fieldNames = property(lambda self: self._id[0].getFieldNames('valueFinfo'))
    className = property(lambda self: self._id[0].getField('class'))
    name = property(lambda self: self._id[0].getField('name'))
    shape = property(lambda self: self._id.getShape())
    

class IntFire(Neutral):
    def __init__(self, *args, **kwargs):
        kwargs['type'] = 'IntFire'
        Neutral.__init__(self, *args, **kwargs)
    

class Element(object):
    def __init__(self, *args, **kwargs):
        self._objid = _moose.ObjId(*args, **kwargs)
        
    className = property(lambda self: self._objid.getField('class'))
    fieldNames = property(lambda self: self._objid.getFieldNames())


def copy(src, dest, name, n=1, copyMsg=True):
    if isinstance(src, Neutral):
        src = src._id
    if isinstance(dest, Neutral):
        dest = dest._id
    new_id = _moose.copy(src=src, dest=dest, name=name, n=n, copyMsg=copyMsg)
    ret = Neutral('/')
    ret._id = new_id
    return ret

def move(src, dest):
    if isinstance(src, Neutral):
        src = src._id
    if isinstance(dest, Neutral):
        dest = dest._id
    _moose.move(src, dest)

def setCwe(element):
    if isinstance(element, Neutral):
        _moose.setCwe(element._id)
    elif isinstance(element, str):
        _moose.setCwe(element)
    else:
        raise TypeError('Argument must be an element or the path of a valid element.')

def getCwe():
    _id = _moose.getCwe()
    obj = Neutral('/')
    obj._id = _id
    return obj

# 
# moose.py ends here
