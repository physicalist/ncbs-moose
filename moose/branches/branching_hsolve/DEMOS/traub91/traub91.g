// moose
// genesis


echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"

include compatibility.g


////////////////////////////////////////////////////////////////////////////////
// MODEL CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////
float SIMDT = 50e-6
float IODT = 100e-6
float SIMLENGTH = 0.10
float INJECT = 2.0e-10
float EREST_ACT = -0.060
float ENA = 0.115 + EREST_ACT // 0.055  when EREST_ACT = -0.060
float EK = -0.015 + EREST_ACT // -0.075
float ECA = 0.140 + EREST_ACT // 0.080

include traub91proto.g

ce /library
	make_Na
	make_Ca
	make_K_DR
	make_K_AHP
	make_K_C
	make_K_A
	make_Ca_conc
ce /

//=====================================
//  Create cells
//=====================================
readcell CA3.p /CA3


////////////////////////////////////////////////////////////////////////////////
// PLOTTING
////////////////////////////////////////////////////////////////////////////////
create table /plot
call /plot TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /plot step_mode 3

//=====================================
//  Record from compartment
//=====================================
addmsg /CA3/soma /plot INPUT Vm


////////////////////////////////////////////////////////////////////////////////
// SIMULATION CONTROL
////////////////////////////////////////////////////////////////////////////////

//=====================================
//  Stimulus
//=====================================
setfield /CA3/soma inject {INJECT}

//=====================================
//  Clocks
//=====================================
if ( MOOSE )
	setclock 0 {SIMDT} 0
	setclock 1 {SIMDT} 1
	setclock 2 {IODT} 0
else
	setclock 0 {SIMDT}
	setclock 1 {SIMDT}
	setclock 2 {IODT}
end

useclock /plot 2

//=====================================
//  Solvers
//=====================================
if ( GENESIS )
	create hsolve /CA3/solve
	setfield /CA3/solve \
		path /CA3/##[TYPE=symcompartment],/CA3/##[TYPE=compartment] \
		chanmode 3
	call /CA3/solve SETUP
	setmethod 11
end

//=====================================
//  Simulation
//=====================================
reset
if ( MOOSE )
	setfield /CA3/solve/integ CaAdvance 0
end
step {SIMLENGTH} -time


////////////////////////////////////////////////////////////////////////////////
//  Write Plots
////////////////////////////////////////////////////////////////////////////////
openfile "test.plot" w
writefile "test.plot" "/newplot"
writefile "test.plot" "/plotname Vm"
closefile "test.plot"
tab2file test.plot /plot table


echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Reference plot is included. Present curve is in test.plot.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"
quit
