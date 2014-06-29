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


"""moose_compartment.py: 

    Support for moose compartment.

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

class MooseCompartment():

    def __init__(self):
        pass


    def setPath(self, **kwargs):
        pass

    @staticmethod
    def segmentToMoosePath(cellPath, component, segmentId):
        """Convert a path of given segment to valid moose compartment """
        return "{}/{}/{}".format(cellPath, component, segmentId)

    
    @classmethod
    def segmentToCompartment(cls, cellPath, component, segment):
        """Create a Moose compartment for a given segment """
        if not segment.id:
            segment.id = '0'
        
        compartmentPath = cls.segmentToMoosePath(cellPath
                , component
                , segment.id
                )
        comp = moose.Compartment(compartmentPath)
        if segment.parent:
            parentSegment = segment.parent.segments
            parentPath = cls.segmentToMoosePath(cellPath
                    , component
                    , parentSegment
                    )
            parentComp = moose.Compartment(parentPath)
            parentComp.connect('axial', comp, 'raxial')

