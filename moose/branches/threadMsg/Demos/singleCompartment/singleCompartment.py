import moose

c = moose.NeutralArray( type = 'Compartment', path = '/c' )[ 0 ]
c.Ra = 1.0
c.Rm = 1.0
c.Cm = 1.0
c.Em = 0.0
c.Vm = 0.0
c.initVm = 0.0

t = moose.NeutralArray( type = 'Table', path = '/t' )[ 0 ]
t.connect( 'requestData', c, 'get_Vm' )

moose.setClock( 0, 0.01 )
moose.setClock( 1, 0.01 )
moose.setClock( 2, 0.1 )

moose.useClock( 0, '/c', 'process' )
moose.useClock( 1, '/c', 'init' )
moose.useClock( 2, '/t', 'process' )

moose.reinit()

c.inject = 0.0
moose.start( 10 )

c.inject = 1.0
moose.start( 10 )

c.inject = 0.0
moose.start( 10 )

with open( 'c.csv', 'w' ) as f:
	for i in t.vec:
		f.write( '{0}\n'.format( i ) )
