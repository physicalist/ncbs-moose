// moose

function make_compartment(path, RA, RM, CM, EM, inject, diameter, length)
	float RA, RM, CM, EM, inject, diameter, length
	str path
	
	float PI = 3.141592654
	float Ra = 4.0 * {length} * {RA} / ({PI} * {diameter} * {diameter})
	float Rm = {RM} / ({PI} * {diameter} * {length})
	float Cm = {CM} * ({PI} * {diameter} * {length})
	
	create compartment {path}
	setfield ^ Ra {Ra} \
	           Rm {Rm}	\
	           Cm {Cm}	\
	           Em {EM}	\
	           Inject {inject}	\
	           diameter {diameter}	\
	           length {length}	\
	           initVm {EM}
end

function link_compartment(path1, path2)
	str path1, path2
	addmsg {path1}/raxial {path2}/axial
end
