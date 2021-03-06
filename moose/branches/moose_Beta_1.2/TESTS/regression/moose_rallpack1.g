// moose
// genesis


echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Rallpack 1: Linear cable
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"

include rallpack1/compatibility.g
include rallpack1/util.g
int USE_SOLVER = 1

////////////////////////////////////////////////////////////////////////////////
// MODEL CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////
float  SIMDT           = 50e-6
float  IODT            = {SIMDT} * 1.0
float  SIMLENGTH       = 0.25
int    N_COMPARTMENT   = 1000
float  CABLE_LENGTH    = 1e-3
float  RA              = 1.0
float  RM              = 4.0
float  CM              = 0.01
float  EM              = -0.065
float  INJECT          = 1e-10
float  DIAMETER        = 1e-6
float  LENGTH          = {CABLE_LENGTH} / {N_COMPARTMENT}
int    SYMMETRIC       = 1


//=====================================
//  Create cells
//=====================================
if ( MOOSE )
	create Cell /cable
else
	create neutral /cable
end

make_compartment /cable/c1 {RA} {RM} {CM} {EM} 0.0 {DIAMETER} {LENGTH} {SYMMETRIC}

int i
for ( i = 2; i <= {N_COMPARTMENT}; i = i + 1 )
	make_compartment /cable/c{i} {RA} {RM} {CM} {EM} 0.0 {DIAMETER} {LENGTH} {SYMMETRIC}
	link_compartment /cable/c{i - 1} /cable/c{i} {SYMMETRIC}
end

echo "Rallpack 1 model set up."


////////////////////////////////////////////////////////////////////////////////
// PLOTTING
////////////////////////////////////////////////////////////////////////////////
create neutral /data

create table /data/v1
call /data/v1 TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/v1 step_mode 3
addmsg /cable/c1 /data/v1 INPUT Vm

create table /data/vn
call /data/vn TABCREATE {SIMLENGTH / IODT} 0 {SIMLENGTH}
setfield /data/vn step_mode 3
addmsg /cable/c{N_COMPARTMENT} /data/vn INPUT Vm


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
setfield /cable/c1 inject {INJECT}

//=====================================
//  Solvers
//=====================================
// In Genesis, an hsolve object needs to be created.
//
// In Moose, hsolve is enabled by default. If USE_SOLVER is 0, we disable it by
// switching to the Exponential Euler method.

if ( USE_SOLVER )
	if ( GENESIS )
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

filename = "test.plot"
// Clear file contents
openfile {filename} w
closefile {filename}

//filename = "cable-0" @ {extension}
openfile {filename} a
writefile {filename} "/newplot"
writefile {filename} "/plotname Vm(0)"
closefile {filename}
tab2file {filename} /data/v1 table

//filename = "cable-x" @ {extension}
openfile {filename} a
writefile {filename} " "
writefile {filename} "/newplot"
writefile {filename} "/plotname Vm(x)"
closefile {filename}
tab2file {filename} /data/vn table

echo "
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Reference curves (analytical and from GENESIS) are in files named
*.analytical.plot and *.genesis.plot.

If you have gnuplot, run 'gnuplot plot.gnuplot' to view the graphs.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"
quit
