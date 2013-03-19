# test_tcr.py --- 
# 
# Filename: test_tcr.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Jul 16 16:12:55 2012 (+0530)
# Version: 
# Last-Updated: Wed Feb  6 12:37:17 2013 (+0530)
#           By: subha
#     Update #: 496
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

# Code:

import unittest
from cell_test_util import setup_current_step_model, SingleCellCurrentStepTest
import testutils
import cells
import moose
from moose import utils

simdt = 5e-6
plotdt = 0.25e-3
simtime = 1.0
    

# pulsearray = [[1.0, 100e-3, 1e-9],
#               [0.5, 100e-3, 0.3e-9],
#               [0.5, 100e-3, 0.1e-9],
#               [0.5, 100e-3, -0.1e-9],
#               [0.5, 100e-3, -0.3e-9]]



class TestTCR(SingleCellCurrentStepTest):
    def __init__(self, *args, **kwargs):
        self.celltype = 'TCR'
        SingleCellCurrentStepTest.__init__(self, *args, **kwargs)
        self.pulse_array = [(100e-3, 100e-3, 1e-9),
                            (1e9, 0, 0)]
        self.solver = 'hsolve'
        self.simdt = simdt
        self.plotdt = plotdt

    def setUp(self):
        SingleCellCurrentStepTest.setUp(self)

    def testVmSeriesPlot(self):
        self.runsim(simtime, self.pulse_array)
        self.plot_vm()

    def testVClamp(self):
        clamp = moose.VClamp('%s/vclamp' % (self.model_container.path))
        moose.connect(clamp, 'currentOut', self.cell.soma, 'injectMsg')
        moose.connect(self.cell.soma, 'VmOut', clamp, 'sensed')
        moose.connect(self.pulsegen, 'outputOut', clamp, 'command')

    def testChannelDensities(self):
        pass
        # equal = compare_cell_dump(self.dump_file, '../nrn/'+self.dump_file)
        # self.assertTrue(equal)


if __name__ == '__main__':
    unittest.main()



# 
# test_tcr.py ends here
