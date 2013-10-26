import moose
import numpy
import math

EREST_ACT = -0.060      # /* hippocampal cell resting potl */
ENA = 0.115 + EREST_ACT # // 0.055
EK = -0.015 + EREST_ACT # // -0.075
ECA = 0.140 + EREST_ACT # // 0.080
SOMA_A = 3.320e-9       # // soma area in square meters
# Traub tabulated calcium channel
def make_Ca():
        if moose.exists( 'Ca' ):
                return
        Ca = moose.HHChannel( 'Ca' )
        Ca.Ek = ECA
        Ca.Gbar = 40 * SOMA_A
        Ca.Gk = 0
        Ca.Xpower = 2
        Ca.Ypower = 1
        Ca.Zpower = 0
    
        xgate = moose.element( 'Ca/gateX' )
    xA = numpy.array( [ 1.6e3, 0, 1.0, -1.0 * (0.065 + EREST_ACT), -0.01389
            , -20e3 * (0.0511 + EREST_ACT), 20e3, -1.0, -1.0 * (0.0511 + EREST_ACT)
            , 5.0e-3, 3000, -0.1, 0.05 ] )
    xgate.setupAlpha( xA )

        ygate = moose.element( 'Ca/gateY' )
    ygate.min = -0.1
    ygate.max = 0.05
    ygate.divs = 3000
    yA = numpy.zeros( (ygate.divs + 1), dtype=float)
    yB = numpy.zeros( (ygate.divs + 1), dtype=float)

    #Fill the Y_A table with alpha values and the Y_B table with (alpha+beta)
    dx = (ygate.max - ygate.min)/ygate.divs
    x = ygate.min
    for i in range( ygate.divs + 1 ):
            if ( x > EREST_ACT):
                    yA[i] = 5.0 * math.exp( -50 * (x - EREST_ACT) )
            else:
                    yA[i] = 5.0
            yB[i] = 5.0
            x += dx
    ygate.tableA = yA
    ygate.tableB = yB
    # Tell the cell reader that the current from this channel must be fed into
    # the Ca_conc pool of calcium.
    addmsg1 = moose.Mstring( '/library/Ca/addmsg1' )
    addmsg1.value = '.      IkOut   ../Ca_conc      current'

    # in some compartments, whe have an NMDA_Ca_conc object to put the current
    # into.
    addmsg2 = moose.Mstring( '/library/Ca/addmsg2' )
    addmsg2.value = '.      IkOut   ../NMDA_Ca_conc current'
def make_K_AHP():
        if moose.exists( 'K_AHP' ):
                return

    K_AHP = moose.HHChannel( 'K_AHP' )
    K_AHP.Ek = EK                      # V
    K_AHP.Gbar = 8 * SOMA_A            # S
    K_AHP.Gk = 0                       # S
    K_AHP.Xpower = 0
    K_AHP.Ypower = 0
    K_AHP.Zpower = 1

        zgate = moose.element( 'K_AHP/gateZ' )
        xmax = 500.0
        zgate.min = 0
        zgate.max = xmax
        zgate.divs = 3000
        zA = numpy.zeros( (zgate.divs + 1), dtype=float)
        zB = numpy.zeros( (zgate.divs + 1), dtype=float)
        dx = (zgate.max - zgate.min)/zgate.divs
        x = zgate.min
        for i in range( zgate.divs + 1 ):
                if (x < (xmax / 2.0 )):
                        zA[i] = 0.02*x
                else:
                        zA[i] = 10.0
                zB[i] = zA[i] + 1.0
                x = x + dx

        zgate.tableA = zA
        zgate.tableB = zB
    
    # Use an added field to tell the cell reader to set up a message from the
    # Ca_Conc with concentration info, to the current K_AHP object.
        addmsg1 = moose.Mstring( '/library/K_AHP/addmsg1' )
        addmsg1.value = '../Ca_conc     concOut . concen'

