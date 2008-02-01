
create Compartment pre
create Compartment post
create SpikeGen spgen
create StochSynchan schan
addmsg pre/Vm spgen/Vm
addmsg post/channel schan/channel
addmsg spgen/event schan/synapse
reset // need to reset so that the StochSynchan updates number of synapses to 1
setfield pre  Em -0.07 Rm 1e9 Cm 1e-9 Ra 1e7 inject 0.1e-6
setfield post Em -0.07 Rm 1e9 Cm 1e-9 Ra 1e7
setfield spgen threshold -0.1 abs_refract 0.001 amplitude 0.040 state 0.0
setfield schan weight[0] 1.0 delay[0] 0.001 releaseP[0] 0.5 tau1 0.0001 tau2 0.0001 Gbar 1e2

create Table postVm
create Table spike 
create Table chan
addmsg postVm/inputRequest post/Vm
addmsg spike/inputRequest spgen/state
addmsg chan/inputRequest schan/Gk

setfield postVm stepmode 3
setfield spike stepmode 3
setfield chan stepmode 3

setclock 0 1e-5 0
useclock pre,post,spgen,schan,/##[TYPE=Table] 0


reset
int i
for ( i = 0; i < 10000; i = i + 1 ) 
        step 10
        showfield spgen state
        showfield schan Gk
        showfield post Vm 
end
setfield postVm print "postvm.txt"
setfield spike print "spike.txt"
setfield chan print "channel.txt"
quit
