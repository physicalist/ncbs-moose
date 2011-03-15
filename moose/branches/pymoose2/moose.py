# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Tue Mar 15 16:44:01 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 31
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
    def __init__(self, path, dims=[1]):
        if isinstance(dims, int):
            dims = [dims]
        else:
            try:
                it = iter(dims)
            except TypeError:
                dims = [1]
        self.__dont_touch_me = _moose._pymoose_Neutral_new('Neutral', path, dims)

    # def __del__(self):
    #     _moose._pymoose_Neutral_delete(self.__dont_touch_me)
    def __get_id(self):
        return _moose._pymoose_Neutral_id(self.__dont_touch_me)
    def __get_path(self):
        return _moose._pymoose_Neutral_path(self.__dont_touch_me)
    id = property(__get_id) # moose Id
    path = property(__get_path)
        
# 
# moose.py ends here
