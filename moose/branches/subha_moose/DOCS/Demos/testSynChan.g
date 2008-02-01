create Compartment preSynaptic
setfield preSynaptic initVm -0.07 Rm 1e7 Cm 1e-6 Ra 1e6 inject 0.1e-4
create Compartment postSynaptic
setfield postSynaptic initVm -0.07 Rm 1e7 Cm 1e-6 Ra 1e6

create SpikeGen spikeGen
setfield spikeGen threshold -0.1 refractT 0.005 amplitude 1.0 state 0.0

create SynChan synChan
addmsg postSynaptic/channel synChan/channel
addmsg spikeGen/event synChan/synapse
reset
setfield synChan weight[0] 1.0 delay[0] 0.001 tau1 0.0001 tau2 0.0001 Gbar 1.0

create Table spikeTable
create Table channelTable
create Table vmTable

setfield spikeTable stepmode 3
setfield channelTable stepmode 3
setfield vmTable stepmode 3

addmsg preSynaptic/Vm spikeGen/Vm

addmsg spikeTable/inputRequest spikeGen/state
addmsg channelTable/inputRequest synChan/Gk
addmsg vmTable/inputRequest postSynaptic/Vm

setclock 0 1e-5 0
setclock 1 1e-4 0

useclock preSynaptic,postSynaptic,spikeGen,synChan,/##[TYPE=Table] 0
//useclock spikeTable,channelTable,vmTable 1

reset

int i = 0
for (i = 0; i < 1000000; i = i+5 )
        step 5
        showfield spikeGen state
        showfield postSynaptic Vm
        showfield synChan Gk
        showfield synChan Ik
end

step 0.01 -t
setfield spikeTable print "spike.plot"
setfield channelTable print "channel.plot"
setfield vmTable print "voltage.plot"
echo "Printed data into: spike.plot, channel.plot, voltage.plot"
quit
