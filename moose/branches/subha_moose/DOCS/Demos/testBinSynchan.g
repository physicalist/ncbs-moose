//moose
// This is an almost perfect match to the old GENESIS squid model
// output with default parameter values. It simulates, as the Squid
// demo does, a stimulus of 0.1 uA starting at time 5 msec and 
// lasting for 40 msec. There is a final 5 msec after the stimulus.
// Most of this file is setting up the parameters and the HH tables.
// Later I'll implement and extended version of the HHGate that knows
// the funny X_alpha and other terms that the GENESIS version uses.

int i

float VMIN = -0.1
float VMAX = 0.05
int NDIVS = 150
float v = VMIN
float dv = ( VMAX - VMIN ) / NDIVS
float SIMDT = 1e-5
float PLOTDT = 1e-4
float RUNTIME = 0.5
float EREST = -0.07
float VLEAK = EREST + 0.010613
float VK = EREST -0.012
float VNa = EREST + 0.115
float RM = 424.4e3
float RA = 7639.44e3
float GLEAK = 0.3e-3
float GK = 36e-3
float GNa = 120e-3
float CM = 0.007854e-6
float INJECT = 0.1e-6

GK = 0.282743e-3
GNa = 0.94248e-3

function calc_Na_m_A( v )
	float v

	if ( { abs { EREST + 0.025 - v } } < 1e-6 )
		v = v + 1e-6
	end
	
	return { ( 0.1e6 * ( EREST + 0.025 - v ) ) / ( { exp { ( EREST + 0.025 - v )/ 0.01 } } - 1.0 ) }
end

function calc_Na_m_B( v )
	float v
	return { 4.0e3 * { exp { ( EREST - v ) / 0.018 } } }
end

function calc_Na_h_A( v )
	float v
	return { 70.0 * { exp { ( EREST - v ) / 0.020 } } }
end

function calc_Na_h_B( v )
	float v
	return { 1.0e3 / ( { exp { ( 0.030 + EREST - v )/ 0.01 } } + 1.0 ) }
end

function calc_K_n_A( v )
	float v
	if ( { abs { 0.01 + EREST - v } } < 1e-6 )
		v = v + 1e-6
	end
	return { ( 1.0e4 * ( 0.01 + EREST - v ) ) / ( { exp { ( 0.01 + EREST - v )/ 0.01 } } - 1.0 ) }
end

function calc_K_n_B( v )
	float v
	return { 0.125e3 * { exp { (EREST - v ) / 0.08 } } }
end

create Compartment /squid
setfield /squid Rm {RM}
setfield /squid Ra {RA}
setfield /squid Cm {CM}
setfield /squid Em {VLEAK}

create HHChannel /squid/Na
setfield /squid/Na Ek {VNa}
setfield /squid/Na Gbar {GNa}
setfield /squid/Na Xpower 3
setfield /squid/Na Ypower 1

create HHChannel /squid/K
setfield /squid/K Ek {VK}
setfield /squid/K Gbar {GK}
setfield /squid/K Xpower 4

addmsg /squid/channel /squid/Na/channel
addmsg /squid/channel /squid/K/channel

create Table /Vm
setfield /Vm stepmode 3
addmsg /Vm/inputRequest /squid/Vm

// This variant should also work for backward compatibility
// addmsg /squid/Vm /Vm/inputRequest



setfield /squid/Na/xGate/A xmin {VMIN}
setfield /squid/Na/xGate/B xmin {VMIN}
setfield /squid/Na/yGate/A xmin {VMIN}
setfield /squid/Na/yGate/B xmin {VMIN}
setfield /squid/K/xGate/A xmin {VMIN}
setfield /squid/K/xGate/B xmin {VMIN}
setfield /squid/Na/xGate/A xmax {VMAX}
setfield /squid/Na/xGate/B xmax {VMAX}
setfield /squid/Na/yGate/A xmax {VMAX}
setfield /squid/Na/yGate/B xmax {VMAX}
setfield /squid/K/xGate/A xmax {VMAX}
setfield /squid/K/xGate/B xmax {VMAX}
setfield /squid/Na/xGate/A xdivs {NDIVS}
setfield /squid/Na/xGate/B xdivs {NDIVS}
setfield /squid/Na/yGate/A xdivs {NDIVS}
setfield /squid/Na/yGate/B xdivs {NDIVS}
setfield /squid/K/xGate/A xdivs {NDIVS}
setfield /squid/K/xGate/B xdivs {NDIVS}

v = VMIN
for ( i = 0 ; i <= NDIVS; i = i + 1 )
	setfield /squid/Na/xGate/A table[{i}] { calc_Na_m_A { v } }
	setfield /squid/Na/xGate/B table[{i}] { { calc_Na_m_A { v } } + { calc_Na_m_B { v } } }
	setfield /squid/Na/yGate/A table[{i}] { calc_Na_h_A { v } }
	setfield /squid/Na/yGate/B table[{i}] { { calc_Na_h_A { v } } +  { calc_Na_h_B { v } } }
	setfield /squid/K/xGate/A table[{i}] { calc_K_n_A { v } }
	setfield /squid/K/xGate/B table[{i}] { { calc_K_n_A { v } } + { calc_K_n_B { v } } }
