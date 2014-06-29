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


"""moose_config.py: 

    Configuration variables for Moose.

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

import moose

cellPath = '/cells'
moose.Neutral(cellPath)

def cell_path(populationName, instanceId=None):
    """Create path for a given instance of a population for MOOSE """
    global cellPath
    if not instanceId:
        moose.Neutral('{}/{}'.format(cellPath, populationName))
        return '{0}/{1}/0'.format(cellPath, populationName)
    return '{0}/{1}/{2}'.format(cellPath, populationName, instanceId)

def moose_path(cellId, segmentId, prefix=None):
    """Return moose-path for a given cell and segment 
    
    The return path:
        prefix/population/population_instance/cell_type/segmentId
    
    """
    if not prefix:
        prefix = cellPath 
    if '../' in cellId:
        cellId = cellId.replace('../', '')
    path = '{}/{}/{}'.format(prefix, cellId, segmentId)
    return path

def moose_path_for_segment(cellPath, cellType, segmentId):
    """Return a valid Compartment path for a given segment in Moose 
    
    """
    path = '{}/{}/{}'.format(cellPath, cellType, segmentId)
    return path
