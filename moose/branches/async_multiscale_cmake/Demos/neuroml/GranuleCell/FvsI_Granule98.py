#!/usr/bin/python
# -*- coding: utf-8 -*-
## all SI units
########################################################################################
## Plot the firing rate vs current injection curve for a Cerebellar Granule Cell neuron
## Author: Aditya Gilra
## Creation Date: 2012-07-12
## Modification Date: 2012-07-12
########################################################################################

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('.')
import moose
from moose.utils import *

from moose.neuroml.NeuroML import NeuroML
import pylab


SIMDT = 25e-6 # s
PLOTDT = 25e-6 # s
RUNTIME = 2.0 # s

injectmax = 20e-12 # Amperes

neuromlR = NeuroML()
neuromlR.readNeuroMLFromFile('cells_channels/Granule_98.morph.xml')
libcell = moose.Neuron('/library/Granule_98')
granCellid = moose.copy(libcell,moose.Neutral('/cells'),'granCell')
granCell = moose.Neuron(granCellid)

## edge-detect the spikes using spike-gen (table does not have edge detect)
spikeGen = moose.SpikeGen(granCell.path+'/spikeGen')
spikeGen.threshold = 0e-3 # V
granCellSoma = moose.Compartment(granCell.path+'/Soma_0')
moose.connect(granCellSoma,'VmOut',spikeGen,'Vm')
## save spikes in table
table_path = moose.Neutral(granCell.path+'/data').path
granCellSpikesTable = moose.Table(table_path+'/spikesTable')
moose.connect(spikeGen,'spikeOut',granCellSpikesTable,'spike')

## from moose_utils.py sets clocks and resets/reinits
resetSim(['/cells'], SIMDT, PLOTDT, simmethod='hsolve')

## Loop through different current injections
freqList = []
currentvec = pylab.arange(0.0, injectmax, injectmax/50.0)
for currenti in currentvec:
    moose.reinit()
    granCellSoma.inject = currenti
    moose.start(RUNTIME)
    spikesList = pylab.array(granCellSpikesTable.vector)
    if len(spikesList)>0:
        spikesList = spikesList[pylab.where(spikesList>0.0)[0]]
        spikesNow = len(spikesList)
    else: spikesNow = 0.0
    print("For injected current =", currenti
            , "number of spikes in",RUNTIME,"seconds =",spikesNow
            )
    freqList.append( spikesNow/float(RUNTIME) )

## plot the F vs I curve of the neuron
pylab.figure(facecolor='w')
pylab.plot(currentvec, freqList,'o-',linewidth=2)
pylab.xlabel('time (s)',fontsize=24)
pylab.ylabel('frequency (Hz)',fontsize=24)
pylab.title('HH single-compartment Cell',fontsize=24)
save = os.environ.get('SAVE_FIG', None)
if not save:
    pylab.show()
else:
    for i in pylab.get_fignums():
        filename = __file__+"_{}.png".format(i)
        pylab.figure(i)
        print(("\t++ Storing figure {} to {}".format(i, filename)))
        pylab.savefig(filename)

