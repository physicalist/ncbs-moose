#!/usr/bin/env python

# Filename       : load_multi.py 
# Created on     : 2013-10-20
# Author         : Dilawar Singh
# Email          : dilawars@ncbs.res.in
#
# Description    : Load multiscale models based on model XML files.
#
# Logs           : 
# Sun 20 Oct 2013 12:12:22 AM IST
#    -- Using Upi's Demo/snippet/MULTI/loadMulti.py file as starting point.


import os

import sys
sys.path.append('../../../python/')
import moose

import os
os.environ['NUMPTHREADS'] = '1'
import signal
import math
import proto
import parser.importer 
import debug.debug as debug

from IPython import embed
  
EREST_ACT = -70e-3

def loadElecFromXML(xmlElem) :
  ''' Read an electrical model from xmlElement '''
  pass 

def loadElec():
    library = moose.Neutral( '/library' )
    # Set current Working Element (setCwe)
    moose.setCwe( '/library' )
    proto.make_Ca()
    proto.make_Ca_conc()
    proto.make_K_AHP()
    proto.make_K_C()
    proto.make_Na()
    proto.make_K_DR()
    proto.make_K_A()
    proto.make_glu()
    proto.make_NMDA()
    proto.make_Ca_NMDA()
    proto.make_NMDA_Ca_conc()
    proto.make_axon()
    model = moose.Neutral( '/model' )
    modelPath = './models/genesis/ca1_asym.p'
    if not os.path.isfile(modelPath) :
      debug.printDebug("INFO"
        , "Trying to load non-existant model : {0}".format(modelPath))
      sys.exit(0)
    cellId = moose.loadModel( modelPath , '/model/elec', "hsolve" ) 
    return cellId

def addPlot( objpath, field, plot ):
    #assert moose.exists( objpath )
    if moose.exists( objpath ):
        tab = moose.Table( '/graphs/' + plot )
        obj = moose.element( objpath )
        if obj.className == 'Neutral':
            print "addPlot failed: object is a Neutral: ", objpath
            return moose.element( '/' )
        else:
            #print "object was found: ", objpath, obj.className
            moose.connect( tab, 'requestData', obj, field )
            return tab
    else:
        print "addPlot failed: object not found: ", objpath
        return moose.element( '/' )

