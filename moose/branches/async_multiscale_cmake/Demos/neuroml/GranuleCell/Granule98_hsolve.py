## Aditya Gilra, NCBS, Bangalore, 2012

"""
Inside the .../Demos/GranuleCell/ directory supplied with MOOSE, run
python testNeuroML_Gran98.py
(other channels and morph xml files are already present in this same directory).
The soma name below is hard coded for gran98, else any other file can be used by modifying this script.
"""

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')

import moose
from moose.utils import *
from moose.neuroml.NeuroML import NeuroML
import pylab


simdt = 10e-6 # s
plotdt = 10e-6 # s
runtime = 0.7 # s
cells_path = '/cells' # neuromlR.readNeuroMLFromFile creates cells in '/cells'

def loadGran98NeuroML_L123(filename):
    neuromlR = NeuroML()
    populationDict, projectionDict = \
        neuromlR.readNeuroMLFromFile(filename)
    soma_path = populationDict['Gran'][1][0].path+'/Soma_0'
    somaVm = setupTable('somaVm',moose.Compartment(soma_path),'Vm')
    somaCa = setupTable('somaCa',moose.CaConc(soma_path+'/Gran_CaPool_98'),'Ca')
    somaIKCa = setupTable('somaIKCa',moose.HHChannel(soma_path+'/Gran_KCa_98'),'Gk')
    ## Am not able to plot KDr gating variable X when running under hsolve
    #KDrX = setupTable('ChanX',moose.HHChannel(soma_path+'/Gran_KDr_98'),'X')

    print("Reinit MOOSE ... ")
    resetSim(['/elec',cells_path], simdt, plotdt, simmethod='hsolve')

    print("Running ... ")
    moose.start(runtime)
    tvec = pylab.arange(0.0,runtime*2.0,plotdt)
    tvec = tvec[ : somaVm.vector.size ]
    pylab.plot(tvec,somaVm.vector)
    pylab.title('Soma Vm')
    pylab.xlabel('time (s)')
    pylab.ylabel('Voltage (V)')
    pylab.figure()
    pylab.plot(tvec,somaCa.vector)
    pylab.title('Soma Ca')
    pylab.xlabel('time (s)')
    pylab.ylabel('Ca conc (mol/m^3)')
    pylab.figure()
    pylab.plot(tvec,somaIKCa.vector)
    pylab.title('soma KCa current')
    pylab.xlabel('time (s)')
    pylab.ylabel('KCa current (A)')
    print("Showing plots ...")
    save  = os.environ.get('SAVE_FIG', None)
    if not save:
        pylab.show()
    else:
        for i in pylab.get_fignums():
            filename = __file__+"_{}.png".format(i)
            pylab.figure(i)
            print(("\t++ Storing figure {} to {}".format(i, filename)))
            pylab.savefig(filename)

filename = "GranuleCell.net.xml"
if __name__ == "__main__":
    if len(sys.argv)<2:
        filename = "GranuleCell.net.xml"
    else:
        filename = sys.argv[1]
loadGran98NeuroML_L123(filename)
