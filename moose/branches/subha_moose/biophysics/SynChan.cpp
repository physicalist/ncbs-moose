/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <math.h>
#include "moose.h"
#include <queue>
#include "SynInfo.h"
#include "SynChan.h"
#include "../element/Neutral.h"

// static const double M_E = 2.7182818284590452354;
static const double SynE = 2.7182818284590452354;

const Cinfo* initSynChanCinfo()
{
	/** 
	 * This is a shared message to receive Process message from
	 * the scheduler.
	 */
	static Finfo* processShared[] =
	{
		new DestFinfo( "process", Ftype1< ProcInfo >::global(),
				RFCAST( &SynChan::processFunc ) ),
	    new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
				RFCAST( &SynChan::reinitFunc ) ),
	};
	static Finfo* process =	new SharedFinfo( "process", processShared, 
			sizeof( processShared ) / sizeof( Finfo* ) );
	/**
	 * This is a shared message to couple channel to compartment.
	 * The first entry is a MsgSrc to send Gk and Ek to the compartment
	 * The second entry is a MsgDest for Vm from the compartment.
	 */
	static Finfo* channelShared[] =
	{
		new SrcFinfo( "channel", Ftype2< double, double >::global() ),
		new DestFinfo( "Vm", Ftype1< double >::global(), 
				RFCAST( &SynChan::channelFunc ) ),
	};

///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////

	static Finfo* SynChanFinfos[] =
	{
		new ValueFinfo( "Gbar", ValueFtype1< double >::global(),
			GFCAST( &SynChan::getGbar ), 
			RFCAST( &SynChan::setGbar )
		),
		new ValueFinfo( "Ek", ValueFtype1< double >::global(),
			GFCAST( &SynChan::getEk ), 
			RFCAST( &SynChan::setEk )
		),
		new ValueFinfo( "tau1", ValueFtype1< double >::global(),
			GFCAST( &SynChan::getTau1 ), 
			RFCAST( &SynChan::setTau1 )
		),
		new ValueFinfo( "tau2", ValueFtype1< double >::global(),
			GFCAST( &SynChan::getTau2 ), 
			RFCAST( &SynChan::setTau2 )
		),
		new ValueFinfo( "normalizeWeights", 
			ValueFtype1< bool >::global(),
			GFCAST( &SynChan::getNormalizeWeights ), 
			RFCAST( &SynChan::setNormalizeWeights )
		),
		new ValueFinfo( "Gk", ValueFtype1< double >::global(),
			GFCAST( &SynChan::getGk ), 
			RFCAST( &SynChan::setGk )
		),
		new ValueFinfo( "Ik", ValueFtype1< double >::global(),
			GFCAST( &SynChan::getIk ), 
			RFCAST( &SynChan::setIk )
		),

		new ValueFinfo( "numSynapses",
			ValueFtype1< unsigned int >::global(),
			GFCAST( &SynChan::getNumSynapses ), 
			&dummyFunc // Prohibit reassignment of this index.
		),

		new LookupFinfo( "weight",
			LookupFtype< double, unsigned int >::global(),
			GFCAST( &SynChan::getWeight ),
			RFCAST( &SynChan::setWeight )
		),

		new LookupFinfo( "delay",
			LookupFtype< double, unsigned int >::global(),
			GFCAST( &SynChan::getDelay ),
			RFCAST( &SynChan::setDelay )
		),
///////////////////////////////////////////////////////
// Shared message definitions
///////////////////////////////////////////////////////
		process,

		/*
		new SharedFinfo( "process", processShared,
			sizeof( processShared ) / sizeof( Finfo* ) ), 
			*/
		new SharedFinfo( "channel", channelShared,
			sizeof( channelShared ) / sizeof( Finfo* ) ),

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
		new SrcFinfo( "IkSrc", Ftype1< double >::global() ),

///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
		// Arrival of a spike. Arg is time of sending of spike.
		new DestFinfo( "synapse", Ftype1< double >::global(),
				RFCAST( &SynChan::synapseFunc ) ),

		// Sometimes we want to continuously activate the channel
		new DestFinfo( "activation", Ftype1< double >::global(),
				RFCAST( &SynChan::activationFunc ) ),

		// Modulate channel response
		new DestFinfo( "modulator", Ftype1< double >::global(),
				RFCAST( &SynChan::modulatorFunc ) ),
	};

	// SynChan is scheduled after the compartment calculations.
	static SchedInfo schedInfo[] = { { process, 0, 1 } };

	static Cinfo SynChanCinfo(
		"SynChan",
		"Upinder S. Bhalla, 2007, NCBS",
		"SynChan: Synaptic channel incorporating weight and delay. Does not\nhandle activity-dependent modification, see HebbSynChan for \nthat. Very similiar to the old synchan from GENESIS.", 
		initNeutralCinfo(),
		SynChanFinfos,
		sizeof( SynChanFinfos )/sizeof(Finfo *),
		ValueFtype1< SynChan >::global(),
		schedInfo, 1
	);

	return &SynChanCinfo;
}

