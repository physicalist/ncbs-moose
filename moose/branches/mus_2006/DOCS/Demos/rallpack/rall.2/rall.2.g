// moose
include compartment.g
setfield /sli_shell isInteractive 0

float  DT              = 100e-6
float  SIMLENGTH       = 0.25
int    MAX_DEPTH       = 10
float  RA              = 1.0
float  RM              = 4.0
float  CM              = 0.01
float  EM              = -0.065
float  INJECT          = 0.1e-9
float  DIAMETER_0      = 16e-6
float  LENGTH_0        = 32e-6

create Neutral /cell

float diameter = {DIAMETER_0}
float length   = {LENGTH_0}
int   label    = 1
make_compartment /cell/c{label} \
	{RA} {RM} {CM} {EM} {INJECT} {diameter} {length}

int i, j
for ( i = 2; i <= MAX_DEPTH; i = i + 1 )
	diameter = {diameter / 2.0 ** (2.0 / 3.0)}
	length   = {length   / 2.0 ** (1.0 / 3.0)}
	
	for ( j = 1; j <= 2 ** (i - 1); j = j + 1 )
		label = label + 1
		make_compartment /cell/c{label} \
			{RA} {RM} {CM} {EM} 0.0 {diameter} {length}
		link_compartment /cell/c{label / 2} /cell/c{label}
	end
end

setfield /sli_shell isInteractive 1
echo "Rallpack 2 model set up."
setfield /sli_shell isInteractive 0

create HSolve1 /solve
/* Unlike Genesis, where the solver is informed of all compartments, here
 * any single "seed" compartment from the tree suffices.
 */
setfield ^ path /cell/c1

setclock 0 {DT} 0
setclock 1 {DT} 1
useclock /cell/##[TYPE=Compartment] 0
useclock /solve 1
reset

/* For now, the "reset" command and the "Plot" object do not mesh well with the
 * solver. In their place, following messages are invoked:
 *	- reinitializeIn: Takes 'dt' as argument.
 *	- recordCompartmentIn: Takes path to compartment, for which membrane
 *		potential needs to be recorded.
 *	- printVoltageTracein: Plots membrane potential time series for tagged
 *		compartments in a single file. Takes file name, and write-mode
 *		(0: append, 1: truncate).
 */
call /solve reinitializeIn {DT}
call /solve recordCompartmentIn /cell/c1
call /solve recordCompartmentIn /cell/c{2 ** MAX_DEPTH - 1}

step {SIMLENGTH} -t
call /solve printVoltageTraceIn "rall.2.plot" 1
setfield /sli_shell isInteractive 1
echo "Plot written to 'rall.2.plot'"
quit
