// create Compartment c1
// create StochBouton c1/b
// setfield c1/b poolSize 10.0
// create BinomialRng c1/b/rng
// setfield c1/b/rng n 10.0
// setfield c1/b/rng p 0.4
// addmsg c1/b/rng/output c1/b/releaseCountDest
// addmsg c1/b/poolSizeSrc c1/b/rng/nDest
// addmsg c1/channel c1/b/channel

// create Compartment c2
// create SynReceptor c2/r
// setfield c2/r scale 0.2
// addmsg c2/r/injectMsg c2/injectMsg

// addmsg c1/b/releaseCount c2/r/releaseCount
// echo "Done with setting up Stochastic Synapse"

float RM = 424.4e3
float RA = 7639.44e3
float EREST = -0.07
float VLEAK = EREST + 0.010613
float CM = 0.007854e-6
float INJECT = 0.1e-6

create Compartment c1
setfield /c1 Rm {RM}
setfield /c1 Ra {RA}
setfield /c1 Cm {CM}
setfield /c1 Em {VLEAK}

create Compartment c2

create SpikeGen c1/spike
create BinSynchan c2/syn           

setfield c1/spike threshold 0 refractT 0.010 abs_refract 0 amplitude 0.0 state 0.99
setfield c1 Vm 0.40
//setfield c2
addmsg c1/Vm c1/spike/Vm
addmsg c2/channel c2/syn/channel
addmsg c1/spike/event c2/syn/synapse

create Table Vm
setfield Vm stepmode 3
addmsg Vm/inputRequest c2/Vm
create Table vSyn
setfield vSyn stepmode 3
addmsg vSyn/inputRequest c2/syn/releaseCount[0]

setfield /c1 initVm {EREST}
reset
setfield /c1 inject 0
step 0.005 -t
setfield /c1 inject {INJECT}
step 10
showfield c1/spike/Vm
showfield c1/Vm
setfield /c1 inject 0
step 0.005 -t

showfield c1/spike/Vm
showfield c1/Vm
step 10 -t
setfield Vm print "vm.plot"
setfield vSyn print "rcount.plot"
