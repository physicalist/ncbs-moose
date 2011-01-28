/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Tick.h"
#include "TickMgr.h"
#include "TickPtr.h"
#include "Clock.h"
#include "testScheduling.h"

#include <queue>
#include "../biophysics/Synapse.h"
#include "../biophysics/IntFire.h"
#include "MsgManager.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "SingleMsg.h"
#include "../builtins/Arith.h"
#include "../randnum/randnum.h"

pthread_mutex_t TestSched::mutex_;
bool TestSched::isInitPending_( 1 );
int TestSched::globalIndex_( 0 );

/**
 * Used to test Tick operation specially with the new process design, where
 * the entire clock sequence is done one tick at a time in parallel from
 * the multithread process loop.
 * Note that ticks cannot be made independently; they are sub-elements
 * of the Clock.
 */
void testTicks()
{
	Eref sheller( Id().eref() );
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	vector< unsigned int > dims( 1, 1 );
	Id clockId = shell->doCreate( "Clock", Id(), "tclock", dims );
	Id tickId = Id( clockId.value() + 1 );
	Id arithId = shell->doCreate( "Arith", clockId, "arith", dims );
	ProcInfo p;

	MsgId m1 = shell->doAddMsg( "Single", 
		FullId( tickId, 0 ), "proc0", FullId( arithId, 0 ), "proc" );
	// Qinfo::clearQ( &p );
	assert( m1 != Msg::badMsg );

	vector< Id > msgDests;
	const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( 
		Tick::initCinfo()->findFinfo( "process0" ) );
	assert( sf );
	tickId()->getOutputs( msgDests, sf );
	assert( msgDests.size() == 1 );
	assert( msgDests[0] == arithId );

	bool ret = Field< double >::set( tickId.eref(), "dt", 5.0);
	assert( ret );

	Tick* t0 = reinterpret_cast< Tick* >( tickId.eref().data() );
	assert( doubleEq( 5.0, t0->dt_ ) );

	// By default it gets hard-code set to the systiem tick Element, so
	// we need to reassign it here for this test.
	t0->setElement( tickId() ); 

	Arith* a0 = reinterpret_cast< Arith* >( arithId.eref().data() );
	a0->arg1_ = 123.4;
	a0->arg2_ = 7;
	a0->arg3_ = 9;

	t0->reinit( &p );
	assert( doubleEq( 0, a0->arg1_ ) );
	assert( doubleEq( 0, a0->arg2_ ) );
	assert( doubleEq( 0, a0->arg3_ ) );

	a0->arg1_ = 123.4;
	a0->arg2_ = 7;
	a0->arg3_ = 9;

	t0->advance( &p );
	assert( doubleEq( 123.4, a0->arg1_ ) );
	assert( doubleEq( 7, a0->arg2_ ) );
	assert( doubleEq( 0, a0->arg3_ ) );
	assert( doubleEq( 139.4, a0->output_ ) );

	const Msg* m = Msg::getMsg( m1 );
	assert( m != 0 );
	assert( tickId() != 0 );
	assert( clockId() != 0 );
	clockId.destroy();
	assert( tickId() != 0 );
	tickId.destroy();
	arithId.destroy();
	assert( clockId() == 0 );
	assert( tickId() == 0 );
	m = Msg::getMsg( m1 );
	assert( m == 0 );

	cout << "." << flush;
}


//////////////////////////////////////////////////////////////////////
// Setting up a class for testing scheduling.
//////////////////////////////////////////////////////////////////////

static DestFinfo processFinfo( "process",
	"handles process call",
	new ProcOpFunc< TestSched >( &TestSched::process ) );
const Cinfo* TestSched::initCinfo()
{

	static Finfo* testSchedFinfos[] = {
		&processFinfo
	};

	static Cinfo testSchedCinfo (
		"testSched",
		0,
		testSchedFinfos,
		sizeof ( testSchedFinfos ) / sizeof( Finfo* ),
		new Dinfo< TestSched >()
	);

	return &testSchedCinfo;
}

static const Cinfo* testSchedCinfo = TestSched::initCinfo();