//	echo {v} { calc_K_n_B { v } }
	v = v + dv
//	echo {v}
end


create SpikeGen /squid/gen
setfield /squid/gen threshold 0.0

create Table /Spike
setfield /Spike stepmode 3
addmsg /Spike/inputRequest /squid/gen/state


create Compartment /dendrite
setfield /dendrite Rm {RM}
setfield /dendrite Ra {RA}
setfield /dendrite Cm {CM}
setfield /dendrite Em {VLEAK}

create HHChannel /dendrite/Na
setfield /dendrite/Na Ek {VNa}
setfield /dendrite/Na Gbar {GNa}
setfield /dendrite/Na Xpower 3
setfield /dendrite/Na Ypower 1

create HHChannel /dendrite/K
setfield /dendrite/K Ek {VK}
setfield /dendrite/K Gbar {GK}
setfield /dendrite/K Xpower 4

addmsg /dendrite/channel /dendrite/Na/channel
addmsg /dendrite/channel /dendrite/K/channel

create BinSynchan /dendrite/syn
addmsg /dendrite/channel /dendrite/syn/channel
addmsg /squid/gen/event /dendrite/syn/synapse
reset
setfield /dendrite/syn poolSize[0] 10
setfield /dendrite/syn releaseP[0] 0.8
setfield /dendrite/syn weight[0] 1.0
setfield /dendrite/syn delay[0] 0.1

create Table /DenVm
setfield /DenVm stepmode 3
addmsg /DenVm/inputRequest /dendrite/Vm

create Table /RCount
setfield /RCount stepmode 3
addmsg /RCount/inputRequest /dendrite/syn/releaseCount[0]

// This variant should also work for backward compatibility
// addmsg /dendrite/Vm /Vm/inputRequest



setfield /dendrite/Na/xGate/A xmin {VMIN}
setfield /dendrite/Na/xGate/B xmin {VMIN}
setfield /dendrite/Na/yGate/A xmin {VMIN}
setfield /dendrite/Na/yGate/B xmin {VMIN}
setfield /dendrite/K/xGate/A xmin {VMIN}
setfield /dendrite/K/xGate/B xmin {VMIN}
setfield /dendrite/Na/xGate/A xmax {VMAX}
setfield /dendrite/Na/xGate/B xmax {VMAX}
setfield /dendrite/Na/yGate/A xmax {VMAX}
setfield /dendrite/Na/yGate/B xmax {VMAX}
setfield /dendrite/K/xGate/A xmax {VMAX}
setfield /dendrite/K/xGate/B xmax {VMAX}
setfield /dendrite/Na/xGate/A xdivs {NDIVS}
setfield /dendrite/Na/xGate/B xdivs {NDIVS}
setfield /dendrite/Na/yGate/A xdivs {NDIVS}
setfield /dendrite/Na/yGate/B xdivs {NDIVS}
setfield /dendrite/K/xGate/A xdivs {NDIVS}
setfield /dendrite/K/xGate/B xdivs {NDIVS}

v = VMIN
for ( i = 0 ; i <= NDIVS; i = i + 1 )
	setfield /dendrite/Na/xGate/A table[{i}] { calc_Na_m_A { v } }
	setfield /dendrite/Na/xGate/B table[{i}] { { calc_Na_m_A { v } } + { calc_Na_m_B { v } } }
	setfield /dendrite/Na/yGate/A table[{i}] { calc_Na_h_A { v } }
	setfield /dendrite/Na/yGate/B table[{i}] { { calc_Na_h_A { v } } +  { calc_Na_h_B { v } } }
	setfield /dendrite/K/xGate/A table[{i}] { calc_K_n_A { v } }
	setfield /dendrite/K/xGate/B table[{i}] { { calc_K_n_A { v } } + { calc_K_n_B { v } } }
//	echo {v} { calc_K_n_B { v } }
	v = v + dv
//	echo {v}
end

setclock 0 {SIMDT} 0
setclock 1 {PLOTDT} 0

useclock /Vm,/squid,/squid/#,/DenVm,/RCount,/dendrite,/dendrite/# 0
useclock /##[TYPE=Table] 1
// useclock /Vm 0

// Crazy hack, but the squid demo does it and we need to match.
setfield /squid initVm {EREST}
reset
setfield /squid inject 0
step 0.005 -t
setfield /squid inject {INJECT}
step 0.040 -t
setfield /squid inject 0
step 0.005 -t
setfield /Vm print "squid.plot"
setfield /DenVm print "dendrite.plot"
setfield /RCount print "release.plot"
setfield /Spike print "spike.plot"