static const Cinfo* synChanCinfo = initSynChanCinfo();

static const unsigned int channelSlot =
	initSynChanCinfo()->getSlotIndex( "channel" );
static const unsigned int ikSlot =
	initSynChanCinfo()->getSlotIndex( "IkSrc" );
static const unsigned int synapseSlot =
	initSynChanCinfo()->getSlotIndex( "synapse" );


///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void SynChan::setGbar( const Conn& c, double Gbar )
{
	static_cast< SynChan* >( c.data() )->Gbar_ = Gbar;
}
double SynChan::getGbar( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->Gbar_;
}

void SynChan::setEk( const Conn& c, double Ek )
{
	static_cast< SynChan* >( c.data() )->Ek_ = Ek;
}
double SynChan::getEk( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->Ek_;
}

void SynChan::setTau1( const Conn& c, double tau1 )
{
	static_cast< SynChan* >( c.data() )->tau1_ = tau1;
}
double SynChan::getTau1( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->tau1_;
}

void SynChan::setTau2( const Conn& c, double tau2 )
{
	static_cast< SynChan* >( c.data() )->tau2_ = tau2;
}
double SynChan::getTau2( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->tau2_;
}

void SynChan::setNormalizeWeights( const Conn& c, bool value )
{
	static_cast< SynChan* >( c.data() )->normalizeWeights_ = value;
}
bool SynChan::getNormalizeWeights( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->normalizeWeights_;
}

void SynChan::setGk( const Conn& c, double Gk )
{
	static_cast< SynChan* >( c.data() )->Gk_ = Gk;
}
double SynChan::getGk( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->Gk_;
}

void SynChan::setIk( const Conn& c, double Ik )
{
	static_cast< SynChan* >( c.data() )->Ik_ = Ik;
}
double SynChan::getIk( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->Ik_;
}

int SynChan::getNumSynapses( const Element* e )
{
	return static_cast< SynChan* >( e->data() )->synapses_.size();
}

void SynChan::setWeight(
				const Conn& c, double val, const unsigned int& i )
{
	vector< SynInfo >& synapses = 
			static_cast< SynChan* >( c.data() )->synapses_;
	if ( i < synapses.size() )
		synapses[i].weight = val;
	else 
		cout << "Error: SynChan::setWeight: Index " << i << 
			" out of range\n";
}
double SynChan::getWeight( const Element* e, const unsigned int& i )
{
	vector< SynInfo >& synapses = 
			static_cast< SynChan* >( e->data() )->synapses_;
	if ( i < synapses.size() )
		return synapses[i].weight;
	cout << "Error: SynChan::getWeight: Index " << i << 
			" out of range\n";
	return 0.0;
}

