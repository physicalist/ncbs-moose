# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Wed Mar 23 17:35:17 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 88
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

class Neutral(object):
    def __init__(self, path, dims=[1], className='Neutral'):
        if isinstance(dims, int):
            dims = [dims]
        else:
            try:
                it = iter(dims)
            except TypeError:
                dims = [1]
        self.__ptr = _moose._pymoose_Neutral_new(className, path, dims)
        

    # def __del__(self):
    #     _moose._pymoose_Neutral_delete(self.__ptr)
    def __get_id(self):
        return _moose._pymoose_Neutral_id(self.__ptr)
    id = property(__get_id) # moose Id
    def __get_path(self):
        return _moose._pymoose_Neutral_path(self.__ptr)
    path = property(__get_path)
    def __get_class(self):
        return _moose._pymoose_Neutral_getattr(self.__ptr, 'class')
    className = property(__get_class)
    def __get_name(self):
        return _moose._pymoose_Neutral_getattr(self.__ptr, 'name')
    name = property(__get_name)
    def _getattr(self, fname, index=0):
        return _moose._pymoose_Neutral_getattr(self.__ptr, fname, index)

    def _setattr(self, fname, value, index=0):
        return _moose._pymoose_Neutral_setattr(self.__ptr, fname, value, index)

    def children(self, index=0):
        children = _moose._pymoose_Neutral_getChildren(self.__ptr, index)
        ret = []
        for child in children:
            ret.append(Neutral('/'))
            ret[-1].__ptr = child
        return ret                       

    def getFieldNames(self, ftype='valueFinfo'):
        return _moose._pymoose_Neutral_getFieldNames(self.__ptr, ftype)

class IntFire(Neutral):
    def __init__(self, path, dims=[1]):
        Neutral.__init__(self, path, dims, 'IntFire')

    def getVm(self, index=0):
        return self._getattr('Vm', index)

    def setVm(self, value, index=0):
        return self._setattr('Vm', value, index)
    
        
# 
# moose.py ends here
