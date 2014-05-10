'''
Created on Apr 30, 2013

@author: saeed
'''

import sys
#sys.path.append('/src/saeed/cuda-workspace/MOOSE/python')
sys.path.append('/src/saeed/cuda-workspace/moose-code/moose/branches/buildQ/python')
#sys.path.append('/usr/local/cuda/lib64/')
#sys.path.append('/usr/local/cuda-5.5/targets/x86_64-linux/lib/')
# sys.path.append('/src/saeed/cuda-workspace/moose-code/moose/branches/buildQ/hsolveCuda/cudaLibrary/')


# from ctypes import *
#lib = CDLL('/src/saeed/cuda-workspace/moose/hsolveCuda/cudaLibrary/libmooseCudaLibrary.so')
#import pdb
#pdb.set_trace()
import moose

if __name__ == '__main__':
    
    # Create Model
    model = moose.Neutral('/model')
    cell1 = moose.Neutral('/model/cell1')
# #     cell2 = moose.Neutral('/model/cell2')
    
    soma1 = moose.Compartment('/model/cell1/soma')
    soma1.Cm = 1.1e-9
    soma1.Rm = 1.1e7
    soma1.initVm = -0.211
    soma1.inject= 10e-9

    s1c1 = moose.Compartment('/model/cell1/c1')
    s1c1.Cm = 1.1e-9
    s1c1.Rm = 1.1e7
    s1c1.initVm = -0.0
    s1c2 = moose.Compartment('/model/cell1/c2')
    s1c2.Cm = 1.1e-9
    s1c2.Rm = 1.1e7
    s1c2.initVm = -0.4
    s1c2.inject= 10e-9


    moose.connect(s1c1, 'raxial', soma1, 'axial')
    moose.connect(s1c2, 'raxial', soma1, 'axial')

    
# #     soma2 = moose.Compartment('/model/cell2/soma')
# #     soma2.Cm = 1.4e-9
# #     soma2.Rm = 1.3e7
# #     soma2.initVm = -0.222
# #     s2c1 = moose.Compartment('/model/cell2/c1')
# #     s2c1.Cm = 1.1e-9
# #     s2c1.Rm = 1.1e7
# #     s2c1.initVm = -0.211
# #     s2c2 = moose.Compartment('/model/cell2/c2')
# #     s2c2.Cm = 1.1e-9
# #     s2c2.Rm = 1.1e7
# #     s2c2.initVm = -0.211
# #     moose.connect(s2c1, 'currentOut', soma2, 'injectMsg')
# #     moose.connect(s2c2, 'currentOut', soma2, 'injectMsg')

    
#     #Output
    data = moose.Neutral('/data')
    vmtab1 = moose.Table('/data/soma_Vm')
    moose.connect(vmtab1, 'requestData', soma1, 'get_Vm')
    
#     vmtab2 = moose.Table('/data/soma2_Vm')
#     moose.connect(vmtab2, 'requestData', soma2, 'get_Vm')
      
    #Clock    
    moose.setClock(0, 0.025e-3)
    moose.setClock(1, 0.025e-3)
    moose.setClock(2, 0.25e-3)
    moose.useClock(2, '/data/#', 'process')
    moose.useClock(0, '/model/##', 'init')
#     moose.useClock(0, soma2.path, 'init')

     
# #     moose.useClock(0, soma2.path, 'init')
    moose.useClock(1, '/model/##', 'process')
#       
     #Solver
    # solver1 = moose.HSolve('/model/solve1')
    # solver1.dt = moose.element('/clock/tick[0]').dt
    # solver1.target = '/model/cell1'    # Where it starts to read from the model 
    # moose.useClock(0,'/model/solve1', 'process')
       
#     solver2 = moose.HSolve('/model/solve2')
#     solver2.dt = moose.element('/clock/tick[0]').dt
#     solver2.target = '/model/cell2'    # Where it starts to read from the model 
#     moose.useClock(0,'/model/solve2', 'process')
       

    moose.reinit()
    moose.start(300e-4)
         
    import pylab
    t = pylab.linspace(0, 300e-3, len(vmtab1.vec))
    pylab.plot(t, vmtab1.vec)
    # pylab.plot(t, vmtab2.vec)
      
    pylab.show()
