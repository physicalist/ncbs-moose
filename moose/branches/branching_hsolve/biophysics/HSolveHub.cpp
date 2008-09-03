/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "../element/Neutral.h"
#include "Compartment.h"
#include "HHChannel.h"
#include "SpikeGen.h"
#include "SynInfo.h"        // for SynChanStruct. Remove eventually.
#include <queue>            // for SynChanStruct. Remove eventually.
#include "HSolveStruct.h"   // for SynChanStruct. Remove eventually.
#include "SynChan.h"
#include "ThisFinfo.h"
#include "SolveFinfo.h"
#include "HSolveHub.h"

const Cinfo* initHSolveHubCinfo()
{
	/**
	 * This is identical to the message sent from clock Ticks to
	 * objects. Here it is used to take over the Process message,
	 * usually only as a handle from the solver to the object.
	 */
	static Finfo* zombieShared[] =
	{
		new SrcFinfo( "process", Ftype1< ProcInfo >::global() ),
		new SrcFinfo( "reinit", Ftype1< ProcInfo >::global() ),
	};
	
	/**
	 * This is the destination of the several messages from the integrator.
	 */
	static Finfo* integShared[] =
	{
		new DestFinfo( "compartment",
			Ftype2< vector< double >*, vector< Element* >* >::global(),
			RFCAST( &HSolveHub::compartmentFunc ) ),
		new DestFinfo( "channel",
			Ftype1< vector< Element* >* >::global(),
			RFCAST( &HSolveHub::channelFunc ) ),
		new DestFinfo( "spikegen",
			Ftype1< vector< Element* >* >::global(),
			RFCAST( &HSolveHub::spikegenFunc ) ),
		new DestFinfo( "synchan",
			Ftype1< vector< Element* >* >::global(),
			RFCAST( &HSolveHub::synchanFunc ) ),
	};
	
	static Finfo* HSolveHubFinfos[] =
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////
	// MsgDest definitions
	///////////////////////////////////////////////////////
		new DestFinfo( "destroy", Ftype0::global(),
			&HSolveHub::destroy ),
		// override the Neutral::childFunc here, so that when this
		// is deleted all the zombies are reanimated.
		new DestFinfo( "child", Ftype1< int >::global(),
			RFCAST( &HSolveHub::childFunc ) ),
	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////
		new SharedFinfo( "integ-hub", integShared, 
			sizeof( integShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "compartmentSolve", zombieShared, 
			sizeof( zombieShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "channelSolve", zombieShared, 
			sizeof( zombieShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "spikegenSolve", zombieShared, 
			sizeof( zombieShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "synchanSolve", zombieShared, 
			sizeof( zombieShared ) / sizeof( Finfo* ) ),
	};
	
	static Cinfo HSolveHubCinfo(
		"HSolveHub",
		"Niraj Dudani, 2007, NCBS",
		"HSolveHub: ",
		initNeutralCinfo(),
		HSolveHubFinfos,
		sizeof( HSolveHubFinfos ) / sizeof( Finfo* ),
		ValueFtype1< HSolveHub >::global()
	);
	
	return &HSolveHubCinfo;
}

static const Cinfo* HSolveHubCinfo = initHSolveHubCinfo();

static const Finfo* compartmentSolveFinfo = 
	initHSolveHubCinfo()->findFinfo( "compartmentSolve" );
static const Finfo* channelSolveFinfo = 
	initHSolveHubCinfo()->findFinfo( "channelSolve" );
static const Finfo* spikegenSolveFinfo = 
	initHSolveHubCinfo()->findFinfo( "spikegenSolve" );
static const Finfo* synchanSolveFinfo = 
	initHSolveHubCinfo()->findFinfo( "synchanSolve" );

/////////////////////////////////////////////////////////////////////////
// Replacement fields for aspiring zombies
/////////////////////////////////////////////////////////////////////////

Finfo* initCompartmentZombieFinfo()
{
	/* 
	 * These fields will replace the original compartment fields so that
	 * the lookups refer to the solver rather than the compartment.
	 */
	static Finfo* compartmentFields[] =
	{
		new ValueFinfo( "Vm",
			ValueFtype1< double >::global(),
			GFCAST( &HSolveHub::getCompartmentVm ),
			RFCAST( &HSolveHub::setCompartmentVm )
		),
		new ValueFinfo( "Em",
			ValueFtype1< double >::global(),
			GFCAST( &HSolveHub::getCompartmentEm ),
			RFCAST( &HSolveHub::setCompartmentEm )
		),
		new ValueFinfo( "Im",
			ValueFtype1< double >::global(),
			GFCAST( &HSolveHub::getCompartmentIm ),
			&dummyFunc
		),
		new ValueFinfo( "inject",
			ValueFtype1< double >::global(),
			GFCAST( &HSolveHub::getCompartmentInject ),
			RFCAST( &HSolveHub::setCompartmentInject )
		),
	};

	static const ThisFinfo* tf = dynamic_cast< const ThisFinfo* >( 
		initCompartmentCinfo()->getThisFinfo( ) );
	assert( tf != 0 );

	static SolveFinfo compartmentZombieFinfo( 
		compartmentFields, 
		sizeof( compartmentFields ) / sizeof( Finfo* ),
		tf
	);

	return &compartmentZombieFinfo;
}

Finfo* initChannelZombieFinfo()
{
	static Finfo* channelFields[] =
	{
		new ValueFinfo( "Gbar", ValueFtype1< double >::global(),
			//~ GFCAST( &HSolveHub::getChannelGbar ), 
			//~ RFCAST( &HSolveHub::setChannelGbar )
			GFCAST( &HHChannel::getGbar ), 
			RFCAST( &HHChannel::setGbar )
		),
	};

	static const ThisFinfo* tf = dynamic_cast< const ThisFinfo* >( 
		initHHChannelCinfo()->getThisFinfo( ) );
	assert( tf != 0 );

	static SolveFinfo channelZombieFinfo( 
		channelFields, 
		sizeof( channelFields ) / sizeof( Finfo* ),
		tf
	);

	return &channelZombieFinfo;
}

Finfo* initSynChanZombieFinfo()
{
	static Finfo* synchanFields[] =
	{
		new ValueFinfo( "Gbar", ValueFtype1< double >::global(),
			GFCAST( &HSolveHub::getSynChanGbar ), 
			RFCAST( &HSolveHub::setSynChanGbar )
		),
	};

	static const ThisFinfo* tf = dynamic_cast< const ThisFinfo* >( 
		initSynChanCinfo()->getThisFinfo( ) );
	assert( tf != 0 );

	static SolveFinfo synchanZombieFinfo( 
		synchanFields, 
		sizeof( synchanFields ) / sizeof( Finfo* ),
		tf
	);

	return &synchanZombieFinfo;
}

static Finfo* compartmentZombieFinfo = initCompartmentZombieFinfo();
static Finfo* channelZombieFinfo = initChannelZombieFinfo();
static Finfo* synchanZombieFinfo = initSynChanZombieFinfo();

/////////////////////////////////////////////////////////////////////////
// End of static initializers.
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////
HSolveHub::HSolveHub( )
	: V_( 0 ), state_( 0 )
{ ; }

/////////////////////////////////////////////////////////////////////////
// Field access functions (for Hub)
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dest functions (for Hub)
/////////////////////////////////////////////////////////////////////////

/**
 * In this destructor we need to put messages back to process,
 * and we need to replace the SolveFinfos on zombies with the
 * original ThisFinfo. This should really just use the clearFunc.
 */
void HSolveHub::destroy( const Conn* c )
{
	static Finfo* origCompartmentFinfo =
		const_cast< Finfo* >(
			initCompartmentCinfo()->getThisFinfo( ) );
	static Finfo* origChannelFinfo =
		const_cast< Finfo* >(
			initHHChannelCinfo()->getThisFinfo( ) );
	static Finfo* origSpikeGenFinfo =
		const_cast< Finfo* >(
			initSpikeGenCinfo()->getThisFinfo( ) );
	static Finfo* origSynChanFinfo =
		const_cast< Finfo* >(
			initSynChanCinfo()->getThisFinfo( ) );
	
	Element* hub = c->target().e;
	unsigned int eIndex = c->target().i;

	Conn* i = hub->targets( compartmentSolveFinfo->msg(), eIndex );
	while ( i->good() ) {
		i->target().e->setThisFinfo( origCompartmentFinfo );
		i->increment();
	}
	delete i;

	i = hub->targets( channelSolveFinfo->msg(), eIndex );
	while ( i->good() ) {
		i->target().e->setThisFinfo( origChannelFinfo );
		i->increment();
	}
	delete i;

	i = hub->targets( spikegenSolveFinfo->msg(), eIndex );
	while ( i->good() ) {
		i->target().e->setThisFinfo( origSpikeGenFinfo );
		i->increment();
	}
	delete i;

	i = hub->targets( synchanSolveFinfo->msg(), eIndex );
	while ( i->good() ) {
		i->target().e->setThisFinfo( origSynChanFinfo );
		i->increment();
	}
	delete i;

	Neutral::destroy( c );
}

void HSolveHub::childFunc( const Conn* c, int stage )
{
	// clear messages: first clean out zombies before the messages are
	// all deleted.
	if ( stage == 1 )
		clearFunc( c->target() );
	
	// Then fall back into what the Neutral version does
	Neutral::childFunc( c, stage );
}

void HSolveHub::compartmentFunc(
	const Conn* c,
	vector< double >* V,
	vector< Element* >* elist )
{
	static_cast< HSolveHub* >( c->data() )->
		innerCompartmentFunc( c->target(), V, elist );
}

void HSolveHub::innerCompartmentFunc(
	Eref hub,
	vector< double >* V,
	vector< Element* >* elist )
{
	V_ = V;
	
	//~ // for redirecting inject messages
	//~ const Finfo* injectFinfo = initCompartmentCinfo()->findFinfo( "inject" );
	const Finfo* initFinfo = initCompartmentCinfo()->findFinfo( "init" );
	
	// Note that here we have perfect alignment between the order of the
	// V_ vector and the elist vector. This is used implicitly in the
	// ordering of the process messages that get set up between the Hub
	// and the objects.
	vector< Element* >::iterator i;
	for ( i = elist->begin(); i != elist->end(); ++i ) {
		zombify( hub, *i, compartmentSolveFinfo, compartmentZombieFinfo );
		// Compartment receives 2 shared messages from Tick's "process"
		Eref( *i ).dropAll( initFinfo->msg() );
		redirectDynamicMessages( *i );
	}
	
	//~ for ( i = elist->begin(); i != elist->end(); i++ ) {
		//~ // Here we replace the injectMessages from outside the tree.
		//~ // The 'retain' flag at the end is 1: we do not want to delete
		//~ // the original message to the compartment.
		//~ redirectDestMessages( hub, *i, molSumFinfo, sumTotFinfo, 
			//~ i - elist->begin(), molSumMap_, elist, 1 );
	//~ }
}

void HSolveHub::channelFunc( const Conn* c, vector< Element* >* elist )
{
	static_cast< HSolveHub* >( c->data() )->
		innerChannelFunc( c->target(), elist );
}

void HSolveHub::innerChannelFunc( Eref hub, vector< Element* >* elist )
{
	vector< Element* >::iterator i;
	for ( i = elist->begin(); i != elist->end(); ++i ) {
		zombify( hub, *i, channelSolveFinfo, channelZombieFinfo );
		//~ redirectDynamicMessages( *i );
	}
}

void HSolveHub::spikegenFunc( const Conn* c, vector< Element* >* elist )
{
	static_cast< HSolveHub* >( c->data() )->
		innerSpikegenFunc( c->target(), elist );
}

void HSolveHub::innerSpikegenFunc( Eref hub, vector< Element* >* elist )
{
	vector< Element* >::iterator i;
	for ( i = elist->begin(); i != elist->end(); ++i ) {
//		zombify( hub, *i, spikegenSolveFinfo, spikegenZombieFinfo );
		redirectDynamicMessages( *i );
	}
}

void HSolveHub::synchanFunc( const Conn* c, vector< Element* >* elist )
{
	static_cast< HSolveHub* >( c->data() )->
		innerSynchanFunc( c->target(), elist );
}

void HSolveHub::innerSynchanFunc( Eref hub, vector< Element* >* elist )
{
	vector< Element* >::iterator i;
	for ( i = elist->begin(); i != elist->end(); ++i ) {
		zombify( hub, *i, synchanSolveFinfo, synchanZombieFinfo );
		redirectDynamicMessages( *i );
	}
}

/////////////////////////////////////////////////////////////////////////
// Class functions
/////////////////////////////////////////////////////////////////////////
/**
 * Clears out all the messages to zombie objects
 */
void HSolveHub::clearFunc( Eref hub )
{
	clearMsgsFromFinfo( hub, compartmentSolveFinfo );
	clearMsgsFromFinfo( hub, channelSolveFinfo );
	clearMsgsFromFinfo( hub, spikegenSolveFinfo );
	clearMsgsFromFinfo( hub, synchanSolveFinfo );

	//~ hub.dropAll( injectFinfo->msg() );
}

void HSolveHub::clearMsgsFromFinfo( Eref hub, const Finfo * f )
{
	Conn* c = hub.e->targets( f->msg(), hub.i );
	vector< Element* > list;
	while ( c->good() ) {
		list.push_back( c->target().e );
		c->increment();
	}
	delete c;
	hub.dropAll( f->msg() );
	
	vector< Element* >::iterator i;
	for ( i = list.begin(); i != list.end(); i++ ) unzombify( *i );
}

void HSolveHub::unzombify( Element* e )
{
	const Cinfo* ci = e->cinfo();
	bool ret = ci->schedule( e, ConnTainer::Default );
	assert( ret );
	e->setThisFinfo( const_cast< Finfo* >( ci->getThisFinfo() ) );
	redirectDynamicMessages( e );
}

/**
 * This operation turns the target element e into a zombie controlled
 * by the hub/solver. It gets rid of any process message coming into 
 * the zombie and replaces it with one from the solver.
 */
void HSolveHub::zombify( 
	Eref hub, Eref e,
	const Finfo* hubFinfo, Finfo* solveFinfo )
{
	// Replace the original procFinfo with one from the hub.
	const Finfo* procFinfo = e->findFinfo( "process" );
	e.dropAll( procFinfo->msg() );
	bool ret = hub.add( hubFinfo->msg(), e, procFinfo->msg(), 
		ConnTainer::Default );
	assert( ret );

	// Redirect original messages from the zombie to the hub.
	// Pending.

	// Replace the 'ThisFinfo' on the solved element
	e->setThisFinfo( solveFinfo );
}

/**
 * This function redirects messages arriving at zombie elements onto
 * the hub. 
 * e is the zombie element whose messages are being redirected to the hub.
 * eFinfo is the Finfo holding those messages.
 * hubFinfo is the Finfo on the hub which will now handle the messages.
 * eIndex is the index to look up the element.
*/
void HSolveHub::redirectDestMessages(
	Eref hub, Eref e, const Finfo* hubFinfo, const Finfo* eFinfo,
	unsigned int eIndex, vector< unsigned int >& map, 
	vector< Element *>* elist, bool retain )
{
	Conn* i = e.e->targets( eFinfo->msg(), e.i );
	vector< Eref > srcElements;
	vector< int > srcMsg;
	vector< const ConnTainer* > dropList;

	while( i->good() ) {
		Element* tgt = i->target().e;
		// Handle messages going outside purview of solver.
		if ( find( elist->begin(), elist->end(), tgt ) == elist->end() ) {
			map.push_back( eIndex );
			srcElements.push_back( i->target() );
			srcMsg.push_back( i->targetMsg() );
			if ( !retain )
				dropList.push_back( i->connTainer() );
		}
		i->increment();
	}
	delete i;

	e.dropVec( eFinfo->msg(), dropList );

	for ( unsigned int j = 0; j != srcElements.size(); j++ ) {
		bool ret = srcElements[j].add( srcMsg[j], hub, hubFinfo->msg(),
			ConnTainer::Default );
		assert( ret );
	}
}

/**
 * Here we replace the existing DynamicFinfos and their messages with
 * new ones for the updated access functions.
 *
 * It would be nice to retain everything and only replace the 
 * access functions, but this gets too messy as it requires poking the
 * new funcVecs into the remote Msgs. So instead we delete the 
 * old DynamicFinfos and recreate them.
 */
// Assumption e is a simple element. Replace it with Eref to make it general
void HSolveHub::redirectDynamicMessages( Element* e )
{
	vector< Finfo* > flist;
	// We get a list of DynamicFinfos independent of the Finfo vector on 
	// the Element, because we will be messing up the iterators on the
	// element.
	e->listLocalFinfos( flist );
	vector< Finfo* >::iterator i;

	// Go through flist noting messages, deleting finfo, and rebuilding.
	for( i = flist.begin(); i != flist.end(); ++i )
	{
		const DynamicFinfo *df = dynamic_cast< const DynamicFinfo* >( *i );
		assert( df != 0 );
		vector< Eref > srcElements;
		vector< const Finfo* > srcFinfos;
		Conn* c = e->targets( ( *i )->msg(), 0 ); //zero index for SE

		// note messages.
		while( c->good() ) {
			srcElements.push_back( c->target() );
			srcFinfos.push_back( 
				c->target().e->findFinfo( c->targetMsg() ) );
			c->increment();
		}
		delete c;
		string name = df->name();
		bool ret = e->dropFinfo( df );
		assert( ret );
		const Finfo* origFinfo = e->findFinfo( name );
		assert( origFinfo );

		unsigned int max = srcFinfos.size();
		for ( unsigned int i =  0; i < max; i++ ) {
			ret = srcElements[ i ].add( srcFinfos[ i ]->name(),
				e, name );
			/*
			ret = srcElements[ i ].add( srcFinfos[ i ]->msg(),
				e, origFinfo->msg(), ConnTainer::Default );
			*/
			// ret = srcFinfos[ i ]->add( srcElements[ i ], e, origFinfo );
			assert( ret );
		}
	}
}

/**
 * Looks up the solver from the zombie element e. Returns the solver
 * element, or null on failure. 
 * It needs the originating Finfo on the solver that connects to the zombie,
 * as the srcFinfo.
 * Also passes back the index of the zombie element on this set of
 * messages. This is NOT the absolute Conn index.
 */
HSolveHub* HSolveHub::getHubFromZombie( Eref e, unsigned int& index )
{
	Conn* c = e.e->targets( "process", e.i );
	if ( c->good() ) {
		index = c->targetIndex();
		HSolveHub* nh = static_cast< HSolveHub* >( c->target().data() );
		c->increment();
		assert( !c->good() ); // Should only be one process incoming.
		return dynamic_cast< HSolveHub* >( nh );
	}
	delete c;
	return 0;
}

/////////////////////////////////////////////////////////////////////////
// Field access functions (Biophysics)
/////////////////////////////////////////////////////////////////////////

/**
 * Here we provide the zombie function to set the 'Vm' field of the 
 * compartment. It first sets the solver location handling this
 * field, then the compartment itself.
 * For the compartment set/get operations, the lookup order is identical
 * to the message order. So we don't need an intermediate table.
 */
void HSolveHub::setCompartmentVm( const Conn* c, double value )
{
	unsigned int comptIndex;
	HSolveHub* nh = getHubFromZombie( c->target(), comptIndex );
	if ( nh ) {
		assert ( comptIndex < nh->V_->size() );
		( *nh->V_ )[ comptIndex ] = value;
	}
}

double HSolveHub::getCompartmentVm( Eref e )
{
	unsigned int comptIndex;
	HSolveHub* nh = getHubFromZombie( e, comptIndex );
	if ( nh ) {
		assert ( comptIndex < nh->V_->size() );
		return ( *nh->V_ )[ comptIndex ];
	}
	return 0.0;
}

void HSolveHub::setCompartmentEm( const Conn* c, double value )
{
	unsigned int comptIndex;
	HSolveHub* nh = getHubFromZombie( c->target(), comptIndex );
	if ( nh ) {
		assert ( comptIndex < nh->V_->size() );
		( *nh->V_ )[ comptIndex ] = value;
	}
}

double HSolveHub::getCompartmentEm( Eref e )
{
	unsigned int comptIndex;
	HSolveHub* nh = getHubFromZombie( e, comptIndex );
	if ( nh ) {
		assert ( comptIndex < nh->V_->size() );
		return ( *nh->V_ )[ comptIndex ];
	}
	return 0.0;
}

double HSolveHub::getCompartmentIm( Eref e )
{
	unsigned int comptIndex;
	HSolveHub* nh = getHubFromZombie( e, comptIndex );
	if ( nh ) {
		assert ( comptIndex < nh->V_->size() );
		return ( *nh->V_ )[ comptIndex ];
	}
	return 0.0;
}

void HSolveHub::setCompartmentInject( const Conn* c, double value )
{
	//~ unsigned int comptIndex;
	//~ HSolveHub* nh = getHubFromZombie( 
		//~ c.targetElement(), comptIndex );
	//~ if ( nh ) {
		//~ assert ( comptIndex < nh->inject_.size() );
		//~ ( nh->inject_ )[ comptIndex ] = value;
	//~ }
}

double HSolveHub::getCompartmentInject( Eref e )
{
	//~ unsigned int comptIndex;
	//~ HSolveHub* nh = getHubFromZombie( e, comptIndex );
	//~ if ( nh ) {
		//~ assert ( comptIndex < nh->inject_.size() );
		//~ return ( nh->inject_ )[ comptIndex ];
	//~ }
	//~ return 0.0;
	return 0.0;
}

void HSolveHub::setChannelGbar( const Conn* c, double value )
{
	;
}

double HSolveHub::getChannelGbar( Eref e )
{
	return 0.0;
}

void HSolveHub::setSynChanGbar( const Conn* c, double value )
{
	;
}

double HSolveHub::getSynChanGbar( Eref e )
{
	return 0.0;
}

/////////////////////////////////////////////////////////////////////////
// Dest functions (Biophysics)
/////////////////////////////////////////////////////////////////////////
void HSolveHub::comptInjectMsgFunc( const Conn* c, double I )
{
/*
	Compartment* compt = static_cast< Compartment* >(
		c.targetElement()->data() );
	compt->sumInject_ += I;
	compt->Im_ += I;
*/
}

