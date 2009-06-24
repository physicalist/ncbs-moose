// moose
// genesis


echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Model: 2 linear cells, with a synapse. 1st cell is excitable, 2nd is not.
Plots: - Vm of last compartment of postsynaptic cell
       - Gk of synaptic channel (embedded in first compartment of the same cell)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"

include synapse/compatibility.g
int USE_SOLVER = 1


////////////////////////////////////////////////////////////////////////////////
// MODEL CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////
float SIMDT = 50e-6
float IODT = 50e-6
float SIMLENGTH = 0.05
float INJECT = 1.0e-10
float EREST_ACT = -0.065

include synapse/simplechan.g

ce /library
	make_Na_mit_usb
	make_K_mit_usb
ce /

//=====================================
//  Create cells
//=====================================
readcell synapse/myelin2.p /axon
readcell synapse/cable.p /cable

//=====================================
//  Create synapse
//=====================================
ce /axon/n99/i20
create spikegen spike
setfield spike thresh 0.0 \
               abs_refract .02
addmsg . spike INPUT Vm
ce /

ce /cable/c1
create synchan syn
setfield ^ Ek 0 gmax 1e-8 tau1 1e-3 tau2 2e-3
addmsg . syn VOLTAGE Vm
addmsg syn . CHANNEL Gk Ek
ce /

addmsg /axon/n99/i20/spike /cable/c1/syn SPIKE


////////////////////////////////////////////////////////////////////////////////
// PLOTTING
////////////////////////////////////////////////////////////////////////////////
create neutral /data

create table /data/Vm
call /data/Vm TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/Vm step_mode 3

create table /data/SynGk
call /data/SynGk TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/SynGk step_mode 3
/*
create table /data/NaGk
call /data/NaGk TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/NaGk step_mode 3

create table /data/X
call /data/X TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/X step_mode 3

create table /data/Y
call /data/Y TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/Y step_mode 3
*/
//=====================================
//  Record from compartment
//=====================================
addmsg /cable/c10 /data/Vm INPUT Vm
addmsg /cable/c1/syn /data/SynGk INPUT Gk
/*
addmsg /axon/n20/Na_mit_usb /data/NaGk INPUT Gk
addmsg /axon/n20/Na_mit_usb /data/X INPUT X
addmsg /axon/n20/Na_mit_usb /data/Y INPUT Y
*/

////////////////////////////////////////////////////////////////////////////////
// SIMULATION CONTROL
////////////////////////////////////////////////////////////////////////////////

//=====================================
//  Clocks
//=====================================
setclock 0 {SIMDT}
setclock 1 {SIMDT}
setclock 2 {IODT}

useclock /data/#[TYPE=table] 2

//=====================================
//  Stimulus
//=====================================
setfield /axon/soma inject {INJECT}

//=====================================
//  Solvers
//=====================================
// In Genesis, an hsolve object needs to be created.
//
// In Moose, hsolve is enabled by default. If USE_SOLVER is 0, we disable it by
// switching to the Exponential Euler method.

if ( USE_SOLVER )
	if ( GENESIS )
		create hsolve /axon/solve
		setfield /axon/solve \
			path /axon/##[TYPE=symcompartment],/axon/##[TYPE=compartment] \
			chanmode 1
		call /axon/solve SETUP
		setmethod 11
		
		create hsolve /cable/solve
		setfield /cable/solve \
			path /cable/##[TYPE=symcompartment],/cable/##[TYPE=compartment] \
			chanmode 1
		call /cable/solve SETUP
		setmethod 11
	end
else
	if ( MOOSE )
		setfield /cable method "ee"
		setfield /axon method "ee"
	end
end

//=====================================
//  Simulation
//=====================================
reset
step {SIMLENGTH} -time


////////////////////////////////////////////////////////////////////////////////
//  Write Plots
////////////////////////////////////////////////////////////////////////////////
str filename
str extension
if ( MOOSE )
	extension = ".moose.plot"
else
	extension = ".genesis.plot"
end

// filename = "Vm" @ {extension}
filename = "test.plot"

openfile {filename} w
writefile {filename} "/newplot"
writefile {filename} "/plotname Vm"
closefile {filename}
tab2file {filename} /data/Vm table

openfile {filename} a
writefile {filename} " "
writefile {filename} "/newplot"
writefile {filename} "/plotname SynGk"
closefile {filename}
tab2file {filename} /data/SynGk table
/*
openfile {filename} a
writefile {filename} " "
writefile {filename} "/newplot"
writefile {filename} "/plotname NaGk"
closefile {filename}
tab2file {filename} /data/NaGk table

openfile {filename} a
writefile {filename} " "
writefile {filename} "/newplot"
writefile {filename} "/plotname X"
closefile {filename}
tab2file {filename} /data/X table

openfile {filename} a
writefile {filename} " "
writefile {filename} "/newplot"
writefile {filename} "/plotname Y"
closefile {filename}
tab2file {filename} /data/Y table
*/

echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Plots written to *.moose.plot. Reference curves from GENESIS are in files named
*.genesis.plot.

If you have gnuplot, run 'gnuplot plot.gnuplot' to view the graphs.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"
quit
