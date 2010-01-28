/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Message.h"
#include "Tick.h"
#include "TickPtr.h"
#include "Clock.h"

#include <queue>
#include "../biophysics/Synapse.h"
#include "../biophysics/IntFire.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "PsparseMsg.h"
#include "../randnum/randnum.h"

class testSchedElement: public Element
{
	public:
		testSchedElement() 
			: Element( Tick::initCinfo(), 0, 0, 0, 0, 0 ), index_( 0 )
		{;}
		
		void process( const ProcInfo* p ) {
			static const int timings[] = { 1, 2, 2, 2, 3, 3, 4, 4, 4, 
				5, 5, 5, 6, 6, 6, 6, 7, 8, 8, 8, 9, 9, 10, 10, 10, 10, 10,
				11, 12, 12, 12, 12, 13, 14, 14, 14, 15, 15, 15, 15,
				16, 16, 16, 17, 18, 18, 18, 18, 19, 20, 20, 20, 20, 20 };
			unsigned int max = sizeof( timings ) / sizeof( int );
			// cout << "timing[ " << index_ << " ] = " << timings[ index_ ] << ", time = " << p->currTime << endl;
			assert( static_cast< int >( p->currTime ) == 	
				timings[ index_++ ] );
			assert( index_ <= max );
			// cout << index_ << ": " << p->currTime << endl;
		}
	private:
		unsigned int index_;
};

class testThreadSchedElement: public Element
{
	public:
		testThreadSchedElement() 
			: Element( Tick::initCinfo(), 0, 0, 0, 0, 0 ), index_( 0 )
		{ 
			pthread_mutex_init( &mutex_, NULL );
		}

		~testThreadSchedElement()
		{
			pthread_mutex_destroy( &mutex_ );
		}
		
		void process( const ProcInfo* p ) {
			static const int timings[] = { 1, 2, 2, 2, 3, 3, 4, 4, 4, 
				5, 5, 5, 6, 6, 6, 6, 7, 8, 8, 8, 9, 9, 10, 10, 10, 10, 10,
				11, 12, 12, 12, 12, 13, 14, 14, 14, 15, 15, 15, 15,
				16, 16, 16, 17, 18, 18, 18, 18, 19, 20, 20, 20, 20, 20 };
			unsigned int max = sizeof( timings ) / sizeof( int );
			// cout << "timing[ " << index_ << ", " << p->threadId << " ] = " << timings[ index_ / p->numThreads ] << ", time = " << p->currTime << endl;
			assert( static_cast< int >( p->currTime ) == 	
				timings[ index_ / p->numThreads ] );

			pthread_mutex_lock( &mutex_ );
				++index_;
			pthread_mutex_unlock( &mutex_ );

			assert( index_ <= max * p->numThreads );
			// cout << index_ << ": " << p->currTime << endl;
		}
	private:
		pthread_mutex_t mutex_;
		unsigned int index_;
};

/**
 * Check that the ticks are set up properly, created and destroyed as
 * needed, and are sorted when dts are assigned
 */
