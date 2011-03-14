# moose.py --- 
# 
# Filename: moose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Sat Mar 12 14:02:40 2011 (+0530)
# Version: 
# Last-Updated: Mon Mar 14 14:00:59 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 16
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
        

# 
# moose.py ends here
