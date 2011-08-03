import moose
from local_code import make_compartment

#~ n_levels        = 10
n_levels        = 4
RA              = 1.0
RM              = 4.0
CM              = 0.01
EM              = -0.065
inject          = 0.1e-9
diameter_0      = 16e-6
length_0        = 32e-6

#~ simlength       = 0.25
simlength       = 0.0
simdt           = 1e-6
iodt            = simdt * 1.0

label = 1
diameter = diameter_0
length = length_0
obj_map = {}
for i in range( n_levels ):
	for j in range( 2 ** i ):
		path = '/c{0}'.format( label )
		# Setting injection current to 0.0 for all compartments.
		c = make_compartment( path, RA, RM, CM, EM, 0.0, diameter, length )
		obj_map[ label ] = c
		
		if i > 0:
			parent = obj_map[ label / 2 ]
			parent.connect( 'axial', c, 'raxial' )
		
		print label
		label += 1
	
	diameter /= 2.0 ** ( 2.0 / 3.0 )
	length   /= 2.0 ** ( 1.0 / 3.0 )

root = obj_map[ 1 ]
leaf = obj_map[ 2 ** n_levels - 1 ]
print root.outMessages()

t1 = moose.NeutralArray( type = 'Table', path = '/t1' )[ 0 ]
t1.connect( 'requestData', root, 'get_Vm' )
t2 = moose.NeutralArray( type = 'Table', path = '/t2' )[ 0 ]
t2.connect( 'requestData', leaf, 'get_Vm' )

moose.setClock( 0, simdt )
moose.setClock( 1, simdt )
moose.setClock( 2, iodt )

moose.useClock( 0, '/c#', 'process' )
moose.useClock( 1, '/c#', 'init' )
moose.useClock( 2, '/t#', 'process' )

root.inject = inject

moose.reinit()
moose.start( simlength )

with open( 'c.csv', 'w' ) as f:
	for ( v1, v2 ) in zip( t1.vec, t2.vec ):
		f.write( '{0}	{1}\n'.format( v1, v2 ) )