def make_K_C():
        if moose.exists( 'K_C'):
                return

        K_C = moose.HHChannel( 'K_C' )
        K_C.Ek = EK                                     #       V
        K_C.Gbar = 100.0 * SOMA_A       #       S
        K_C.Gk = 0                                      #       S
        K_C.Xpower = 1
        K_C.Zpower = 1
        K_C.instant = 4                         # Flag: 0x100 means Z gate is instant.

        # Now make a X-table for the voltage-dependent activation parameter.
        xgate = moose.element( 'K_C/gateX' )
        xgate.min = -0.1
        xgate.max = 0.05
        xgate.divs = 3000
        xA = numpy.zeros( (xgate.divs + 1), dtype=float)
        xB = numpy.zeros( (xgate.divs + 1), dtype=float)
        dx = (xgate.max - xgate.min)/xgate.divs
        x = xgate.min
        for i in range( xgate.divs + 1 ):
                alpha = 0.0
                beta = 0.0
                if (x < EREST_ACT + 0.05):
                        alpha = math.exp( 53.872 * (x - EREST_ACT) - 0.66835 ) / 0.018975
                        beta = 2000* (math.exp ( (EREST_ACT + 0.0065 - x)/0.027)) - alpha
                else:
                        alpha = 2000 * math.exp( ( EREST_ACT + 0.0065 - x)/0.027 )
                        beta = 0.0
                xA[i] = alpha
                xB[i] = alpha + beta
                x = x + dx
        xgate.tableA = xA
        xgate.tableB = xB

    # Create a table for the function of concentration, allowing a
    # concentration range of 0 to 1000, with 50 divisions.  This is done
    # using the Z gate, which can receive a CONCEN message.  By using
    # the "instant" flag, the A and B tables are evaluated as lookup tables,
    #  rather than being used in a differential equation.
        zgate = moose.element( 'K_C/gateZ' )
        zgate.min = 0.0
        xmax = 500.0
        zgate.max = xmax
        zgate.divs = 3000
        zA = numpy.zeros( (zgate.divs + 1), dtype=float)
        zB = numpy.zeros( (zgate.divs + 1), dtype=float)
        dx = ( zgate.max -  zgate.min)/ zgate.divs
        x = zgate.min
        for i in range( xgate.divs + 1 ):
                if ( x < ( xmax / 4.0 ) ):
                        zA[i] = x * 4.0 / xmax
                else:
                        zA[i] = 1.0
                zB[i] = 1.0
                x += dx
        zgate.tableA = zA
        zgate.tableB = zB
   
   # Now we need to provide for messages that link to external elements.
   # The message that sends the Ca concentration to the Z gate tables is stored
   # in an added field of the channel, so that it may be found by the cell
   # reader.
        addmsg1 = moose.Mstring( '/library/K_C/addmsg1' )
        addmsg1.value = '../Ca_conc     concOut . concen'
def make_Na():
        if moose.exists( 'Na' ):
                return
        Na = moose.HHChannel( 'Na' )
        Na.Ek = ENA                             #       V
        Na.Gbar = 300 * SOMA_A  #       S
        Na.Gk = 0                               #       S
        Na.Xpower = 2
        Na.Ypower = 1
        Na.Zpower = 0

        xgate = moose.element( 'Na/gateX' )
        xA = numpy.array( [ 320e3 * (0.0131 + EREST_ACT),
                -320e3, -1.0, -1.0 * (0.0131 + EREST_ACT), -0.004, 
                -280e3 * (0.0401 + EREST_ACT), 280e3, -1.0, 
                -1.0 * (0.0401 + EREST_ACT), 5.0e-3, 
                3000, -0.1, 0.05 ] )
        xgate.setupAlpha( xA )

        ygate = moose.element( 'Na/gateY' )
        yA = numpy.array( [ 128.0, 0.0, 0.0, -1.0 * (0.017 + EREST_ACT), 0.018,
                4.0e3, 0.0, 1.0, -1.0 * (0.040 + EREST_ACT), -5.0e-3, 
                3000, -0.1, 0.05 ] )
        ygate.setupAlpha( yA )

def make_K_DR():
        if moose.exists( 'K_DR' ):
                return
        K_DR = moose.HHChannel( 'K_DR' )
        K_DR.Ek = EK                            #       V
        K_DR.Gbar = 150 * SOMA_A        #       S
        K_DR.Gk = 0                             #       S
        K_DR.Xpower = 1
        K_DR.Ypower = 0
        K_DR.Zpower = 0

        xgate = moose.element( 'K_DR/gateX' )
        xA = numpy.array( [ 16e3 * (0.0351 + EREST_ACT), 
                -16e3, -1.0, -1.0 * (0.0351 + EREST_ACT), -0.005,
                250, 0.0, 0.0, -1.0 * (0.02 + EREST_ACT), 0.04,
                3000, -0.1, 0.05 ] )
        xgate.setupAlpha( xA )
