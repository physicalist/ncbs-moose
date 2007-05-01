set title 'Rallpack 2 (Branched cable)'
set xlabel 'Time (s)'
set ylabel 'Vm (V)'

plot \
	'rall.2.plot' with line title 'Compt 0 (MOOSE)', \
	'ref_branch.0' with line title 'Compt 0 (GENESIS)', \
	'rall.2.plot' using 1:3 with line title 'Compt x (MOOSE)', \
	'ref_branch.x' with line title 'Compt x (GENESIS)'

pause 5
