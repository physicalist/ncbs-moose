import moose
from math import pi

def make_compartment( path, RA, RM, CM, EM, inject, diameter, length ):
	c = moose.NeutralArray( type = 'Compartment', path = path )[ 0 ]
	c.Ra = 4 * length * RA / ( pi * diameter ** 2 )
	c.Rm = RM / ( pi * diameter * length )
	c.Cm = CM * ( pi * diameter * length )
	c.Em = EM
	c.Vm = EM
	c.initVm = EM
	
	return c