void SynChan::setDelay(
				const Conn& c, double val, const unsigned int& i )
{
	vector< SynInfo >& synapses = 
			static_cast< SynChan* >( c.data() )->synapses_;
	if ( i < synapses.size() )
		synapses[i].delay = val;
	else 
		cout << "Error: SynChan::setDelay: Index " << i << 
			" out of range\n";
}
double SynChan::getDelay( const Element* e, const unsigned int& i )
{
	vector< SynInfo >& synapses = 
			static_cast< SynChan* >( e->data() )->synapses_;
	if ( i < synapses.size() )
		return synapses[i].delay;
	cout << "Error: SynChan::getDelay: Index " << i << 
			" out of range\n";
	return 0.0;
}

unsigned int SynChan::updateNumSynapse( const Element* e )
{
	static const Finfo* synFinfo = initSynChanCinfo()->findFinfo( "synapse" );

	unsigned int n = synFinfo->numIncoming( e );
	if ( n >= synapses_.size() )
			synapses_.resize( n );
	return synapses_.size();
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void SynChan::innerProcessFunc( Element* e, ProcInfo info )
{
	while ( !pendingEvents_.empty() &&
		pendingEvents_.top().delay <= info->currTime_ ) {
		activation_ += pendingEvents_.top().weight / info->dt_;
		pendingEvents_.pop();
	}
	X_ = modulation_ * activation_ * xconst1_ + X_ * xconst2_;
	Y_ = X_ * yconst1_ + Y_ * yconst2_;
	Gk_ = Y_ * norm_;
	Ik_ = ( Ek_ - Vm_ ) * Gk_;
	activation_ = 0.0;
	modulation_ = 1.0;
	send2< double, double >( e, channelSlot, Gk_, Ek_ );
	send1< double >( e, ikSlot, Ik_ );
}
void SynChan::processFunc( const Conn& c, ProcInfo p )
{
	Element* e = c.targetElement();
	static_cast< SynChan* >( e->data() )->innerProcessFunc( e, p );
}

/*
 * Note that this causes issues if we have variable dt.
 */
void SynChan::innerReinitFunc( Element* e, ProcInfo info )
{
	double dt = info->dt_;
	activation_ = 0.0;
	modulation_ = 1.0;
	Gk_ = 0.0;
	Ik_ = 0.0;
	X_ = 0.0;
	Y_ = 0.0;
	xconst1_ = tau1_ * ( 1.0 - exp( -dt / tau1_ ) );
	xconst2_ = exp( -dt / tau1_ );
	yconst1_ = tau2_ * ( 1.0 - exp( -dt / tau2_ ) );
	yconst2_ = exp( -dt / tau2_ );
	if ( tau1_ == tau2_ ) {
		norm_ = Gbar_ * SynE / tau1_;
	} else {
		double tpeak = tau1_ * tau2_ * log( tau1_ / tau2_ ) / 
			( tau1_ - tau2_ );
		norm_ = Gbar_ * ( tau1_ - tau2_ ) / 
			( tau1_ * tau2_ * ( 
				exp( -tpeak / tau1_ ) - exp( tpeak / tau2_ )
			) );
	}
	updateNumSynapse( e );
	if ( normalizeWeights_ && synapses_.size() > 0 )
		norm_ /= static_cast< double >( synapses_.size() );
	while ( !pendingEvents_.empty() )
		pendingEvents_.pop();
}
void SynChan::reinitFunc( const Conn& c, ProcInfo p )
{
	Element* e = c.targetElement();
	static_cast< SynChan* >( e->data() )->innerReinitFunc( e, p );
}

void SynChan::innerSynapseFunc( const Conn& c, double time )
{
	unsigned int index = 
		c.targetElement()->connDestRelativeIndex( c, synapseSlot );
	// Actually we should simply ignore any message where the
	// index is bigger than synapses_.size(), because the syn
	// strength will not yet have been set.
	assert( index < synapses_.size() );
	// The event function generates a new SynInfo object with the
	// time argument added to the delay_ field.
	// This goes into a priority_queue sorted by delay_.
	pendingEvents_.push( synapses_[index].event( time ) );
}
void SynChan::synapseFunc( const Conn& c, double time )
{
	static_cast< SynChan* >( c.data() )->innerSynapseFunc( c, time );
}

void SynChan::channelFunc( const Conn& c, double Vm )
{
	static_cast< SynChan* >( c.data() )->Vm_ = Vm;
}

void SynChan::activationFunc( const Conn& c, double val )
{
	static_cast< SynChan* >( c.data() )->activation_ += val;
}

void SynChan::modulatorFunc( const Conn& c, double val )
{
	static_cast< SynChan* >( c.data() )->modulation_ *= val;
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS
#include "../element/Neutral.h"
#include "SpikeGen.h"

/**
 * Here we set up a SynChan recieving spike inputs from two
 * SpikeGens. The first has a delay of 1 msec, the second of 10.
 * The tau of the SynChan is 1 msec.
 * We test for generation of peak responses at the right time, that
 * is 2 and 11 msec.
 */
void testSynChan()
{
	cout << "\nTesting SynChan" << flush;

	Element* n = Neutral::create( "Neutral", "n", Element::root() );
	Element* syn = Neutral::create( "SynChan", "syn", n );
	Element* sg1 = Neutral::create( "SpikeGen", "sg1", n );
	Element* sg2 = Neutral::create( "SpikeGen", "sg2", n );
	ASSERT( syn != 0, "creating Synapse" );
	ASSERT( sg1 != 0, "testing Synapse" );
	ASSERT( sg2 != 0, "testing Synapse" );
	ProcInfoBase p;
	p.dt_ = 1.0e-4;
	p.currTime_ = 0;
	Conn c( syn, 0 );
	Conn csg1( sg1, 0 );
	Conn csg2( sg2, 0 );
	bool ret;
	ret = set< double >( syn, "tau1", 1.0e-3 );
	ASSERT( ret, "setup SynChan" );
	ret = set< double >( syn, "tau2", 1.0e-3 );
	ASSERT( ret, "setup SynChan" );
	ret = set< double >( syn, "Gbar", 1.0 );
	ASSERT( ret, "setup SynChan" );
	ret = sg1->findFinfo( "event" )->
			add( sg1, syn, syn->findFinfo( "synapse" ) );
	ASSERT( ret, "setup SynChan" );
	ret = sg2->findFinfo( "event" )->
			add( sg2, syn, syn->findFinfo( "synapse" ) );
	ASSERT( ret, "setup SynChan" );
	SynChan::reinitFunc( c, &p );
	
	ret = set< double >( sg1, "threshold", 0.0 );
	ret = set< double >( sg1, "refractT", 1.0 );
	ret = set< double >( sg1, "amplitude", 1.0 );
	ret = set< double >( sg2, "threshold", 0.0 );
	ret = set< double >( sg2, "refractT", 1.0 );
	ret = set< double >( sg2, "amplitude", 1.0 );
	SpikeGen::reinitFunc( csg1, &p );
	SpikeGen::reinitFunc( csg2, &p );

	unsigned int temp;
	ret = get< unsigned int >( syn, "numSynapses", temp );
	ASSERT( ret, "setup SynChan" );
	ASSERT( temp == 2, "setup SynChan" );
	ret = lookupSet< double, unsigned int >( syn, "weight", 1.0, 0 );
	ASSERT( ret, "setup SynChan" );
	ret = lookupSet< double, unsigned int >( syn, "delay", 0.001, 0 );
	ASSERT( ret, "setup SynChan" );
	ret = lookupSet< double, unsigned int >( syn, "weight", 1.0, 1 );
	ASSERT( ret, "setup SynChan" );
	ret = lookupSet< double, unsigned int >( syn, "delay", 0.01, 1 );
	ASSERT( ret, "setup SynChan" );

	double dret;
	ret = lookupGet< double, unsigned int >( syn, "weight", dret, 0 );
	ASSERT( dret == 1.0, "setup SynChan" );
	ret = lookupGet< double, unsigned int >( syn, "delay", dret, 0 );
	ASSERT( dret == 0.001, "setup SynChan" );
	ret = lookupGet< double, unsigned int >( syn, "weight", dret, 1 );
	ASSERT( dret == 1.0, "setup SynChan" );
	ret = lookupGet< double, unsigned int >( syn, "delay", dret, 1 );
	ASSERT( dret == 0.01, "setup SynChan" );

	ret = set< double >( sg1, "Vm", 2.0 );
	ret = set< double >( sg2, "Vm", 2.0 );
	ASSERT( ret, "setup SynChan" );
	ret = get< double >( syn, "Gk", dret );
	ASSERT( ret, "setup SynChan" );
	// cout << "dret = " << dret << endl;
	ASSERT( dret == 0.0, "setup SynChan" );
	// Set off the two action potls. They should arrive at 1 and 10 msec
	// respectively
	SpikeGen::processFunc( csg1, &p );
	SpikeGen::processFunc( csg2, &p );

	// First 1 msec is the delay, so response should be zero.
	for ( p.currTime_ = 0.0; p.currTime_ < 0.001; p.currTime_ += p.dt_ )
		SynChan::processFunc( c, &p );
	ret = get< double >( syn, "Gk", dret );
	// cout << "t = " << p.currTime_ << " dret = " << dret << endl;
	ASSERT( dret == 0.0, "Testing SynChan response" );

	// At 0.5 msec after delay, that is, 1.5 msec, it is at half-tau.
	for ( ; p.currTime_ < 0.0015; p.currTime_ += p.dt_ )
		SynChan::processFunc( c, &p );
	ret = get< double >( syn, "Gk", dret );
	// cout << "t = " << p.currTime_ << " dret = " << dret << endl;
	ASSERT( fabs( dret - 0.825  ) < 1e-3 , "Testing SynChan response " );

	// At 1 msec it should be at the peak, almost exactly 1.
	for ( ; p.currTime_ < 0.002; p.currTime_ += p.dt_ )
		SynChan::processFunc( c, &p );
	ret = get< double >( syn, "Gk", dret );
	// cout << "t = " << p.currTime_ << " dret = " << dret << endl;
	ASSERT( fabs( dret - 1.0  ) < 2e-3 , "Testing SynChan response" );

	// At 2 msec it is down to 70% of peak.
	for ( ; p.currTime_ < 0.003; p.currTime_ += p.dt_ )
		SynChan::processFunc( c, &p );
	ret = get< double >( syn, "Gk", dret );
	// cout << "t = " << p.currTime_ << " dret = " << dret << endl;
	ASSERT( fabs( dret - 0.7  ) < 1e-3 , "Testing SynChan response" );

	// At 3 msec it is down to 38% of peak.
	for ( ; p.currTime_ < 0.004; p.currTime_ += p.dt_ )
		SynChan::processFunc( c, &p );
	ret = get< double >( syn, "Gk", dret );
	// cout << "t = " << p.currTime_ << " dret = " << dret << endl;
	ASSERT( fabs( dret - 0.38  ) < 1e-3 , "Testing SynChan response" );

	// Go on to next peak.
	for ( ; p.currTime_ < 0.011; p.currTime_ += p.dt_ )
		SynChan::processFunc( c, &p );
	ret = get< double >( syn, "Gk", dret );
	// cout << "t = " << p.currTime_ << ", dret = " << dret << endl;
	ASSERT( fabs( dret - 1.0  ) < 2e-3 , "Testing SynChan response 2" );

	////////////////////////////////////////////////////////////////
	// Clear it all up
	////////////////////////////////////////////////////////////////
	set( n, "destroy" );
}
#endif 
