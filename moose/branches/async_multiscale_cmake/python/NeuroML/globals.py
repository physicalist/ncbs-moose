# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with MOOSE.  If not, see <http://www.gnu.org/licenses/>.

"""global.py: 

    Keep all of your global variables here.

Last modified: Sat Jan 18, 2014  05:01PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import re
import inspect
import sys
from . import print_utils

design_dir = "."

valuePat = re.compile(r'(?P<value>[\d\.eE-]+)(?P<unit>\D+)$')

def toSIValue(value):
    """Convert a given value string to SI value"""
    m = valuePat.match(value)
    assert m, "Could not match the given value {}".format(value)
    unit = m.group('unit')
    if unit in [ "ms", "mV", "mI", "mS"]:
        return float(m.group('value')) * 1e-3
    else:
        print_utils.dump("FATAL"
                , "Don't know what to do with unit %s " % unit
                , frame = inspect.currentframe()
                )
        raise
                
