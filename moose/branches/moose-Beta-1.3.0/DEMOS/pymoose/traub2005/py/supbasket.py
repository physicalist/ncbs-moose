# supbasket.py --- 
# 
# Filename: supbasket.py
# Description: Superficial Layer 2/3 basket cells
# Author: subhasis ray
# Maintainer: 
# Created: Tue Oct  6 16:52:28 2009 (+0530)
# Version: 
# Last-Updated: Tue Feb  9 14:29:30 2010 (+0100)
#           By: Subhasis Ray
#     Update #: 41
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

from datetime import datetime
import config
import trbutil
import moose
from cell import *
from capool import CaPool

class SupBasket(TraubCell):
    prototype = TraubCell.read_proto('SupBasket.p', 'SupBasket')
    def __init__(self, *args):
	TraubCell.__init__(self, *args)
	
    def _topology(self):
	self.presyn = 59
	
    def _setup_passive(self):
	for comp in self.comp[1:]:
	    comp.Em = -65e-3

    def _setup_channels(self):
	for comp in self.comp[1:]:
	    ca_pool = None
            ca_dep_chans = []
            ca_chans = []
            for child in comp.children():
                obj = moose.Neutral(child)
                if obj.name == 'CaPool':
                    ca_pool = moose.CaConc(child)
                    ca_pool.tau = 20e-3
                elif obj.className == 'HHChannel':
                    obj = moose.HHChannel(child)
                    pyclass = eval(obj.name)
                    if issubclass(pyclass, KChannel):
                        obj.Ek = -100e-3
                        if issubclass(pyclass, KCaChannel):
                            ca_dep_chans.append(obj)
                    elif issubclass(pyclass, NaChannel):
                        obj.Ek = 50e-3
                    elif issubclass(pyclass, CaChannel):
                        obj.Ek = 125e-3
                        if issubclass(pyclass, CaL):
                            ca_chans.append(obj)
                    elif issubclass(pyclass, AR):
                        obj.Ek = -40e-3
                        obj.X = 0.0
                if ca_pool:
                    for channel in ca_chans:
                        channel.connect('IkSrc', ca_pool, 'current')

                    for channel in ca_dep_chans:
                        channel.useConcentration = 1
                        ca_pool.connect("concSrc", channel, "concen")

                    
        obj = moose.CaConc(self.soma.path + '/CaPool')
        obj.tau = 50e-3

    @classmethod
    def test_single_cell(cls):
        """Simulates a single nRT cell and plots the Vm and [Ca2+]"""

        config.LOGGER.info("/**************************************************************************")
        config.LOGGER.info(" *")
        config.LOGGER.info(" * Simulating a single cell: %s" % (cls.__name__))
        config.LOGGER.info(" *")
        config.LOGGER.info(" **************************************************************************/")
        sim = Simulation(cls.__name__)
        mycell = SupBasket(SupBasket.prototype, sim.model.path + "/SupBasket")
        print 'Created cell:', mycell.path
        vm_table = mycell.comp[mycell.presyn].insertRecorder('Vm_supbask', 'Vm', sim.data)
        ca_table = mycell.soma.insertCaRecorder('CaPool', sim.data)

        pulsegen = mycell.soma.insertPulseGen('pulsegen', sim.model, firstLevel=3e-10, firstDelay=50e-3, firstWidth=50e-3)


        sim.schedule()
        if mycell.has_cycle():
            print "WARNING!! CYCLE PRESENT IN CICRUIT."
        t1 = datetime.now()
        sim.run(200e-3)
        t2 = datetime.now()
        delta = t2 - t1
        print 'simulation time: ', delta.seconds + 1e-6 * delta.microseconds
        sim.dump_data('data')
        mus_vm = pylab.array(vm_table) * 1e3
        mus_t = linspace(0, sim.simtime * 1e3, len(mus_vm))
        mus_ca = pylab.array(ca_table)
        nrn_vm = trbutil.read_nrn_data('Vm_supbask.plot', 'test_supbask.hoc')
        nrn_ca = trbutil.read_nrn_data('Ca_supbask.plot', 'test_supbask.hoc')
        if len(nrn_vm) > 0:
            nrn_t = nrn_vm[:, 0]
            nrn_vm = nrn_vm[:, 1]
            nrn_ca = nrn_ca[:,1]

        trbutil.do_plot(cls.__name__, mus_t, mus_ca, mus_vm, nrn_t, nrn_ca, nrn_vm)
        
        
# test main --
from simulation import Simulation
import pylab
from subprocess import call
if __name__ == "__main__":
    SupBasket.test_single_cell()
    


# 
# supbasket.py ends here