void TestSched::process( const Eref& e, ProcPtr p )
{
	static const int timings[] = { 1, 2, 2, 2, 3, 3, 4, 4, 4, 
		5, 5, 5, 6, 6, 6, 6, 7, 8, 8, 8, 9, 9, 10, 10, 10, 10, 10,
		11, 12, 12, 12, 12, 13, 14, 14, 14, 15, 15, 15, 15,
		16, 16, 16, 17, 18, 18, 18, 18, 19, 20, 20, 20, 20, 20 };
	// unsigned int max = sizeof( timings ) / sizeof( int );
	// cout << Shell::myNode() << ":" << p->threadIndexInGroup << " : timing[ " << index_ << ", " << p->threadId << " ] = " << timings[ index_ ] << ", time = " << p->currTime << endl;
	if ( static_cast< int >( p->currTime ) != timings[ index_ ] ) {
		cout << Shell::myNode() << ":" << p->threadIndexInGroup << " :testThreadSchedElement::process: index= " << index_ << ", numThreads = " <<
			p->numThreads << ", currTime = " << p->currTime << endl;
	}

	assert( static_cast< int >( p->currTime ) == timings[ index_ ] );
	++index_;
	/*
	assert( static_cast< int >( p->currTime ) == 	
		timings[ index_ / p->numThreadsInGroup ] );
		*/

	// Check that everything remains in sync across threads.
	pthread_mutex_lock( &mutex_ );
		assert( ( globalIndex_ - index_ )*( globalIndex_ - index_ ) <= 1 );
		if ( p->threadIndexInGroup == 0 )
			globalIndex_ = index_;
	pthread_mutex_unlock( &mutex_ );

	// assert( index_ <= max * p->numThreads );
	// cout << index_ << ": " << p->currTime << endl;
}

//////////////////////////////////////////////////////////////////////

/**
 * Check that the ticks are set up properly, created and destroyed as
 * needed, and are sorted when dts are assigned
 */