def make_K_A():
        if moose.exists( 'K_A' ):
                return
        K_A = moose.HHChannel( 'K_A' )
        K_A.Ek = EK                             #       V
        K_A.Gbar = 50 * SOMA_A  #       S
        K_A.Gk = 0                              #       S
        K_A.Xpower = 1
        K_A.Ypower = 1
        K_A.Zpower = 0

        xgate = moose.element( 'K_A/gateX' )
        xA = numpy.array( [ 20e3 * (0.0131 + EREST_ACT), 
                -20e3, -1.0, -1.0 * (0.0131 + EREST_ACT), -0.01,
                -17.5e3 * (0.0401 + EREST_ACT), 
                17.5e3, -1.0, -1.0 * (0.0401 + EREST_ACT), 0.01,
                3000, -0.1, 0.05 ] )
        xgate.setupAlpha( xA )

        ygate = moose.element( 'K_A/gateY' )
        yA = numpy.array( [ 1.6, 0.0, 0.0, 0.013 - EREST_ACT, 0.018,
                50.0, 0.0, 1.0, -1.0 * (0.0101 + EREST_ACT), -0.005,
                3000, -0.1, 0.05 ] )
        ygate.setupAlpha( yA )

def make_Ca_conc():
        if moose.exists( 'Ca_conc' ):
                return
        conc = moose.CaConc( 'Ca_conc' )
        conc.tau = 0.013333  # sec
        conc.B  = 17.402e12 # Curr to conc conversion for soma
        conc.Ca_base = 0.0
def make_NMDA_Ca_conc():
        if moose.exists( 'NMDA_Ca_conc' ):
                return
        NMDA_Ca_conc = moose.CaConc( 'NMDA_Ca_conc' )
        NMDA_Ca_conc.tau = 0.004        # sec. Faster in spine than dend
        NMDA_Ca_conc.B = 17.402e12      # overridden by cellreader.
        NMDA_Ca_conc.Ca_base = 0.0

def make_glu():
        if moose.exists( 'glu' ):
                return
        glu = moose.SynChan( 'glu' )
        glu.Ek = 0.0
        glu.tau1 = 2.0e-3
        glu.tau2 = 9.0e-3
        glu.Gbar = 40 * SOMA_A
def make_NMDA():
        if moose.exists( 'NMDA' ):
                return
        NMDA = moose.SynChan( 'NMDA' )
        NMDA.Ek = 0.0
        NMDA.tau1 = 20.0e-3
        NMDA.tau2 = 20.0e-3
        NMDA.Gbar = 5 * SOMA_A

        block = moose.MgBlock( '/library/NMDA/block' )
        block.CMg = 1.2         #       [Mg] in mM
        block.Zk = 2
        block.KMg_A = 1.0/0.28
        block.KMg_B = 1.0/62

        moose.connect( NMDA, 'channelOut', block, 'origChannel', 'OneToOne' )
        addmsg1 = moose.Mstring( '/library/NMDA/addmsg1' )
        addmsg1.value = '.. channel     ./block channel'
        #Here we want to also tell the cell reader to _remove_ the original
        #Gk, Ek term going from the channel to the compartment, as this is
        # now handled by the MgBlock.
        #addmsg2 = moose.Mstring( 'NMDA/addmsg2'
        #addmsg2.value = 'DropMsg       ..      channel'
        addmsg3 = moose.Mstring( '/library/NMDA/addmsg3' )
        addmsg3.value = '.. VmOut       .       Vm'

def make_Ca_NMDA():
        if moose.exists( 'Ca_NMDA' ):
                return
        Ca_NMDA = moose.SynChan( 'Ca_NMDA' )
        Ca_NMDA.Ek = ECA
        Ca_NMDA.tau1 = 20.0e-3
        Ca_NMDA.tau2 = 20.0e-3
        Ca_NMDA.Gbar = 5 * SOMA_A

        block = moose.MgBlock( '/library/Ca_NMDA/block' )
        block.CMg = 1.2         #       [Mg] in mM
        block.Zk = 2
        block.KMg_A = 1.0/0.28
        block.KMg_B = 1.0/62

        moose.connect( Ca_NMDA, 'channelOut', block, 'origChannel', 'OneToOne' )
        addmsg1 = moose.Mstring( '/library/Ca_NMDA/addmsg1' )
        addmsg1.value = '.. VmOut       ./block Vm'
        addmsg2 = moose.Mstring( '/library/Ca_NMDA/addmsg2' )
        addmsg2.value = './block        IkOut ../NMDA_Ca_conc current'
        # The original model has the Ca current also coming here.

def make_axon():
        if moose.exists( 'axon' ):
                return
        axon = moose.SpikeGen( 'axon' )
        axon.threshold = -40e-3         # V
        axon.abs_refract = 10e-3        # sec
    
