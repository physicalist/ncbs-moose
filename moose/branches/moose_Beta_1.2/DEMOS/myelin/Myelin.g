// moose
// genesis


echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
A linear cell with with passive compartments, punctuated with excitable 'Nodes
of Ranvier'.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"

include compatibility.g
int USE_SOLVER = 1

////////////////////////////////////////////////////////////////////////////////
// MODEL CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////
float SIMDT = 50e-6
float IODT = 100e-6
float SIMLENGTH = 0.25
float INJECT = 1e-10
float EREST_ACT = -0.065

include chan.g

ce /library
	make_Na_mit_usb
	make_K_mit_usb
ce /

//=====================================
//  Create cells
//=====================================
readcell myelin2.p /axon


////////////////////////////////////////////////////////////////////////////////
// PLOTTING
////////////////////////////////////////////////////////////////////////////////
create neutral /plots

create table /plots/Vm0
call /plots/Vm0 TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /plots/Vm0 step_mode 3
addmsg /axon/soma /plots/Vm0 INPUT Vm

create table /plots/Vm1
call /plots/Vm1 TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /plots/Vm1 step_mode 3
addmsg /axon/n99/i20 /plots/Vm1 INPUT Vm


////////////////////////////////////////////////////////////////////////////////
// SIMULATION CONTROL
////////////////////////////////////////////////////////////////////////////////

//=====================================
//  Clocks
//=====================================
setclock 0 {SIMDT}
setclock 1 {SIMDT}
setclock 2 {IODT}

useclock /plots/#[TYPE=table] 2

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
	end
else
	if ( MOOSE )
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

filename = "axon-0" @ {extension}
openfile {filename} w
writefile {filename} "/newplot"
writefile {filename} "/plotsname Vm(0)"
closefile {filename}
tab2file {filename} /plots/Vm0 table

filename = "axon-x" @ {extension}
openfile {filename} w
writefile {filename} "/newplot"
writefile {filename} "/plotsname Vm(100)"
closefile {filename}
tab2file {filename} /plots/Vm1 table

echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Plots written to *.moose.plot. Reference plots in *.genesis.plot

If you have gnuplot, run 'gnuplot plot.gnuplot' to view the graphs.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"
quit
