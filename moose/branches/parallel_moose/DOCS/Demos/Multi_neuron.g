// moose
function make_compartment(path, RA, RM, CM, EM, inject, diameter, length)
          float RA, RM, CM, EM, inject, diameter, length
          str path
 		
	  echo {path}

          float PI = 3.141592654
          float Ra = 4.0 * {length} * {RA} / ({PI} * {diameter} * {diameter})
          float Rm = {RM} / ({PI} * {diameter} * {length})
          float Cm = {CM} * ({PI} * {diameter} * {length})
  
          create Compartment {path}
          setfield {path} \
                  Ra {Ra} \
                  Rm {Rm} \
                  Cm {Cm} \
                  Em {EM} \
                  inject {inject} \
                  diameter {diameter} \
                  length {length} \
                  initVm {EM}
end
  
function link_compartment(path1, path2)
          str path1, path2
          addmsg {path1}/raxial {path2}/axial
end

function make_neuron(path, RA, RM, CM, EM, inject, diameter, length, count)
	float RA, RM, CM, EM, inject, diameter, length, count
        str path
	int i = 1

	make_compartment {path}/c1 {RA} {RM} {CM} {EM} {inject} {diameter} {length}

	for ( i = 2; i <= {count}; i = i + 1 )
		make_compartment {path}/c{i} {RA} {RM} {CM} {EM} 0.0 {diameter} {length}
		link_compartment {path}/c{i - 1} {path}/c{i}		
	end

	create SpikeGen /channel/spkgn
	create SynChan /channel/syncn

	create Table {path}/v1
	create Table {path}/vn

	setfield {path}/v1,{path}/vn stepmode 3

	setfield /channel/syncn "tau1" 1.0e-3
	setfield /channel/syncn "tau2" 1.0e-3
	setfield /channel/syncn "Gbar" 0.9e-3
	setfield /channel/syncn "Ek"   -0.010

	setfield /channel/spkgn "threshold" -0.004
	setfield /channel/spkgn "refractT" 0.025
	setfield /channel/spkgn "amplitude" 1.0
	setfield /channel/spkgn "Vm" -0.065

	addmsg {path}/v1/inputRequest {path}/c1/Vm
	addmsg {path}/vn/inputRequest {path}/c{count}/Vm

	addmsg {path}/c{count}/VmSrc /channel/spkgn/Vm
	addmsg /channel/syncn/IkSrc {path}/c1/injectMsg

	useclock {path}/##[TYPE=Table] 5
	//useclock {path}/##[TYPE=Compartment] 4
        useclock /channel/spkgn 5
        useclock /channel/syncn 5
end


float  SIMDT           = 50e-6
float  PLOTDT          = {SIMDT} * 1.0
float  SIMLENGTH       = 0.25
int    N_COMPARTMENT   = 5
float  CABLE_LENGTH    = 1e-3
float  RA              = 1.0
float  RM              = 4.0
float  CM              = 0.01
float  EM              = -0.065
float  INJECT          = 1e-10
float  DIAMETER        = 1e-6
float  LENGTH          = {CABLE_LENGTH} / {N_COMPARTMENT}
float  N_NEURON        = 3
int i

setrank 0

setclock 4 {SIMDT} 0
setclock 5 {PLOTDT} 0

create Neutral /channel

for ( i = 1; i <= {N_NEURON}; i = i + 1 )
	setrank {i}
	create Neutral /neuron{i}
	make_neuron /neuron{i} {RA} {RM} {CM} {EM} {INJECT} {DIAMETER} {LENGTH} {N_COMPARTMENT}
end

setrank 0

planarconnect /channel/spkgn /channel/syncn

//echo "Rallpack 1 model set up."

reset

planardelay /channel/syncn 0.05
planarweight /channel/syncn 1

step {SIMLENGTH} -t

for ( i = 1; i <= {N_NEURON}; i = i + 1 )
	setrank {i}
	setfield /neuron{i}/v1 print "sim_cable.0"{i}
	setfield /neuron{i}/vn print "sim_cable.x"{i}
end
setrank 0

//echo "Plots written to 'sim_cable.*'"
quit


