set title 'Rallpack 1 (Linear cable)'
set xlabel 'Time (s)'
set ylabel 'Vm (V)'

plot \
	'rall.1.plot' with line title 'Compt 0 (MOOSE)', \
	'ref_cable.0' with line title 'Compt 0 (GENESIS)', \
	'rall.1.plot' using 1:3 with line title 'Compt x (MOOSE)', \
	'ref_cable.x' with line title 'Compt x (GENESIS)'

pause 5