void setupTicks()
{
	static const double EPSILON = 1.0e-9;
	const Cinfo* tc = Tick::initCinfo();
	Id clock = Clock::initCinfo()->create( "tclock", 1 );
	Element* clocke = clock();
	Eref clocker = clock.eref();
	FieldElement< Tick, Clock, &Clock::getTick > ticke( tc, clocke, 
		&Clock::getNumTicks, &Clock::setNumTicks );
	unsigned int size = 10;

	bool ret = OneToAllMsg::add( clocker, "tick", &ticke, "parent" );
	assert( ret );

	assert( ticke.numData() == 0 );
	ret = Field< unsigned int >::set( clocker, "numTicks", size );
	assert( ret );
	assert( ticke.numData() == size );

	Eref er0( &ticke, DataId( 0, 2 ) );
	ret = Field< double >::set( er0, "dt", 5.0);
	assert( ret );
	ret = Field< unsigned int >::set( er0, "stage", 0);
	assert( ret );
	Eref er1( &ticke, DataId( 0, 1 ) );
	ret = Field< double >::set( er1, "dt", 2.0);
	assert( ret );
	ret = Field< unsigned int >::set( er1, "stage", 0);
	assert( ret );
	Eref er2( &ticke, DataId( 0, 0 ) );
	ret = Field< double >::set( er2, "dt", 2.0);
	assert( ret );
	ret = Field< unsigned int >::set( er2, "stage", 1);
	assert( ret );
	Eref er3( &ticke, DataId( 0, 3 ) );
	ret = Field< double >::set( er3, "dt", 1.0);
	assert( ret );
	ret = Field< unsigned int >::set( er3, "stage", 0);
	assert( ret );
	Eref er4( &ticke, DataId( 0, 4 ) );
	ret = Field< double >::set( er4, "dt", 3.0);
	assert( ret );
	ret = Field< unsigned int >::set( er4, "stage", 5);
	assert( ret );
	// Note that here I put the tick on a different DataId. later it gets
	// to sit on the appropriate Conn, when the SingleMsg is set up.
	Eref er5( &ticke, DataId( 0, 7 ) );
	ret = Field< double >::set( er5, "dt", 5.0);
	assert( ret );
	ret = Field< unsigned int >::set( er5, "stage", 1);
	assert( ret );

	Clock* cdata = reinterpret_cast< Clock* >( clocker.data() );
	assert( cdata->tickPtr_.size() == 4 );
	assert( fabs( cdata->tickPtr_[0].dt_ - 1.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[1].dt_ - 2.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[2].dt_ - 3.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[3].dt_ - 5.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[0].nextTime_ - 1.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[1].nextTime_ - 2.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[2].nextTime_ - 3.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[3].nextTime_ - 5.0 ) < EPSILON );
	assert( cdata->tickPtr_[0].ticks_.size() == 1 );
	assert( cdata->tickPtr_[1].ticks_.size() == 2 );
	assert( cdata->tickPtr_[2].ticks_.size() == 1 );
	assert( cdata->tickPtr_[3].ticks_.size() == 2 );

	assert( cdata->tickPtr_[0].ticks_[0] == reinterpret_cast< const Tick* >( er3.data() ) );
	assert( cdata->tickPtr_[1].ticks_[0] == reinterpret_cast< const Tick* >( er1.data() ) );
	assert( cdata->tickPtr_[1].ticks_[1] == reinterpret_cast< const Tick* >( er2.data() ) );
	assert( cdata->tickPtr_[2].ticks_[0] == reinterpret_cast< const Tick* >( er4.data() ) );
	assert( cdata->tickPtr_[3].ticks_[0] == reinterpret_cast< const Tick* >( er0.data() ) );
	assert( cdata->tickPtr_[3].ticks_[1] == reinterpret_cast< const Tick* >( er5.data() ) );


	testSchedElement tse;
	Eref ts( &tse, 0 );
	
	SingleMsg m0( er0, ts ); er0.element()->addMsgToConn( m0.mid(), 0 );
	SingleMsg m1( er1, ts ); er1.element()->addMsgToConn( m1.mid(), 1 );
	SingleMsg m2( er2, ts ); er2.element()->addMsgToConn( m2.mid(), 2 );
	SingleMsg m3( er3, ts ); er3.element()->addMsgToConn( m3.mid(), 3 );
	SingleMsg m4( er4, ts ); er4.element()->addMsgToConn( m4.mid(), 4 );
	SingleMsg m5( er5, ts ); er5.element()->addMsgToConn( m5.mid(), 7 );

	Qinfo q( 0, 0, 8 );
	cdata->start( clocker, &q, 20 );

	cout << "." << flush;

	delete clocke;
}

void testThreads()
{
	Element* se = Id()();
	Shell* s = reinterpret_cast< Shell* >( se->data( 0 ) );
	s->setclock( 0, 5.0, 0 );
	s->setclock( 1, 2.0, 0 );
	s->setclock( 2, 2.0, 1 );
	s->setclock( 3, 1.0, 0 );
	s->setclock( 4, 3.0, 5 );
	s->setclock( 5, 5.0, 1 );

	testThreadSchedElement tse;
	Eref ts( &tse, 0 );

	/*
	// The useclock function needs to set up messages that call suitable
	// sub-parts of the elements for each thread.
	// Here we hard-code a lot of stuff that can be simplified when the
	// Element parent/child framework is in place.
	Eref ce = Id( 1, 0 ).eref();
	Eref ticke = Id( 2, 0 ).eref();
	// Clock* cdata = reinterpret_cast< Clock* >( ce.data() );
	FieldElement< Tick, Clock, &Clock::getTick > ticke( tc, clocke, 
		&Clock::getNumTicks, &Clock::setNumTicks );
	unsigned int size = 10;

	bool ret = OneToAllMsg::add( clocker, "tick", &ticke, "parent" );
	assert( ret );

	assert( ticke.numData() == 0 );
	ret = Field< unsigned int >::set( clocker, "numTicks", size );
	assert( ret );
	assert( ticke.numData() == size );
	*/
	Element* ticke = Id( 2, 0 )();
	Eref er0( ticke, DataId( 0, 0 ) );
	Eref er1( ticke, DataId( 0, 1 ) );
	Eref er2( ticke, DataId( 0, 2 ) );
	Eref er3( ticke, DataId( 0, 3 ) );
	Eref er4( ticke, DataId( 0, 4 ) );
	Eref er5( ticke, DataId( 0, 5 ) );

	SingleMsg m0( er0, ts ); er0.element()->addMsgToConn( m0.mid(), 0 );
	SingleMsg m1( er1, ts ); er1.element()->addMsgToConn( m1.mid(), 1 );
	SingleMsg m2( er2, ts ); er2.element()->addMsgToConn( m2.mid(), 2 );
	SingleMsg m3( er3, ts ); er3.element()->addMsgToConn( m3.mid(), 3 );
	SingleMsg m4( er4, ts ); er4.element()->addMsgToConn( m4.mid(), 4 );
	SingleMsg m5( er5, ts ); er5.element()->addMsgToConn( m5.mid(), 5 );
	s->start( 10 );

	cout << "." << flush;
}