def dumpPlots( fname ):
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/graphs/#[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name )
    for x in moose.wildcardFind( '/graphs/elec/#[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name + '_e' )

def moveCompt( path, oldParent, newParent ):
    meshEntries = moose.element( newParent.path + '/mesh' )
    # Set up vol messaging from new compts to all their child objects.
    for x in moose.wildcardFind( path + '/##[ISA=PoolBase]' ):
        moose.connect( meshEntries, 'mesh', x, 'mesh', 'OneToOne' )
    orig = moose.element( path )
    moose.move( orig, newParent )
    moose.delete( moose.ematrix( oldParent.path ) )
    chem = moose.element( '/model/chem' )
    moose.move( newParent, chem )

def loadChem( neuroCompt, spineCompt, psdCompt ):
    # We need the compartments to come in with a volume of 1 to match the
    # original CubeMesh.
    assert( neuroCompt.volume == 1.0 )
    assert( spineCompt.volume == 1.0 )
    assert( psdCompt.volume == 1.0 )
    assert( neuroCompt.mesh.num == 1 )
    print 'volume = ', neuroCompt.mesh[0].volume
    #assert( neuroCompt.mesh[0].volume == 1.0 ) 
    #an unfortunate mismatch
    # So we'll have to resize the volumes of the current compartments to the
    # new ones.

    modelId = moose.loadModel( './models/genesis/psd_merged31e.g', '/model', 'ee' )
    chem = moose.element( '/model/model' )
    chem.name = 'chem'
    oldN = moose.element( '/model/chem/kinetics' )
    oldS = moose.element( '/model/chem/compartment_1' )
    oldP = moose.element( '/model/chem/compartment_2' )
    print 'old NSP vols = ', oldN.volume, oldS.volume, oldP.volume
    print 'old NSP.mesh vols = ', oldN.mesh[0].volume, oldS.mesh[0].volume, oldP.mesh[0].volume 
    print 'new NSP vols = ', neuroCompt.volume, spineCompt.volume, psdCompt.volume 
    print 'new NSP.mesh vols = ', neuroCompt.mesh[0].volume, spineCompt.mesh[0].volume, psdCompt.mesh[0].volume 
    for i in moose.wildcardFind( '/model/chem/#/#/#/transloc#' ):
        print i[0].name, i[0].Kf, i[0].Kb, i[0].kf, i[0].kb
    tr0 = moose.element( '/model/chem/kinetics/SPINE/CaMKII_BULK/tr0[6]' )
    print "tr0 rates = ", tr0.Kf, tr0.Kb, tr0.kf, tr0.kb
    oldN.volume = neuroCompt.mesh[0].volume
    oldS.volume = spineCompt.mesh[0].volume
    oldP.volume = psdCompt.mesh[0].volume
    #oldN.volume = neuroCompt.volume
    #oldS.volume = spineCompt.volume
    #oldP.volume = psdCompt.volume
    print 'old NSP vols = ', oldN.volume, oldS.volume, oldP.volume
    print 'old NSP.mesh vols = ', oldN.mesh[0].volume, oldS.mesh[0].volume, oldP.mesh[0].volume 
    print 'new NSP vols = ', neuroCompt.volume, spineCompt.volume, psdCompt.volume 
    print 'new NSP.mesh vols = ', neuroCompt.mesh[0].volume, spineCompt.mesh[0].volume, psdCompt.mesh[0].volume 
    print "tr0 rates = ", tr0.Kf, tr0.Kb, tr0.kf, tr0.kb
    for i in moose.wildcardFind( '/model/chem/#/#/#/transloc#' ):
        print i[0].name, i[0].Kf, i[0].Kb, i[0].kf, i[0].kb
    moveCompt( '/model/chem/kinetics/PSD', oldP, psdCompt )
    moveCompt( '/model/chem/kinetics/SPINE', oldS, spineCompt )
    moveCompt( '/model/chem/kinetics/DEND', oldN, neuroCompt )
    print "aftermove tr0 rates = ", tr0.Kf, tr0.Kb, tr0.kf, tr0.kb
    for i in moose.wildcardFind( '/model/chem/#/#/#/transloc#' ):
        print i[0].name, i[0].Kf, i[0].Kb, i[0].kf, i[0].kb
def makeNeuroMeshModelFromXml( xmlElem ) :
  pass 

def makeNeuroMeshModel():
    debug.printDebug("INFO", "calling function makeNeuroMeshModel")
    diffLength = 20e-6 # But we only want diffusion over part of the model.
    numSyn = 13
    elec = loadElec()
    debug.printDebug("INFO", "Electrical model is added to moose")
    synInput = moose.SpikeGen( '/model/elec/synInput' )
    synInput.threshold = -1.0
    synInput.edgeTriggered = 0
    synInput.Vm( 0 )
    synInput.refractT = 47e-3

    for i in range( numSyn ):
        name = '/model/elec/spine_head_14_' + str( i + 1 )
        r = moose.element( name + '/glu' )
        r.synapse.num = 1 
        print("r.path {0}".format(r.path))
        syn = moose.element( r.path + '/synapse' )
        moose.connect( synInput, 'event', syn, 'addSpike', 'Single' )
        syn.weight = 0.2 * i * ( numSyn - 1 - i )
        syn.delay = i * 1.0e-3

    neuroCompt = moose.NeuroMesh( '/model/neuroMesh' )
    neuroCompt.separateSpines = 1
    neuroCompt.diffLength = diffLength
    neuroCompt.geometryPolicy = 'cylinder'
    spineCompt = moose.SpineMesh( '/model/spineMesh' )
    moose.connect( neuroCompt, 'spineListOut', spineCompt, 'spineList', 'OneToOne' )
    psdCompt = moose.PsdMesh( '/model/psdMesh' )
    #print 'Meshvolume[neuro, spine, psd] = ', neuroCompt.mesh[0].volume, spineCompt.mesh[0].volume, psdCompt.mesh[0].volume
    moose.connect( neuroCompt, 'psdListOut', psdCompt, 'psdList', 'OneToOne' )
    loadChem( neuroCompt, spineCompt, psdCompt )
    for x in moose.wildcardFind( '/model/chem/##[ISA=PoolBase]' ):
        if (x.diffConst > 0):
            x.diffConst = 1e-11
    for x in moose.wildcardFind( '/model/chem/##/Ca' ):
        x.diffConst = 1e-10

    # Put in the solvers, see how they fare.
    nmksolve = moose.GslStoich( '/model/chem/neuroMesh/ksolve' )
    nmksolve.path = '/model/chem/neuroMesh/##'
    nmksolve.compartment = moose.element( '/model/chem/neuroMesh' )
    nmksolve.method = 'rk5'
    nm = moose.element( '/model/chem/neuroMesh/mesh' )
    moose.connect( nm, 'remesh', nmksolve, 'remesh' )
    #print "neuron: nv=", nmksolve.numLocalVoxels, ", nav=", nmksolve.numAllVoxels, nmksolve.numVarPools, nmksolve.numAllPools

    #print 'setting up smksolve'
    smksolve = moose.GslStoich( '/model/chem/spineMesh/ksolve' )
    smksolve.path = '/model/chem/spineMesh/##'
    smksolve.compartment = moose.element( '/model/chem/spineMesh' )
    smksolve.method = 'rk5'
    sm = moose.element( '/model/chem/spineMesh/mesh' )
    moose.connect( sm, 'remesh', smksolve, 'remesh' )
    #print "spine: nv=", smksolve.numLocalVoxels, ", nav=", smksolve.numAllVoxels, smksolve.numVarPools, smksolve.numAllPools
    #
    #print 'setting up pmksolve'
    pmksolve = moose.GslStoich( '/model/chem/psdMesh/ksolve' )
    pmksolve.path = '/model/chem/psdMesh/##'
    pmksolve.compartment = moose.element( '/model/chem/psdMesh' )
    pmksolve.method = 'rk5'
    pm = moose.element( '/model/chem/psdMesh/mesh' )
    moose.connect( pm, 'remesh', pmksolve, 'remesh' )
    #print "psd: nv=", pmksolve.numLocalVoxels, ", nav=", pmksolve.numAllVoxels, pmksolve.numVarPools, pmksolve.numAllPools
    #

    # Now to set up the model.
    #neuroCompt.cell = elec # This loads the chem pathways into the entire neuronal model
    # It is preferable to put chem only in selected compartments, as here:
    os.kill( PID, signal.SIGUSR1)
    neuroCompt.cellPortion( elec, '/model/elec/lat_14_#,/model/elec/spine_neck#,/model/elec/spine_head#' )
    ns = neuroCompt.numSegments
    #assert( ns == 11 ) # dend, 5x (shaft+head)
    ndc = neuroCompt.numDiffCompts
    assert( ndc == 13 )
    ndc = neuroCompt.mesh.num
    assert( ndc == 13 )

    sdc = spineCompt.mesh.num
    assert( sdc == 13 )
    pdc = psdCompt.mesh.num
    assert( pdc == 13 )
    #print 'mesh nums ( neuro, spine, psd ) = ', ndc, sdc, pdc, ', numSeg = ', ns

    mesh = moose.ematrix( '/model/chem/neuroMesh/mesh' )
    mesh2 = moose.ematrix( '/model/chem/spineMesh/mesh' )
    mesh = moose.ematrix( '/model/chem/psdMesh/mesh' )

    # We need to use the spine solver as the master for the purposes of
    # these calculations. This will handle the diffusion calculations
    # between head and dendrite, and between head and PSD.
    smksolve.addJunction( nmksolve )
    #print "spine: nv=", smksolve.numLocalVoxels, ", nav=", smksolve.numAllVoxels, smksolve.numVarPools, smksolve.numAllPools
    smksolve.addJunction( pmksolve )
    #print "psd: nv=", pmksolve.numLocalVoxels, ", nav=", pmksolve.numAllVoxels, pmksolve.numVarPools, pmksolve.numAllPools

    # oddly, numLocalFields does not work.
    #moose.le( '/model/chem/neuroMesh' )
    ca = moose.element( '/model/chem/neuroMesh/DEND/Ca' )
    assert( ca.lastDimension == ndc )
    """
    CaNpsd = moose.ematrix( '/model/chem/psdMesh/PSD/PP1_PSD/CaN' )
    print 'numCaN in PSD = ', CaNpsd.nInit, ', vol = ', CaNpsd.volume
    CaNspine = moose.ematrix( '/model/chem/spineMesh/SPINE/CaN_BULK/CaN' )
    print 'numCaN in spine = ', CaNspine.nInit, ', vol = ', CaNspine.volume
    """

    # set up adaptors
    aCa = moose.Adaptor( '/model/chem/psdMesh/adaptCa', pdc )
    adaptCa = moose.ematrix( '/model/chem/psdMesh/adaptCa' )
    chemCa = moose.ematrix( '/model/chem/psdMesh/PSD/CaM/Ca' )
    assert( len( adaptCa ) == pdc )
    assert( len( chemCa ) == pdc )
    for i in range( pdc ):
        path = '/model/elec/spine_head_14_' + str( i + 1 ) + '/NMDA_Ca_conc'
        elecCa = moose.element( path )
        moose.connect( elecCa, 'concOut', adaptCa[i], 'input', 'Single' )
        print path
    moose.connect( adaptCa, 'outputSrc', chemCa, 'set_conc', 'OneToOne' )
    adaptCa.inputOffset = 0.0    # 
    adaptCa.outputOffset = 0.00008    # 80 nM offset in chem.
    adaptCa.scale = 1e-5    # 520 to 0.0052 mM
    print adaptCa.inputOffset
    print adaptCa.outputOffset
    print adaptCa.scale
    #print adaptCa.outputOffset
    #print adaptCa.scale

    """
    aGluR = moose.Adaptor( '/model/chem/psdMesh/adaptGluR', 5 )
    adaptGluR = moose.ematrix( '/model/chem/psdMesh/adaptGluR' )
    chemR = moose.ematrix( '/model/chem/psdMesh/psdGluR' )
    assert( len( adaptGluR ) == 5 )
    for i in range( 5 ):
        path = '/model/elec/head' + str( i ) + '/gluR'
        elecR = moose.element( path )
            moose.connect( adaptGluR[i], 'outputSrc', elecR, 'set_Gbar', 'Single' )
    #moose.connect( chemR, 'nOut', adaptGluR, 'input', 'OneToOne' )
    # Ksolve isn't sending nOut. Not good. So have to use requestField.
    moose.connect( adaptGluR, 'requestField', chemR, 'get_n', 'OneToOne' )
    adaptGluR.outputOffset = 1e-7    # pS
    adaptGluR.scale = 1e-6 / 100     # from n to pS

    adaptK = moose.Adaptor( '/model/chem/neuroMesh/adaptK' )
    chemK = moose.element( '/model/chem/neuroMesh/kChan' )
    elecK = moose.element( '/model/elec/compt/K' )
    moose.connect( adaptK, 'requestField', chemK, 'get_conc', 'OneToAll' )
    moose.connect( adaptK, 'outputSrc', elecK, 'set_Gbar', 'Single' )
    adaptK.scale = 0.3               # from mM to Siemens
    """

def makeElecPlots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    addPlot( '/model/elec/soma', 'get_Vm', 'elec/somaVm' )
    addPlot( '/model/elec/soma/Ca_conc', 'get_Ca', 'elec/somaCa' )
    addPlot( '/model/elec/basal_3', 'get_Vm', 'elec/basal3Vm' )
    addPlot( '/model/elec/apical_14', 'get_Vm', 'elec/apical_14Vm' )
    addPlot( '/model/elec/apical_14/Ca_conc', 'get_Ca', 'elec/apical_14Ca' )
    addPlot( '/model/elec/spine_head_14_7', 'get_Vm', 'elec/spine_7Vm' )
    addPlot( '/model/elec/spine_head_14_7/NMDA_Ca_conc', 'get_Ca', 'elec/spine_7Ca' )
    addPlot( '/model/elec/spine_head_14_13/NMDA_Ca_conc', 'get_Ca', 'elec/spine_13Ca' )

def makeChemPlots():
    spineMesh = moose.element( '/model/chem/spineMesh' )
    middleSpine = 6
    # handy lookup function to find which voxel # the spine is on.
    midSpineVoxel = spineMesh.parentVoxel[middleSpine]
    graphs = moose.Neutral( '/graphs' )
    #moose.le( '/model/chem/psdMesh/PSD' )
    addPlot( '/model/chem/psdMesh/PSD/tot_PSD_R[0]', 'get_n', 'psd0R' )
    addPlot( '/model/chem/psdMesh/PSD/tot_PSD_R[1]', 'get_n', 'psd1R' )
    addPlot( '/model/chem/psdMesh/PSD/tot_PSD_R[2]', 'get_n', 'psd2R' )
    addPlot( '/model/chem/psdMesh/PSD/CaM/CaM_dash_Ca4[0]', 'get_conc', 'psdCaM0' )
    addPlot( '/model/chem/psdMesh/PSD/CaM/CaM_dash_Ca4[6]', 'get_conc', 'psdCaM6' )
    addPlot( '/model/chem/psdMesh/PSD/CaM_CaN[0]', 'get_conc', 'CaM_CaN' )
    addPlot( '/model/chem/psdMesh/PSD/PP1_PSD/PP1_dash_active[0]', 'get_conc', 'PP1_active0' )
    addPlot( '/model/chem/psdMesh/PSD/PP1_PSD/PP1_dash_active[6]', 'get_conc', 'psdPP1_active6' )
    addPlot( '/model/chem/psdMesh/PSD/actCaMKII[6]', 'get_conc', 'psdCaMKII6' )
    addPlot( '/model/chem/spineMesh/SPINE/actCaMKII[6]', 'get_conc', 'spineCaMKII6' )
    #moose.le( '/model/chem/spineMesh/SPINE' )
    #moose.le( '/model/chem/neuroMesh/DEND' )
    addPlot( '/model/chem/psdMesh/PSD/CaM/Ca[0]', 'get_conc', 'psd0Ca' )
    addPlot( '/model/chem/psdMesh/PSD/CaM/Ca[6]', 'get_conc', 'psd6Ca' )
    addPlot( '/model/chem/psdMesh/PSD/CaM/Ca[12]', 'get_conc', 'psd12Ca' )

    addPlot( '/model/chem/spineMesh/SPINE/CaM/Ca[0]', 'get_conc', 'spine0Ca' )
    addPlot( '/model/chem/spineMesh/SPINE/CaM/Ca[6]', 'get_conc', 'spine6Ca' )
    addPlot( '/model/chem/spineMesh/SPINE/CaM/Ca[12]', 'get_conc', 'spine12Ca' )

    path = '/model/chem/neuroMesh/DEND/Ca['
    addPlot( '/model/chem/neuroMesh/DEND/Ca[0]', 'get_conc', 'dend0Ca' )
    addPlot( path + str( midSpineVoxel ) + ']', 'get_conc', 'dendMidCa' )
    addPlot( path + str( midSpineVoxel+2 ) + ']', 'get_conc', 'dendMid2Ca' )
    addPlot( path + str( midSpineVoxel+4 ) + ']', 'get_conc', 'dendMid4Ca' )
    addPlot( path + str( midSpineVoxel+6 ) + ']', 'get_conc', 'dendMid6Ca' )
    addPlot( '/model/chem/neuroMesh/DEND/Ca[144]', 'get_conc', 'dend144Ca' )

    #addPlot( '/n/neuroMesh/Ca', 'get_conc', 'dendCa' )
    #addPlot( '/n/neuroMesh/inact_kinase', 'get_conc', 'inactDendKinase' )
    #addPlot( '/n/psdMesh/psdGluR', 'get_n', 'psdGluR' )
    addPlot( '/model/chem/psdMesh/PSD/CaMKII_PSD/CaMKII_dash_CaM[6]', 'get_conc', 'psdCaM_CaMKII6' )
    addPlot( '/model/chem/spineMesh/SPINE/CaMKII_BULK/CaMKII_dash_CaM[6]', 'get_conc', 'spineCaM_CaMKII6' )
    addPlot( '/model/chem/spineMesh/SPINE/CaMKII_BULK/NMDAR[6]', 'get_conc', 'psd_NMDAR6' )

    addPlot( '/model/chem/psdMesh/PSD/CaMKII_PSD/bar[6]', 'get_conc', 'psd_bar' )
    addPlot( '/model/chem/spineMesh/SPINE/CaMKII_BULK/foo[6]', 'get_conc', 'spine_foo' )
    bar = moose.element( '/model/chem/psdMesh/PSD/CaMKII_PSD/bar[6]' )
    foo = moose.element( '/model/chem/spineMesh/SPINE/CaMKII_BULK/foo[6]' )
    print "bar,foo vols = ", bar.volume, foo.volume, bar.concInit, foo.concInit
    tr0 = moose.element( '/model/chem/spineMesh/SPINE/CaMKII_BULK/tr0[6]' )
    print "tr0 rates = ", tr0.Kf, tr0.Kb, tr0.kf, tr0.kb