void setupTicks()
{
	static const double EPSILON = 1.0e-9;
	const double runtime = 20.0;
	// const Cinfo* tc = Tick::initCinfo();
	Id clock = Id::nextId();
	vector< unsigned int > dims( 1, 1 );
	Element* clocke = new Element( clock, Clock::initCinfo(), "tclock",
		dims, 1 );
	assert( clocke );
	// bool ret = Clock::initCinfo()->create( clock, "tclock", 1 );
	// assert( ret );
	// Element* clocke = clock();
	Eref clocker = clock.eref();
	Id tickId( clock.value() + 1 );
	Element* ticke = tickId();
	assert( ticke->getName() == "tick" );

	unsigned int size = 10;

	for ( unsigned int i = 0; i < size; ++i ) {
		Eref er( ticke, DataId( 0, i ) );
		reinterpret_cast< Tick* >( er.data() )->setElement( ticke );
	}

	// cout << Shell::myNode() << ": numTicks: " << ticke->dataHandler()->totalEntries() << ", " << size << endl;
	assert( ticke->dataHandler()->localEntries() == size );

	Eref er0( ticke, DataId( 0, 2 ) );
	bool ret = Field< double >::set( er0, "dt", 5.0);
	assert( ret );
	Eref er1( ticke, DataId( 0, 1 ) );
	ret = Field< double >::set( er1, "dt", 2.0);
	assert( ret );
	Eref er2( ticke, DataId( 0, 0 ) );
	ret = Field< double >::set( er2, "dt", 2.0);
	assert( ret );
	Eref er3( ticke, DataId( 0, 3 ) );
	ret = Field< double >::set( er3, "dt", 1.0);
	assert( ret );
	Eref er4( ticke, DataId( 0, 4 ) );
	ret = Field< double >::set( er4, "dt", 3.0);
	assert( ret );
	// Note that here I put the tick on a different DataId. later it gets
	// to sit on the appropriate Conn, when the SingleMsg is set up.
	Eref er5( ticke, DataId( 0, 7 ) );
	ret = Field< double >::set( er5, "dt", 5.0);
	assert( ret );

	Clock* cdata = reinterpret_cast< Clock* >( clocker.data() );
	assert( cdata->tickPtr_.size() == 4 );
	assert( fabs( cdata->tickPtr_[0].mgr()->dt_ - 1.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[1].mgr()->dt_ - 2.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[2].mgr()->dt_ - 3.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[3].mgr()->dt_ - 5.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[0].mgr()->nextTime_ - 1.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[1].mgr()->nextTime_ - 2.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[2].mgr()->nextTime_ - 3.0 ) < EPSILON );
	assert( fabs( cdata->tickPtr_[3].mgr()->nextTime_ - 5.0 ) < EPSILON );
	assert( cdata->tickPtr_[0].mgr()->ticks_.size() == 1 );
	assert( cdata->tickPtr_[1].mgr()->ticks_.size() == 2 );
	assert( cdata->tickPtr_[2].mgr()->ticks_.size() == 1 );
	assert( cdata->tickPtr_[3].mgr()->ticks_.size() == 2 );

	assert( cdata->tickPtr_[0].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er3.data() ) );
	assert( cdata->tickPtr_[1].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er2.data() ) );
	assert( cdata->tickPtr_[1].mgr()->ticks_[1] == reinterpret_cast< const Tick* >( er1.data() ) );
	assert( cdata->tickPtr_[2].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er4.data() ) );
	assert( cdata->tickPtr_[3].mgr()->ticks_[0] == reinterpret_cast< const Tick* >( er0.data() ) );
	assert( cdata->tickPtr_[3].mgr()->ticks_[1] == reinterpret_cast< const Tick* >( er5.data() ) );

	Id tsid = Id::nextId();
	Element* tse = new Element( tsid, testSchedCinfo, "tse", dims, 1 );

	Eref ts( tse, 0 );
	
	FuncId f( processFinfo.getFid() );
	const Finfo* proc0 = ticke->cinfo()->findFinfo( "process0" );
	assert( proc0 );
	const SrcFinfo* sproc0 = dynamic_cast< const SrcFinfo* >( proc0 );
	assert( sproc0 );
	unsigned int b0 = sproc0->getBindIndex();
	SingleMsg *m0 = new SingleMsg( er0, ts ); 
	er0.element()->addMsgAndFunc( m0->mid(), f, 0 + b0 );
	SingleMsg *m1 = new SingleMsg( er1, ts ); 
	er1.element()->addMsgAndFunc( m1->mid(), f, 2 + b0 );
	SingleMsg *m2 = new SingleMsg( er2, ts );
	er2.element()->addMsgAndFunc( m2->mid(), f, 4 + b0 );
	SingleMsg *m3 = new SingleMsg( er3, ts ); 
	er3.element()->addMsgAndFunc( m3->mid(), f, 6 + b0 );
	SingleMsg *m4 = new SingleMsg( er4, ts ); 
	er4.element()->addMsgAndFunc( m4->mid(), f, 8 + b0 );
	SingleMsg *m5 = new SingleMsg( er5, ts ); 
	er5.element()->addMsgAndFunc( m5->mid(), f, 14 + b0 );

	cdata->rebuild();

	ProcInfo p;
	cdata->handleReinit();
	assert( Clock::procState_ == Clock::TurnOnReinit ); 
	cdata->reinitPhase1( &p );
	assert( Clock::procState_ == Clock::TurnOnReinit ); 
	cdata->reinitPhase2( &p );
	assert( Clock::procState_ == Clock::TurnOffReinit ); 
	Clock::procState_ = Clock::NoChange;
	assert( cdata->doingReinit_ == 0 );

	cdata->handleStart( runtime );

	// Normally flipRunning_ signals the system to flip the isRunning flag
	assert( Clock::procState_ == Clock::StartOnly ); 
	Clock::procState_ = Clock::NoChange;

	while ( Clock::procState_ == Clock::NoChange ) {
		cdata->advancePhase1( &p );
		cdata->advancePhase2( &p );
	}
	Clock::procState_ = Clock::NoChange;

	assert( doubleEq( cdata->getCurrentTime(), runtime ) );
	// Get rid of pending events in the queues.
	Qinfo::clearQ( &p );
	Qinfo::clearQ( &p );

	tickId.destroy();
	clock.destroy();
	tsid.destroy();
	cout << "." << flush;
}