void testThreadIntFireNetwork()
{
	// Need another way to validate simulation output.
	static const unsigned int qSize[] =
		{ 838, 10, 6, 18, 36, 84, 150, 196, 258, 302 };
	static const unsigned int NUMSYN = 104576;
	static const double thresh = 0.2;
	static const double Vmax = 1.0;
	static const double refractoryPeriod = 0.4;
	static const double weightMax = 0.02;
	static const double delayMax = 4;
	static const double timestep = 0.2;
	static const double connectionProbability = 0.1;
	static const unsigned int runsteps = 5;
	const Cinfo* ic = IntFire::initCinfo();
	const Cinfo* sc = Synapse::initCinfo();
	unsigned int size = 1024;
	string arg;

	mtseed( 5489UL ); // The default value, but better to be explicit.

	Id i2 = ic->create( "test2", size );
	Eref e2 = i2.eref();
	FieldElement< Synapse, IntFire, &IntFire::synapse > syn( sc, i2(), &IntFire::getNumSynapses, &IntFire::setNumSynapses );

	assert( syn.numData() == 0 );

	DataId di( 1, 0 ); // DataId( data, field )
	Eref syne( &syn, di );

	unsigned int numThreads = 1;
	if ( Qinfo::numSimGroup() >= 2 ) {
		numThreads = Qinfo::simGroup( 1 )->numThreads;
	}
	bool ret = PsparseMsg::add( e2.element(), "spike", &syn, "addSpike", 
		connectionProbability, numThreads ); // Include group id as an arg. 
	assert( ret );

	unsigned int nd = syn.numData();
//	cout << "Num Syn = " << nd << endl;
	assert( nd == NUMSYN );
	vector< double > temp( size, 0.0 );
	for ( unsigned int i = 0; i < size; ++i )
		temp[i] = mtrand() * Vmax;

	ret = Field< double >::setVec( e2, "Vm", temp );
	assert( ret );
	/*
	for ( unsigned int i = 0; i < 40; ++i )
		cout << reinterpret_cast< IntFire* >( e2.element()->data( i ) )->getVm() << "	" << temp[i] << endl;
		*/
	temp.clear();
	temp.resize( size, thresh );
	ret = Field< double >::setVec( e2, "thresh", temp );
	assert( ret );
	temp.clear();
	temp.resize( size, refractoryPeriod );
	ret = Field< double >::setVec( e2, "refractoryPeriod", temp );
	assert( ret );

	vector< double > weight;
	weight.reserve( nd );
	vector< double > delay;
	delay.reserve( nd );
	for ( unsigned int i = 0; i < size; ++i ) {
		unsigned int numSyn = syne.element()->numData2( i );
		for ( unsigned int j = 0; j < numSyn; ++j ) {
			weight.push_back( mtrand() * weightMax );
			delay.push_back( mtrand() * delayMax );
		}
	}
	ret = Field< double >::setVec( syne, "weight", weight );
	assert( ret );
	ret = Field< double >::setVec( syne, "delay", delay );
	assert( ret );

	// printGrid( i2(), "Vm", 0, thresh );
	Element* se = Id()();
	Shell* s = reinterpret_cast< Shell* >( se->data( 0 ) );
	s->setclock( 0, timestep, 0 );
	cerr << "starting\n";

	Element* ticke = Id( 2, 0 )();
	Eref er0( ticke, DataId( 0, 0 ) );

	ret = SingleMsg::add( er0, "process", e2, "process" );
	assert( ret );
	/*
	SingleMsg m0( er0, e2 ); 
	er0.element()->addMsgToConn( m0.mid(), 0 );
	*/
	s->start( timestep * runsteps );
	cerr << "ending\n";

	cout << "." << flush;
	delete i2();
}
	

void testScheduling( )
{
	setupTicks();
	testThreads();
	testThreadIntFireNetwork();
}
