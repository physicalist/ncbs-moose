// moose

function make_compartment(path, RA, RM, CM, EM, inject, diameter, length)
	float RA, RM, CM, EM, inject, diameter, length
	str path
	
	float PI = 3.141592654
	float PI_D_L = {PI} * {diameter} * {length}
	float Ra = 4.0 * {length} * {RA} / {PI_D_L}
	float Rm = {RM} / {PI_D_L}
	float Cm = {CM} * {PI_D_L}
	
	create Compartment {path}
	setfield ^ Ra {Ra} \
	           Rm {Rm}	\
	           Cm {Cm}	\
	           Em {EM}	\
	           Inject {inject}	\
	           diameter {diameter}	\
	           length {length}	\
	           initVm {EM}
	
	float EK  = -0.077
	float ENa = 0.050
	float GK  = 360
	float GNa = 1200
	float Gbar_Na = {GNa} * {PI_D_L}
	float Gbar_K  = {GK}  * {PI_D_L}
	
	create ZChannel {path}/Na
	setfield {path}/Na Ek {ENa}
	setfield {path}/Na Gbar {Gbar_Na}
	
	create ZGate {path}/Na/m
	setfield ^ \
		power      3        \
		state      0.0529325  \
		alphaForm  3        \
		alpha_A    -1.0e5   \
		alpha_B    -0.010   \
		alpha_V0   -0.040   \
		betaForm   1        \
		beta_A     4.0e3    \
		beta_B     -0.018   \
		beta_V0    -0.065
	
	create ZGate {path}/Na/h
	setfield ^ \
		power      1        \
		state      0.59612067   \
		alphaForm  1        \
		alpha_A    70.0     \
		alpha_B    -0.020   \
		alpha_V0   -0.065   \
		betaForm   2        \
		beta_A     1.0e3    \
		beta_B     -0.010   \
		beta_V0    -0.035
	
	create ZChannel {path}/K
	setfield {path}/K Ek {EK}
	setfield {path}/K Gbar {Gbar_K}
		
	create ZGate {path}/K/n
	setfield ^ \
		power      4        \
		state      0.31767695   \
		alphaForm  3        \
		alpha_A    -1.0e4   \
		alpha_B    -0.010   \
		alpha_V0   -0.055   \
		betaForm   1        \
		beta_A     125.0    \
		beta_B     -0.080   \
		beta_V0    -0.065
	
	addmsg {path}/channel {path}/Na/channel
	addmsg {path}/channel {path}/K/channel

	addmsg {path}/Na/xGateOut {path}/Na/m/gateIn
	addmsg {path}/Na/yGateOut {path}/Na/h/gateIn
	addmsg {path}/K/xGateOut {path}/K/n/gateIn
end

function link_compartment(path1, path2)
	str path1, path2
	addmsg {path1}/raxial {path2}/axial
end
