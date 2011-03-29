# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Tue Mar 29 10:04:31 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 131
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
class Neutral(_moose.Neutral):
    def __init__(self, *args, **kwargs):
        _moose.Neutral.__init__(self, *args, **kwargs)

    path = property(lambda self: self.getPath())
    id = property(lambda self: self.getId())

class IntFire(Neutral):
    def __init__(self, *args, **kwargs):
        kwargs['type'] = 'IntFire'
        Neutral.__init__(self, *args, **kwargs)

    
# import _moose

# def useClock(tick, path, field):
#     _moose._pymoose_useClock(path, field, tick)

# def setClock(tick, dt):
#     _moose._pymoose_setClock(tick, dt)

# def start(runtime):
#     _moose._pymoose_start(runtime)

# def reinit():
#     _moose._pymoose_reinit()

# def stop():
#     _moose._pymoose_stop()

    
# class Neutral(object):
#     def __init__(self, path, dims=[1], className='Neutral'):
#         if isinstance(dims, int):
#             dims = [dims]
#         else:
#             try:
#                 it = iter(dims)
#             except TypeError:
#                 dims = [1]
#         self._ptr = _moose._pymoose_Neutral_new(className, path, dims)
        

#     # def __del__(self):
#     #     _moose._pymoose_Neutral_delete(self._ptr)
#     def __get_id(self):
#         return _moose._pymoose_Neutral_id(self._ptr)
#     id = property(__get_id) # moose Id
#     def __get_path(self):
#         return _moose._pymoose_Neutral_path(self._ptr)
#     path = property(__get_path)
#     def __get_class(self):
#         return _moose._pymoose_Neutral_getattr(self._ptr, 'class')
#     className = property(__get_class)
#     def __get_name(self):
#         return _moose._pymoose_Neutral_getattr(self._ptr, 'name')
#     name = property(__get_name)
#     def _getattr(self, fname, index=0):
#         return _moose._pymoose_Neutral_getattr(self._ptr, fname, index)

#     def _setattr(self, fname, value, index=0):
#         return _moose._pymoose_Neutral_setattr(self._ptr, fname, value, index)

#     def children(self, index=0):
#         children = _moose._pymoose_Neutral_getChildren(self._ptr, index)
#         ret = []
#         for child in children:
#             ret.append(Neutral('/'))
#             ret[-1]._ptr = child
#         return ret                       

#     def getFieldNames(self, ftype='valueFinfo'):
#         return _moose._pymoose_Neutral_getFieldNames(self._ptr, ftype)

#     def connect(self, srcField, dest, destField, msgType, srcIndex=0, destIndex=0, srcDataIndex=0, destDataIndex=0):
#         return _moose._pymoose_Neutral_connect(self._ptr, srcField, dest._ptr, destField, msgType, srcIndex, destIndex, srcDataIndex, destDataIndex)


# class IntFire(Neutral):
#     def __init__(self, path, dims=[1]):
#         Neutral.__init__(self, path, dims, 'IntFire')

#     def getVm(self, index=0):
#         return self._getattr('Vm', index)

#     def setVm(self, value, index=0):
#         return self._setattr('Vm', value, index)
    

# def loadModel(filepath, modelPath):
#     model = Neutral("/")
#     model._ptr = moose_loadModel(path, model)
#     return model

# def getCwe():
#     obj = Neutral("/")
#     obj._ptr = _moose._pymoose_getCwe()
#     return obj

# def setCwe(obj):
#     if (isinstance(obj, Neutral)):
#         _moose._pymoose_setCwe(obj._ptr)
#     elif isinstance(obj, str):
#         newobj = Neutral(obj)
#         _moose._pymoose_setCwe(newobj._ptr)
#     else:
#         raise TypeError('Argument must be a path or a Neutral. Got %s' % (type(obj)))
    
# 
# moose.py ends here
