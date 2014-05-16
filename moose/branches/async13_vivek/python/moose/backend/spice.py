#!/usr/bin/env python

"""backend_spice.py: 

    This backend write spice files.

Last modified: Mon May 12, 2014  06:23PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@iitb.ac.in"
__status__           = "Development"

from backend  import Backend 
import print_utils as debug

import os 
import sys
import time
import datetime

thisDir = os.path.dirname( __file__ )
sys.path.append(os.path.join(thisDir, '..'))

import _moose
from methods_utils import idPathToObjPath

class Spice(Backend):
    """Converts moose to spice-netlist"""

    def __init__(self, *args):
        super(Spice, self).__init__()
        self.args = args
        self.spiceText = [ self.compartmentModel( ) ]

    def moosePathToSpiceNode(self, path):
        """Make moose-path sane for spice """
        # 
        newPath = ''
        for p in path:
            if p in "[]/_+-'()":
                newPath += ''
            else:
                newPath += p
        return newPath

    def compartmentModel(self):
        """ A default compartment model """
        model = []
        st = time.time()
        stamp = datetime.datetime.fromtimestamp(st).strftime('%Y-%m-%d-%H%M')
        model.append("* AUTOGENERATED MOOSE-MODEL IN SPICE.")
        model.append("* GENERATED ON: {}".format( stamp ))
        model.append(".SUBCKT {name} {ports} {params}".format(
                    ports = "in1 in2 inject out1 out2" 
                    , name = "moosecompartment"
                    , params = "Ra=10k Rm=10G Cm=10pF Em=-60m"
                    )
                )
        model.append("Ra1 in1 inject {Ra/2}")
        model.append("Ra2 inject out1 {Ra/2}")
        model.append("Cm inject in2 {Cm}")
        model.append("Rm inject rmcmnode {Rm}")
        model.append("VEm rmcmnode out2 dc {Em}")
        model.append(".ENDS \n\n")
        return "\n".join(model)


    def spiceLineForDevice(self, compartment):
        """Generates spice lines for a given compartment.
        axials: Axial compartments
        raxials: Raxial compartments
        """
        cPath = compartment.path
        spiceLine = ''
        compName = self.moosePathToSpiceNode( cPath )
        # Add a line for each compartments. To make connection, we add a 0V
        # voltage source. It does nothing to circuit but one can measure current
        # through it.
        spiceLine += 'X{0} n{0}in1 GND n{0}inject n{0}out1 GND '.format(compName)
        spiceLine += ' moosecompartment '
        spiceLine += 'Ra={Ra} Rm={Rm} Cm={Cm} Em={Em}'.format(
                Ra = compartment.Ra
                , Rm = compartment.Rm
                , Cm = compartment.Cm
                , Em = compartment.Em
                )
        self.spiceText.append( spiceLine )

    def spiceLineForConnections(self, compartment):
        """Add a connection between two compartments
        """

        def spiceLine( src, tgt):
            # Write a line for connections.
            spiceLine = 'V{src}{tgt} n{src}out1 n{tgt}in1 dc 0'.format(
                    src = self.moosePathToSpiceNode( src )
                    , tgt = self.moosePathToSpiceNode( tgt )
                    )
            return spiceLine

        compPath = compartment.path 
        raxials = compartment.neighbors['raxial']
        axials = compartment.neighbors['axial']

        raxialsPath = [ idPathToObjPath( p.path ) for p in raxials ]
        axialsPath = [ idPathToObjPath( p.path ) for p in axials ]

        for rpath in raxialsPath:
            if (compPath, rpath) not in self.connections:
                self.connections.add( (compPath, rpath) )
                spiceLine = spiceLine( compPath, rpath)
                self.spiceText.append( spiceLine )
            else:               # This connection is already added.
                pass

        for apath in axialsPath:
            if (apath, compPath) not in self.connections:
                self.connections.add( (apath, compPath) )
                spiceLine =  spiceLine( apath, compPath )
                self.spiceText.append( spiceLine )
            else:
                pass
            
    def spiceLineForPulseGen(self, pulseGen):
        """Write spice-line for pulse """
        pulsePath = idPathToObjPath( pulseGen.path )
        pulseName = self.moosePathToSpiceNode( pulsePath )

        td1 = pulseGen.delay[0]
        td2 = pulseGen.delay[1]
        width = pulseGen.width[0]
        level1 = 0.0
        level2 = pulseGen.level[0]
    
        targets = pulseGen.neighbors['output']
        if not targets:
            debug.dump("WARN"
                    , "It seems that pulse `%s` is not connected" % pulsePath
                    )
        for i, t in enumerate(targets):
            tPath = idPathToObjPath( t.path )
            spiceLine = "* Current pulse: N+ N-, N+ is where current enters\n"
            spiceLine += 'I{id}{name} GND n{target}inject PULSE'.format(
                    id = i
                    , name = pulseName 
                    , target = self.moosePathToSpiceNode( tPath )
                    )
            spiceLine += '({level1} {level2} {TD} {TR} {TF} {PW} {PER})'.format(
                    level1 = level1
                    , level2 = level2 
                    , TD = td1
                    , TR = 0.0
                    , TF = 0.0
                    , PW = width 
                    , PER = td1 + td2 + width
                    )
            self.spiceText.append(spiceLine)


    def buildModel(self):
        """Build data-stucture to write spice """
        self.populateStoreHouse()
        for c in self.compartments:
            self.spiceLineForDevice( c )
            self.spiceLineForConnections( c )

        self.spiceText.append("\n\n** Current pulses from moose ")
        for p in self.pulseGens:
            self.spiceLineForPulseGen(p)

    def writeSpice(self, **kwargs):
        ''' Turn moose into  spice '''
        debug.dump("STEP", "Writing moose internals to spice")
        self.buildModel()
        spiceText = "\n".join( self.spiceText )

        outputFile = kwargs.get('output', None)
        if outputFile is not None:
            debug.dump("STEP", "Writing spice netlist to {}".format(outputFile))
            with open(outputFile, "w") as spiceFile:
                spiceFile.write( spiceText )
        else:
            return spiceText

def toSpiceNetlist(**kwargs):
    """Write an equivalent spice file"""
    sp = Spice( )
    sp.writeSpice( **kwargs )

def main():
    _moose.Neutral('/cable')
    c1 = _moose.Compartment('/cable/a')
    c2 = _moose.Compartment('/cable/b')
    c1.connect('raxial', c2, 'axial')
    p = _moose.PulseGen('/pulse1')
    p.delay[0] = 0.01
    p.level[0] = 1e-9
    p.width[0] = 0.10
    p.delay[1] = 0.08
    p.connect('output', c1, 'injectMsg')
    s = Spice()
    print s.writeSpice( output='test_spice.spice' )

if __name__ == '__main__':
    main()
