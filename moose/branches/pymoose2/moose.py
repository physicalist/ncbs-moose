# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Wed Mar 16 17:59:22 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 44
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
        return _moose._pymoose_Neutral_getattr(self.__ptr, 'class', 'string')
    className = property(__get_class)
        
# 
# moose.py ends here
