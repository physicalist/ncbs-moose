//moose
// This is a 2-node simulation to test messaging.

// Set up the instantaneous scheduling first of all.
float SIMDT = 1
float CONTROLDT = 10
float PLOTDT = 10
setclock 0 {SIMDT}	0
setclock 1 {SIMDT}	1
setclock 2 { CONTROLDT }
setclock 3 { PLOTDT }
setclock 4 {SIMDT} 2

useclock /kinetics/##[TYPE=Molecule],/kinetics/##[TYPE=Table],/##[TYPE=Compartment] 0
useclock /kinetics/##[TYPE=Reaction],/kinetics/##[TYPE=Enzyme],/##[TYPE=ConcChan] 1
useclock /graphs/##[TYPE=Plot],/moregraphs/##[TYPE=Plot] 3
reset
/*
addmsg /sched/cj/clock /sched/cj/ct0/clock
addmsg /sched/cj/clock /sched/cj/ct1/clock
addmsg /sched/cj/clock /sched/cj/ct2/clock
addmsg /sched/cj/clock /sched/cj/ct3/clock
addmsg /sched/cj/clock /sched/cj/ct4/clock
*/

create Neutral /kinetics
create Neutral /node1/kinetics
create Molecule /kinetics/m
setfield /kinetics/m nInit 1234
setfield /kinetics/m n 4231.0987

create Molecule /node1/kinetics/n
create Neutral /node1/cell
create ConcChan /node1/kinetics/cc

/*
create Compartment /cell
create CaConc /cell/ca1
setfield /cell/Ca1 Ca_base 9.8765
setfield /cell/Ca1 Ca 4.3210
create CaConc /cell/ca2
create CaConc /cell/ca3

create HHChannel /node1/cell/KA1
create HHChannel /node1/cell/KA2
create HHChannel /node1/cell/KA3
*/

// barrier

// currently we can only send non-shared messages 
// addmsg /kinetics/m/nOut /node1/kinetics/n/sumTotalIn
addmsg /kinetics/m/nOut /node1/kinetics/cc/nIn
// addmsg /cell/ca1/concOut /node1/cell/KA1/concenIn
// addmsg /cell/ca2/concOut /node1/cell/KA2/concenIn
// addmsg /cell/ca3/concOut /node1/cell/KA3/concenIn

// reset

step 1 -t
// call /p processIn
echo showfield /kinetics/m n 
showfield /kinetics/m n 
echo showfield /node1/kinetics/cc n 
showfield /node1/kinetics/cc n 
