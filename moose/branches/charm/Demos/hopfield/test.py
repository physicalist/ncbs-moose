import moose
from pylab import *
from moose.utils import *
import numpy as np

inputGiven = 1
moose.Neutral('/elec')
pg = moose.PulseGen('/elec/inPulGen')
pgTable = moose.Table('/elec/inPulGen/pgTable')
moose.connect(pgTable, 'requestData', pg, 'get_output')

pg.firstDelay = 10e-3
pg.firstWidth = 2e-03
pg.firstLevel = 3
pg.secondDelay = 1.0

cellPath = '/cell'

# cell = moose.LeakyIaF(cellPath)
# cell.setField('Cm',1e-6)
# cell.setField('Rm',1e4)
# cell.setField('Em',-0.07)
# cell.setField('initVm',-0.05)
# cell.setField('Vreset',-0.07)
# cell.setField('Vthreshold',0.0)
# cell.setField('refractoryPeriod',0.01)

cell = moose.IntFire(cellPath)
cell.setField('tau',10e-3)
#cell.setField('Vm', 1.0)
cell.setField('refractoryPeriod', 5e-3)
cell.setField('thresh', 0.9)
cell.synapse.num = 1
cell.synapse[0].weight = 0.91 
cell.synapse[0].delay = 10e-3
#this cell fires only when synaptic weight reaching it exceeds threshold - resets to neg and resets to 0 in refractoryPeriod time
#else, goes back to 0 in tau time

VmVal = moose.Table(cellPath+'/Vm_cell')
print 'table>cellVm:', moose.connect(VmVal, 'requestData', cell, 'get_Vm')
spikeTime = moose.Table(cellPath+'/spikeTimes')
print 'table>cellSpike:', moose.connect(cell, 'spike', spikeTime, 'input')

inSpkGen = moose.SpikeGen(cellPath+'/inSpkGen')
inSpkGen.setField('threshold', 2.0)
inSpkGen.setField('edgeTriggered', True)

if inputGiven == 1:
    print 'pulse>spike:', moose.connect(pg, 'outputOut', moose.element(cellPath+'/inSpkGen'), 'Vm')
    inTable = moose.Table(cellPath+'/inSpkGen/inTable')
    print 'table>spike:',moose.connect(inTable, 'requestData', inSpkGen, 'get_hasFired')

print 'spike>cell:', moose.connect(inSpkGen, 'event', cell.synapse[0] ,'addSpike')
#print 'spike>cell:', moose.connect(pg, 'outputOut', cell ,'injectDest')

moose.setClock(0, 1e-4)
moose.useClock(0, '/cell,/cell/##,/elec/##','process')
moose.reinit()
moose.start(0.2)

subplot(311)
plot(np.arange(0,0.2,0.2/len(pgTable.vec[1:])),pgTable.vec[1:])
subplot(312)
#plot(inTable.vec[1:])
scatter(spikeTime.vec,np.ones(len(spikeTime.vec)))
subplot(313)
plot(np.arange(0,0.2,0.2/len(pgTable.vec[1:])),VmVal.vec[1:])
show()
