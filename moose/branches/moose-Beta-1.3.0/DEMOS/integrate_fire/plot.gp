set title 'Integrate and Fire Neuron'
set xlabel 'Time (s)'
set ylabel 'Membrane Potential (V)'
plot 'vm.plot' w l
pause mouse key "Any key to continue.\n"
set term png
set output 'Vm.png'
replot
set output
set term x11
print "Plot image written to Vm.png.\n"