void testThreads()
{
	Element* se = Id()();
	Shell* s = reinterpret_cast< Shell* >( se->dataHandler()->data( 0 ) );
	s->doSetClock( 0, 5.0 );
	s->doSetClock( 1, 2.0 );
	s->doSetClock( 2, 2.0 );
	s->doSetClock( 3, 1.0 );
	s->doSetClock( 4, 3.0 );
	s->doSetClock( 5, 5.0 );

	vector< unsigned int > dims;
	dims.push_back( 7 ); // A suitable number to test dispatch of Process calls during threading.
	Id tsid = Id::nextId();
	Element* tse = new Element( tsid, testSchedCinfo, "tse", dims, 1 );
	// testThreadSchedElement tse;
	Eref ts( tse, 0 );
	Element* ticke = Id( 2 )();
	Eref er0( ticke, DataId( 0, 0 ) );
	Eref er1( ticke, DataId( 0, 1 ) );
	Eref er2( ticke, DataId( 0, 2 ) );
	Eref er3( ticke, DataId( 0, 3 ) );
	Eref er4( ticke, DataId( 0, 4 ) );
	Eref er5( ticke, DataId( 0, 5 ) );

	const Finfo* proc0 = ticke->cinfo()->findFinfo( "process0" );
	assert( proc0 );
	const SrcFinfo* sproc0 = dynamic_cast< const SrcFinfo* >( proc0 );
	assert( sproc0 );
	unsigned int b0 = sproc0->getBindIndex();
	/*
	const Finfo* proc1 = ticke->cinfo()->findFinfo( "process1" );
	const SrcFinfo* sproc1 = dynamic_cast< const SrcFinfo* >( proc1 );
	const Finfo* proc2 = ticke->cinfo()->findFinfo( "process2" );
	const SrcFinfo* sproc2 = dynamic_cast< const SrcFinfo* >( proc2 );
	unsigned int b1 = sproc1->getBindIndex();
	unsigned int b2 = sproc2->getBindIndex();
	*/
	FuncId f( processFinfo.getFid() );
	SingleMsg* m0 = new SingleMsg( er0, ts );
	er0.element()->addMsgAndFunc( m0->mid(), f, 0 + b0 );
	SingleMsg* m1 = new SingleMsg( er1, ts );
	er1.element()->addMsgAndFunc( m1->mid(), f, 2 + b0 );
	SingleMsg* m2 = new SingleMsg( er2, ts );
	er2.element()->addMsgAndFunc( m2->mid(), f, 4 + b0 );
	SingleMsg* m3 = new SingleMsg( er3, ts );
	er3.element()->addMsgAndFunc( m3->mid(), f, 6 + b0 );
	SingleMsg* m4 = new SingleMsg( er4, ts );
	er4.element()->addMsgAndFunc( m4->mid(), f, 8 + b0 );
	SingleMsg* m5 = new SingleMsg( er5, ts );
	er5.element()->addMsgAndFunc( m5->mid(), f, 10 + b0 );
	s->start( 10 );

	// Qinfo::mergeQ( 0 ); // Need to clean up stuff.

	// cout << "Done TestThreads" << flush;
	tsid.destroy();
	cout << "." << flush;
}

