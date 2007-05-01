// moose
include compartment.g
setfield /sli_shell isInteractive 0

float  DT              = 100e-6
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

create Neutral /cable
make_compartment /cable/c1 {RA} {RM} {CM} {EM} {INJECT} {DIAMETER} {LENGTH}

int i
for ( i = 2; i <= {N_COMPARTMENT}; i = i + 1 )
	make_compartment /cable/c{i} {RA} {RM} {CM} {EM} 0.0 {DIAMETER} {LENGTH}
	link_compartment /cable/c{i - 1} /cable/c{i}
end

setfield /sli_shell isInteractive 1
echo "Rallpack 1 model set up."
setfield /sli_shell isInteractive 0

create HSolve1 solve
/* Unlike Genesis, where the solver is informed of all compartments, here
 * any single "seed" compartment from the tree suffices.
 */
setfield ^ path /cable/c1

setclock 0 {DT} 0
setclock 1 {DT} 1
useclock /cable/##[TYPE=Compartment] 0
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
call /solve recordCompartmentIn /cable/c1
call /solve recordCompartmentIn /cable/c{N_COMPARTMENT}

step {SIMLENGTH} -t
call /solve printVoltageTraceIn "rall.1.plot" 1
setfield /sli_shell isInteractive 1
echo "Plot written to 'rall.1.plot'"
quit
