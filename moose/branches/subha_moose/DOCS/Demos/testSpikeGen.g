create Compartment compartment
setfield compartment Em -0.07 Rm 1e7 Cm 1e-6 Ra 1e6 inject 0.1e-4

create SpikeGen spikeGen
setfield spikeGen threshold -0.1 abs_refract 0.001 amplitude 0.040 state 0.0


create Table spikeTable
create Table voltageTable

addmsg compartment/Vm spikeGen/Vm
addmsg spikeTable/inputRequest spikeGen/state
addmsg voltageTable/inputRequest compartment/Vm
setfield spikeTable stepmode 3
setfield voltageTable stepmode 3
setclock 0 1e-5 0

useclock /##[TYPE=Table],voltageTable,compartment,spikeGen 0

reset 

step 10
showfield compartment Vm
showfield spikeGen state

step 30
showfield compartment Vm
showfield spikeGen state

step 60
showfield compartment Vm
showfield spikeGen state

step 100
showfield compartment Vm
showfield spikeGen state

step 500
showfield compartment Vm
showfield spikeGen state

step 500
showfield compartment Vm
showfield spikeGen state

step 500
showfield compartment Vm
showfield spikeGen state

step 500
showfield compartment Vm
showfield spikeGen state

step 0.05 -t 
setfield spikeTable print "spike.plot"
setfield voltageTable print "voltage.plot"
echo "Done with simulating SpikeGen. Output is available in spike.plot"
quit