void testThreadIntFireNetwork()
{
	// Known value from single-thread run, at t = 1 sec.
	// These are the old values
	static const double Vm100 = 0.0857292;
	static const double Vm900 = 0.107449;
	// static const double Vm100 = 0.10124059893763067;
	// static const double Vm900 = 0.091409481280996352;
	static const unsigned int NUMSYN = 104576;
	static const double thresh = 0.2;
	static const double Vmax = 1.0;
	static const double refractoryPeriod = 0.4;
	static const double weightMax = 0.02;
	static const double delayMax = 4;
	static const double timestep = 0.2;
	static const double connectionProbability = 0.1;
	static const unsigned int runsteps = 5;
	// static const unsigned int runsteps = 1000;
	const Cinfo* ic = IntFire::initCinfo();
	// const Cinfo* sc = Synapse::initCinfo();
	unsigned int size = 1024;
	string arg;

	// Qinfo::mergeQ( 0 );

	mtseed( 5489UL ); // The default value, but better to be explicit.

	Id i2 = Id::nextId();
	// bool ret = ic->create( i2, "test2", size );
	vector< unsigned int > dims( 1, size );
	Element* t2 = new Element( i2, ic, "test2", dims );
	assert( t2 );

	Eref e2 = i2.eref();
	// FieldElement< Synapse, IntFire, &IntFire::synapse > syn( sc, i2(), &IntFire::getNumSynapses, &IntFire::setNumSynapses );
	Id synId( i2.value() + 1 );
	Element* syn = synId();
	assert( syn->getName() == "synapse" );

	assert( syn->dataHandler()->totalEntries() == size );
	assert( syn->dataHandler()->localEntries() == 0 );

	DataId di( 1, 0 ); // DataId( data, field )
	Eref syne( syn, di );

	/*
	unsigned int numThreads = 1;
	if ( Qinfo::numSimGroup() >= 2 ) {
		numThreads = Qinfo::simGroup( 1 )->numThreads;
	}
	*/
	/*
	bool ret = SparseMsg::add( e2.element(), "spike", syn, "addSpike", 
		connectionProbability, numThreads ); // Include group id as an arg. 
	assert( ret );
	*/
	SparseMsg* sm = new SparseMsg( e2.element(), syn );
	assert( sm );
	const Finfo* f1 = ic->findFinfo( "spike" );
	const Finfo* f2 = Synapse::initCinfo()->findFinfo( "addSpike" );
	assert( f1 && f2 );
	f1->addMsg( f2, sm->mid(), t2 );
	sm->randomConnect( connectionProbability );
	// sm->loadBalance( numThreads );

	unsigned int nd = syn->dataHandler()->localEntries();
//	cout << "Num Syn = " << nd << endl;
	assert( nd == NUMSYN );
	vector< double > initVm( size, 0.0 );
	for ( unsigned int i = 0; i < size; ++i )
		initVm[i] = mtrand() * Vmax;

	double initVm100 = initVm[100];
	double initVm900 = initVm[900];

	bool ret;

	vector< double > temp( size, thresh );
	ret = Field< double >::setVec( e2, "thresh", temp );
	assert( ret );
	temp.clear();
	temp.resize( size, refractoryPeriod );
	ret = Field< double >::setVec( e2, "refractoryPeriod", temp );
	assert( ret );
	FieldDataHandlerBase* fd = dynamic_cast< FieldDataHandlerBase *>(
		syn->dataHandler() );
	assert( fd );
	unsigned int fieldSize = fd->biggestFieldArraySize();
	fd->setFieldDimension( fieldSize );
	vector< double > weight( size * fieldSize, 0.0 );
	vector< double > delay( size * fieldSize, 0.0 );
	unsigned int numTotSyn = 0;
	for ( unsigned int i = 0; i < size; ++i ) {
		unsigned int numSyn = fd->getFieldArraySize( i );
		unsigned int k = i * fieldSize;
		for ( unsigned int j = 0; j < numSyn; ++j ) {
			weight[ k + j ] = mtrand() * weightMax;
			delay[ k + j ] = mtrand() * delayMax;
			++numTotSyn;
		}
	}
	assert ( numTotSyn == nd );
	/*
	vector< double > weight;
	weight.reserve( nd );
	vector< double > delay;
	delay.reserve( nd );
	for ( unsigned int i = 0; i < size; ++i ) {
		unsigned int numSyn = syne.element()->dataHandler()->getFieldArraySize( i );
		for ( unsigned int j = 0; j < numSyn; ++j ) {
			weight.push_back( mtrand() * weightMax );
			delay.push_back( mtrand() * delayMax );
		}
	}
	assert( syne.element()->dataHandler()->totalEntries() == weight.size());
	*/

	ret = Field< double >::setVec( syne, "weight", weight );
	assert( ret );
	ret = Field< double >::setVec( syne, "delay", delay );
	assert( ret );


	Element* ticke = Id( 2 )();
	Eref er0( ticke, DataId( 0, 0 ) );

	SingleMsg* m = new SingleMsg( er0, e2 );
	const Finfo* p1 = Tick::initCinfo()->findFinfo( "process0" );
	const Finfo* p2 = ic->findFinfo( "process" );
	ret = p1->addMsg( p2, m->mid(), ticke );

	// ret = SingleMsg::add( er0, "process0", e2, "process" );
	assert( ret );

	// printGrid( i2(), "Vm", 0, thresh );
	Element* se = Id()();
	Shell* s = reinterpret_cast< Shell* >( se->dataHandler()->data( 0 ) );
	s->doSetClock( 0, timestep );
	s->doReinit();

	ret = Field< double >::setVec( e2, "Vm", initVm );
	assert( ret );

	IntFire* ifire100 = reinterpret_cast< IntFire* >( e2.element()->dataHandler()->data( 100 ) );
	IntFire* ifire900 = reinterpret_cast< IntFire* >( e2.element()->dataHandler()->data( 900 ) );

	assert( doubleEq( ifire100->getVm(), initVm100 ) );
	assert( doubleEq( ifire900->getVm(), initVm900 ) );

	s->doStart( timestep * runsteps );

	assert( doubleEq( ifire100->getVm(), Vm100 ) );
	assert( doubleEq( ifire900->getVm(), Vm900 ) );

	// cout << "Done ThreadIntFireNetwork" << flush;
	cout << "." << flush;
	// delete i2();
	synId.destroy();
	i2.destroy();
	// synId.destroy();
}

