set title 'Rallpack 3 (Axon with HH Channels)'
set xlabel 'Time (s)'
set ylabel 'Vm (V)'
set xrange [0:0.25]

plot \
	'rall.3.plot' with line title 'Compt 0 (MOOSE)', \
	'ref_axon.0.genesis' with line title 'Compt 0 (GENESIS)', \
	'rall.3.plot' using 1:3 with line title 'Compt x (MOOSE)', \
	'ref_axon.x.genesis' with line title 'Compt x (GENESIS)'

pause 5
