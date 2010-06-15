# Izhikevich.py --- 
# 
# Filename: Izhikevich.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri May 28 14:42:33 2010 (+0530)
# Version: 
# Last-Updated: Tue Jun 15 20:21:21 2010 (+0530)
#           By: Subhasis Ray
#     Update #: 684
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:

from numpy import *
from pylab import *
import moose

class IzhikevichDemo:
    """Class to setup and simulate the various kind of neuronal behaviour using Izhikevich model.
    
    Fields:
    """    
    # Paramteres for different kinds of behaviour described by Izhikevich
    # (1. IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 14, NO. 6, NOVEMBER 2003
    # and 2. IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 15, NO. 5, SEPTEMBER
    # 2004)
    # Modified and enhanced using: http://www.izhikevich.org/publications/figure1.m
    # The entries in the tuple are as follows:
    # fig. no. in paper (2), parameter a, parameter b, parameter c (reset value of v in mV), parameter d (after-spike reset value of u), injection current I (mA), initial value of Vm, duration of simulation (ms)
    # 
    # They are all in whatever unit they were in the paper. Just before use we convert them to SI.
    parameters = {
        "tonic_spiking":    ('A', 0.02  ,    0.2  ,   -65.0,     6.0  ,      14.0,      -70.0,  100.0), # Fig. 1.A
        "phasic_spiking":   ('B', 0.02  ,    0.25 ,   -65.0,     6.0  ,      0.5,       -64.0,  200.0), # Fig. 1.B
        "tonic_bursting":   ('C', 0.02  ,    0.2  ,   -50.0,     2.0  ,      15.0,      -70.0,  220.0), # Fig. 1.C
        "phasic_bursting":  ('D', 0.02  ,    0.25 ,   -55.0,     0.05 ,      0.6,       -64.0,  200.0), # Fig. 1.D
        "mixed_mode":       ('E', 0.02  ,    0.2  ,   -55.0,     4.0   ,     10.0,      -70.0,  160.0), # Fig. 1.E
        "spike_freq_adapt": ('F', 0.01  ,    0.2  ,   -65.0,     8.0   ,     30.0,      -70.0,  85.0), # Fig. 1.F # spike frequency adaptation
        "Class_1":          ('G', 0.02  ,    -0.1 ,   -55.0,     6.0   ,     0,         -60.0,  300.0), # Fig. 1.G # Spikining Frequency increases with input strength
        "Class_2":          ('H', 0.2   ,    0.26 ,   -65.0,     0.0   ,     0,         -64.0,  300.0), # Fig. 1.H # Produces high frequency spikes  
        "spike_latency":    ('I', 0.02  ,    0.2  ,   -65.0,     6.0   ,     7.0,       -70.0,  100.0), # Fig. 1.I
        "subthresh_osc":    ('J', 0.05  ,    0.26 ,   -60.0,     0.0   ,     0,         -62.0,  200.0), # Fig. 1.J # subthreshold oscillations
        "resonator":        ('K', 0.1   ,    0.26 ,   -60.0,     -1.0  ,     0,         -62.0,  400.0), # Fig. 1.K 
        "integrator":       ('L', 0.02  ,    -0.1 ,   -55.0,     6.0   ,     0,         -60.0,  100.0), # Fig. 1.L 
        "rebound_spike":    ('M', 0.03  ,    0.25 ,   -60.0,     4.0   ,     -15,       -64.0,  200.0), # Fig. 1.M 
        "rebound_burst":    ('N', 0.03  ,    0.25 ,   -52.0,     0.0   ,     -15,       -64.0,  200.0), # Fig. 1.N 
        "thresh_var":       ('O', 0.03  ,    0.25 ,   -60.0,     4.0   ,     0,         -64.0,  100.0), # Fig. 1.O # threshold variability
        "bistable":         ('P', 1.0   ,    1.5  ,   -60.0,     0.0   ,     1.24,      -61.0,  300.0), # Fig. 1.P 
        "DAP":              ('Q', 1.0   ,    0.2  ,   -60.0,     -21.0 ,     20,        -70.0,  50.0 ), # Fig. 1.Q # Depolarizing after-potential 
        "accommodation":    ('R', 0.02  ,    1.0  ,   -55.0,     4.0   ,     0,         -65.0,  400.0), # Fig. 1.R 
        "iispike":          ('S', -0.02 ,    -1.0 ,   -60.0,     8.0   ,     75.0,      -63.8,  350.0), # Fig. 1.S # inhibition-induced spiking
        "iiburst":          ('T', -0.026,    -1.0 ,   -45.0,     0.0   ,     75.0,      -63.8,  350.0)  # Fig. 1.T # inhibition-induced bursting
    }

    
    def __init__(self):
        """Initialize the object."""
        self.neurons = {}
        self.Vm_tables = {}
        self.inject_tables = {}
        self.inputs = {}
        self.context = moose.PyMooseBase.getContext()
        self.simtime = 100e-3
        self.dt = 1e-5
        self.steps = int(self.simtime/self.dt)
        self.context.setClock(0, self.dt)
        self.context.setClock(1, self.dt)
        self.context.setClock(2, self.dt)
        self.pulsegen = moose.PulseGen('pulsegen')
        self.neuron = None
        self.current = 'tonic_spiking'

    def setup(self, key):
        neuron = self._get_neuron(key)
        pulsegen = self._make_pulse_input(key)
        if pulsegen is None:
            print key, 'Not implemented.'
            
    def simulate(self, key):
        self.setup(key)
        return self.run(key)

    def run(self, key):
        try:
            Vm = self.Vm_tables[key]
        except KeyError, e:
            Vm = moose.Table(key + '_Vm')
            Vm.stepMode = 3
            Vm.connect('inputRequest', self.neurons[key], 'Vm')
            self.Vm_tables[key] = Vm
        try:
            Im = self.inject_tables[key]
        except KeyError, e:
            Im = moose.Table(key + '_inject') # May be different for non-pulsegen sources.
            Im.stepMode = 3
            Im.connect('inputRequest', self._get_neuron(key), 'Im')
            self.inject_tables[key] = Im
        self.simtime = IzhikevichDemo.parameters[key][7] * 1e-3
        self.context.reset()
        self.context.step(self.simtime)
        time = linspace(0, IzhikevichDemo.parameters[key][7], len(Vm))
        return (time, Vm, Im)


    def _get_neuron(self, key):
        try:
            neuron = self.neurons[key]
            return neuron
        except KeyError, e:
            pass
        try:
            params = IzhikevichDemo.parameters[key]
        except KeyError, e:
            print ' %s : Invalid neuron type. The valid types are:' % (key)
            for key in IzhikevichDemo.paramteres:
                print key
            raise
        neuron = moose.IzhikevichNrn(key)
        self.neuron = neuron
        neuron.a = params[1] * 1e3 # ms^-1 -> s^-1
        neuron.b = params[2] * 1e3 # ms^-1 -> s^-1
        neuron.c = params[3] * 1e-3 # mV -> V
        neuron.d = params[4]  # d is in mV/ms = V/s
        neuron.initVm = params[6] * 1e-3 # mV -> V
        neuron.Vmax = 0.03 # mV -> V
        if key is not 'accommodation':
            neuron.initU = neuron.initVm * neuron.b
        else:
            neuron.initU = -16.0 # u is in mV/ms = V/s
        self.neurons[key] = neuron
        return neuron

    def _make_pulse_input(self, key):
        """This is for creating a pulse generator for use as a current
        source for all cases except Class_1, Class_2, resonator,
        integrator, thresh_var and accommodation."""
        baseLevel = 0.0
        firstWidth = 1e6
        firstDelay = 0.0
        firstLevel = IzhikevichDemo.parameters[key][5] * 1e-9
        secondDelay = 1e6
        secondWidth = 0.0
        secondLevel = 0.0
        if key is 'tonic_spiking':
            firstDelay = 10e-3
        elif key is 'phasic_spiking':
            firstDelay = 20e-3
        elif key is 'tonic_bursting':
            firstDelay = 22e-3
        elif key is 'phasic_bursting':
            firstDelay = 20e-3
        elif key is 'mixed_mode':
            firstDelay = 16e-3
        elif key is 'spike_freq_adapt':
            firstDelay = 8.5e-3
        elif key is 'spike_latency':
            firstDelay = 10e-3
            firstWidth = 3e-3
        elif key is 'subthresh_osc':
            firstDelay = 20e-3
            firstWidth = 5e-3
        elif key is 'rebound_spike':
            firstDelay = 20e-3
            firstWidth = 5e-3
        elif key is 'rebound_burst':
            firstDelay = 20e-3
            firstWidth = 5e-3
        elif key is 'bistable':
            firstDelay = 300e-3/8
            firstWidth = 5e-3
            secondLevel = firstLevel
            secondWidth = 5e-3
            secondDelay = 216e-3 - firstDelay
            baseLevel = 0.24e-9
        elif key is 'DAP':
            firstDelay = 9e-3
            firstWidth = 2e-3
        elif key is 'iispike' or key is 'iiburst':
            baseLevel = 80e-9
            firstDelay = 50e-3
            firstWidth = 200e-3
        elif key is 'Class_1':
            input_table = self._make_Class_1_input()
            input_table.connect('outputSrc', self._get_neuron(key), 'injectDest')
            self.inputs[key] = input_table
            return input_table
        elif key is 'Class_2':
            input_table = self._make_Class_2_input()
            input_table.connect('outputSrc', self._get_neuron(key), 'injectDest')
            self.inputs[key] = input_table
            return input_table
        else:
            print key, ': Stimulus is not based on pulse generator.'
            return None
        pulsegen = self._make_pulsegen(key, 
                                      firstLevel,
                                      firstDelay,
                                      firstWidth,
                                      secondLevel,
                                      secondDelay,
                                      secondWidth, baseLevel)
        return pulsegen
                                               

    def _make_pulsegen(self, key, firstLevel, firstDelay, firstWidth=1e6, secondLevel=0, secondDelay=1e6, secondWidth=0, baseLevel=0):
        try:
            pulsegen = self.inputs[key]
        except KeyError:
            pulsegen = moose.PulseGen(self.current + '_input')
            pulsegen.firstLevel = firstLevel
            pulsegen.firstDelay = firstDelay
            pulsegen.firstWidth = firstWidth
            pulsegen.secondLevel = secondLevel
            pulsegen.secondDelay = secondDelay
            pulsegen.secondWidth = secondWidth
            pulsegen.baseLevel = baseLevel
            self.inputs[key] = pulsegen
            pulsegen.connect('outputSrc', self._get_neuron(key), 'injectDest')
        return pulsegen    
        
    def _make_Class_1_input(self):
        input_table = moose.Table('Class_1_input')
        input_table.stepMode = 1 # Table acts as a function generator
        input_table.stepSize = self.dt
        input_table.xmin = 30e-3 # The ramp starts at 30 ms
        input_table.xmax = IzhikevichDemo.parameters['Class_1'][7] * 1e-3
        input_table.xdivs = int(ceil((input_table.xmax - input_table.xmin) / input_table.stepSize))
        input_table[0] = 0.0
        for i in range(1, len(input_table)):
            input_table[i] = 0.075e-9 * i * self.dt * 1e3
        return input_table

    def _make_Class_2_input(self):
        input_table = moose.Table('Class_2_input')
        input_table.stepMode = 1 # Table acts as a function generator
        input_table.stepSize = self.dt
        input_table.xmin = 30e-3 # The ramp starts at 30 ms
        input_table.xmax = IzhikevichDemo.parameters['Class_2'][7] * 1e-3
        input_table.xdivs = int(ceil((input_table.xmax - input_table.xmin) / input_table.stepSize))
        input_table[0] = -0.5e-9
        for i in range(1, len(input_table)):
            # The matlab code is:if (t>T1) I=-0.5+(0.015*(t-T1)); else I=-0.5
            input_table[i] = (-0.5 + 0.015 * i * self.dt * 1e3) * 1e-9 # convert dt from s to ms, and convert total current from nA to A.
        return input_table
        
import sys
if __name__ == '__main__':
    key = 'tonic_spiking'
    if len(sys.argv) > 1:
        key = sys.argv[1]
    demo = IzhikevichDemo()
    (time, Vm, Im) = demo.simulate(key)
    title(IzhikevichDemo.parameters[key][0] + '. ' + key)
    subplot(2,1,1)
    plot(time, array(Vm))
    subplot(2,1,2)
    plot(time, array(Im))
    show()
    # data.dumpFile(data.name + '.plot')
    # demo.inject_table.dumpFile(demo.inject_table.name + '.plot')
    print 'Finished simulation.'

# 
# Izhikevich.py ends here