void testMultiNodeIntFireNetwork()
{
	// Known value from single-thread run, at t = 1 sec.
	static const double Vm100 = 0.0857292;
	static const double Vm900 = 0.107449;
	// static const double Vm100 = 0.10124059893763067;
	// static const double Vm900 = 0.091409481280996352;
	// static const unsigned int NUMSYN = 104576;
	static const double thresh = 0.2;
	static const double Vmax = 1.0;
	static const double refractoryPeriod = 0.4;
	static const double weightMax = 0.02;
	static const double delayMax = 4;
	static const double timestep = 0.2;
	static const double connectionProbability = 0.1;
	static const unsigned int runsteps = 5;
	// These are the starting indices of synapses on
	// IntFire[0], [100], [200], ...
	/*
	static unsigned int synIndices[] = {
		0, 10355, 20696, 30782, 41080,
		51226, 61456, 71579, 81765, 92060,
		102178,
	};
	*/
	// static const unsigned int runsteps = 1000;
	// const Cinfo* ic = IntFire::initCinfo();
	// const Cinfo* sc = Synapse::initCinfo();
	unsigned int size = 1024;
	string arg;
	Eref sheller( Id().eref() );
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	// Qinfo::mergeQ( 0 );

	//mtseed( 5489UL ); // The default value, but better to be explicit.

	vector< unsigned int > dims( 1, size );
	Id i2 = shell->doCreate( "IntFire", Id(), "test2", dims );
	assert( i2()->getName() == "test2" );
	Eref e2 = i2.eref();

	// FieldElement< Synapse, IntFire, &IntFire::synapse > syn( sc, i2(), &IntFire::getNumSynapses, &IntFire::setNumSynapses );
	Id synId( i2.value() + 1 );
	Element* syn = synId();
	assert( syn->getName() == "synapse" );

	assert( syn->dataHandler()->totalEntries() == size );
	assert( syn->dataHandler()->localEntries() == 0 );

	DataId di( 1, 0 ); // DataId( data, field )
	Eref syne( syn, di );

	/*
	unsigned int numThreads = 1;
	if ( Qinfo::numSimGroup() >= 2 ) {
		numThreads = Qinfo::simGroup( 1 )->numThreads;
	}
	*/

	MsgId mid = shell->doAddMsg( "Sparse", e2.fullId(), "spike",
		FullId( synId, 0 ), "addSpike" );
	
	const Msg* m = Msg::getMsg( mid );
	assert( m );
	Eref mer = m->manager( m->id() );
	assert( mer.element() );

	SetGet2< double, long >::set( mer, "setRandomConnectivity", 
		connectionProbability, 5489UL );

	FieldDataHandlerBase * fdh =
		static_cast< FieldDataHandlerBase *>( syn->dataHandler() );
	fdh->syncFieldArraySize();
	assert( fdh->biggestFieldArraySize() == 134 );

	/*
	SetGet1< unsigned int >::set( mer, "loadBalance", numThreads ); 
	vector< unsigned int > synArraySizes;
	unsigned int start = syn->dataHandler()->getNumData2( synArraySizes );
	// cout << "start = " << start << endl;
	unsigned int synIndex = start;
	for ( unsigned int i = 0; i < size; ++i ) {
		// if ( ( i % 100 ) == 0 ) cout << "i = " << i << "SynIndex = " << synIndex << endl;
		synIndex += synArraySizes[i];
	}
	*/

	unsigned int nd = syn->dataHandler()->localEntries();
	assert( syn->dataHandler()->totalEntries() == size * 134 );
	assert( nd == 104576 );
	// cout << "Num Syn = " << nd << endl;
	// nd = 104576;

	// Here we have an interesting problem. The mtRand might be called
	// by multiple threads if the above Set call is not complete.
	// usleep( 1000000);

	// This fails on multinodes.
	// assert( nd == NUMSYN );
	vector< double > temp( size, 0.0 );
	for ( unsigned int i = 0; i < size; ++i )
		temp[i] = mtrand() * Vmax;

	double origVm100 = temp[100];
	double origVm900 = temp[900];

	bool ret = Field< double >::setVec( e2, "Vm", temp );
	assert( ret );

	temp.clear();
	temp.resize( size, thresh );
	ret = Field< double >::setVec( e2, "thresh", temp );
	assert( ret );
	temp.clear();
	temp.resize( size, refractoryPeriod );
	ret = Field< double >::setVec( e2, "refractoryPeriod", temp );
	assert( ret );

	FieldDataHandlerBase* fd = dynamic_cast< FieldDataHandlerBase* >(
		syne.element()->dataHandler() );
	assert( fd );
	unsigned int fieldSize = fd->biggestFieldArraySize();
	fd->setFieldDimension( fieldSize );
	vector< double > weight( size * fieldSize, 0.0 );
	vector< double > delay( size * fieldSize, 0.0 );
	unsigned int numTotSyn = 0;
	for ( unsigned int i = 0; i < size; ++i ) {
		unsigned int numSyn = fd->getFieldArraySize( i );
		unsigned int k = i * fieldSize;
		for ( unsigned int j = 0; j < numSyn; ++j ) {
			weight[ k + j ] = mtrand() * weightMax;
			delay[ k + j ] = mtrand() * delayMax;
			++numTotSyn;
		}
	}
	assert ( numTotSyn == nd );

	/*
	vector< double > weight;
	weight.reserve( nd );
	vector< double > delay;
	delay.reserve( nd );
	for ( unsigned int i = 0; i < nd; ++i ) {
		weight.push_back( mtrand() * weightMax );
		delay.push_back( mtrand() * delayMax );
	}
	*/
	ret = Field< double >::setVec( syne, "weight", weight );
	assert( ret );
	ret = Field< double >::setVec( syne, "delay", delay );
	assert( ret );

	for ( unsigned int i = 0; i < size; i+= 100 ) {
		double wt = Field< double >::get( 
			Eref( syne.element(), DataId( i, 0 ) ), "weight" );
		assert( doubleEq( wt, weight[ i * fieldSize ] ) );
		// assert( doubleEq( wt, weight[ synIndices[ i / 100 ] ] ) );
		// cout << "Actual wt = " << wt << ", expected = " << weight[ synIndices[ i / 100 ] ] << endl;
	}

	Element* ticke = Id( 2 )();
	Eref er0( ticke, DataId( 0, 0 ) );

	shell->doAddMsg( "Single", er0.fullId(), "process0",
		e2.fullId(), "process" );
	shell->doSetClock( 0, timestep );
	shell->doReinit();

	double retVm100 = Field< double >::get( Eref( e2.element(), 100 ), "Vm" );
	double retVm900 = Field< double >::get( Eref( e2.element(), 900 ), "Vm" );
	assert( fabs( retVm100 - origVm100 ) < 1e-6 );
	assert( fabs( retVm900 - origVm900 ) < 1e-6 );

	shell->doStart( static_cast< double >( timestep * runsteps) + 0.0 );
	retVm100 = Field< double >::get( Eref( e2.element(), 100 ), "Vm" );
	retVm900 = Field< double >::get( Eref( e2.element(), 900 ), "Vm" );

	// cout << "MultiNodeIntFireNetwork: Vm100 = " << retVm100 << ", " << Vm100 << "; Vm900 = " << retVm900 << ", " << Vm900 << endl;
	assert( fabs( retVm100 - Vm100 ) < 1e-6 );
	assert( fabs( retVm900 - Vm900 ) < 1e-6 );

	cout << "." << flush;
	// shell->doDelete( synId );
	shell->doDelete( i2 );
}
	
void speedTestMultiNodeIntFireNetwork( unsigned int size, unsigned int runsteps )
{
	static const double thresh = 0.1;
	static const double Vmax = 1.0;
	static const double refractoryPeriod = 0.4;
	static const double weightMax = 0.1;
	static const double delayMax = 4;
	static const double timestep = 0.2;
	static const double connectionProbability = 0.2;
	/*
	// Applicable for size == 1024
	static const double testVm[] = {
		-3.41353,
		-2.50449,
		-2.64486,
		-0.387621,
		-1.08778,
		-1e-07,
		-0.187895,
		-0.716069,
		-1e-07,
		-0.0155043,
		-1e-07
	};
	// Applicable for size == 2048
	static const double testVm[] = {
		-1e-07,
		-0.0505845,
		0.0456282,
		-5.61229,
		-1e-07,
		-1e-07,
		-2.72,
		-0.0441219,
		-1e-07,
		-1e-07,
		-1e-07,
		-2.4886,
		-3.89123,
		-0.479408,
		-2.84331,
		-2.7225,
		-1e-07,
		-1.74236,
		-0.35803,
		-1.89033,
		-1.81688
	};
	*/

	string arg;
	Eref sheller( Id().eref() );
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dims( 1, size );
	Id i2 = shell->doCreate( "IntFire", Id(), "test2", dims );
	assert( i2()->getName() == "test2" );
	Eref e2 = i2.eref();

	Id synId( i2.value() + 1 );
	Element* syn = synId();
	assert( syn->getName() == "synapse" );

	assert( syn->dataHandler()->totalEntries() == 0 );

	DataId di( 1, 0 ); // DataId( data, field )
	Eref syne( syn, di );

	/*
	unsigned int numThreads = 1;
	if ( Qinfo::numSimGroup() >= 2 ) {
		numThreads = Qinfo::simGroup( 1 )->numThreads;
	}
	*/

	MsgId mid = shell->doAddMsg( "Sparse", e2.fullId(), "spike",
		FullId( synId, 0 ), "addSpike" );
	
	const Msg* m = Msg::getMsg( mid );
	Eref mer = m->manager( m->id() );

	SetGet2< double, long >::set( mer, "setRandomConnectivity", 
		connectionProbability, 5489UL );

	// SetGet1< unsigned int >::set( mer, "loadBalance", numThreads ); 
	vector< unsigned int > synArraySizes;
	// unsigned int start = syn->dataHandler()->getNumData2( synArraySizes );

	unsigned int nd = syn->dataHandler()->totalEntries();
	nd = 104576;

	vector< double > temp( size, 0.0 );
	for ( unsigned int i = 0; i < size; ++i )
		temp[i] = mtrand() * Vmax;

	bool ret = Field< double >::setVec( e2, "Vm", temp );
	assert( ret );

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
	for ( unsigned int i = 0; i < nd; ++i ) {
		weight.push_back( 2.0 * ( mtrand() - 0.5 ) * weightMax );
		delay.push_back( mtrand() * delayMax );
	}
	ret = Field< double >::setVec( syne, "weight", weight );
	assert( ret );
	ret = Field< double >::setVec( syne, "delay", delay );
	assert( ret );

	Element* ticke = Id( 2 )();
	Eref er0( ticke, DataId( 0, 0 ) );

	shell->doAddMsg( "Single", er0.fullId(), "process0",
		e2.fullId(), "process" );
	shell->doSetClock( 0, timestep );

	shell->doStart( static_cast< double >( timestep * runsteps) + 0.0 );

	/*
	for ( unsigned int i = 0; i < size; i += 100 ) {
		double ret = Field< double >::get( Eref( e2.element(), i ), "Vm" );
		if ( fabs( ret - testVm[i/100] ) > 1e-5 )
			cout << "1000 step IntFire test Error: " << i << ": got = " 
				<< ret << ", expected = " << testVm[i/100] << endl;
	}
	*/

	cout << "." << flush;
	shell->doDelete( synId );
	shell->doDelete( i2 );
	shell->doQuit();
}
void testScheduling()
{
	testTicks();
	setupTicks();
}

void testSchedulingProcess()
{
	testThreads();
	testThreadIntFireNetwork();
	testMultiNodeIntFireNetwork();
}

void testMpiScheduling()
{
	testMultiNodeIntFireNetwork();
}
