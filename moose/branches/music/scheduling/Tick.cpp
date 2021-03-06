/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "moose.h"
#include "Tick.h"

/**
 * The Tick handles the nuts and bolts of scheduling. It sends the
 * Process (or other event) message to all the scheduled objects,
 * and it keeps track of the update sequence with its sibling Tick
 * objects.
 *
 * \todo: How to handle the 'path' assignment for all targets?
 * \todo: What to do about the ProcInfo argument to some of the
 * tick functions?
 * Are we sharing a ProcInfo between multiple Ticks, or can it be
 * stored locally?
 */

const Cinfo* initTickCinfo()
{
	/**
	 * This is a shared message that connects up to the next 
	 * Tick in the sequence. It invokes its incrementTick
	 * function and also manages various functions for reset
	 * and return values. It is meant to handle only a
	 * single target.
	 */
	static Finfo* nextShared[] = 
	{
		// This first entry is for the incrementTick function
		new SrcFinfo( "increment", Ftype2< ProcInfo, double >::global() ),
		// The second entry is a request to send nextTime_ from the next
		// tick to the current one. 
		new SrcFinfo( "nextTimeSrc", Ftype0::global() ),
		// The third entry is for receiving the nextTime_ value
		// from the following tick.
		new DestFinfo( "nextTime", Ftype1< double >::global(), 
			RFCAST( &Tick::receiveNextTime ) ),
		// The third one is for propagating resched forward.
		new SrcFinfo( "resched", Ftype0::global() ),
		// The fourth one is for propagating reinit forward.
		new SrcFinfo( "reinit", Ftype1< ProcInfo >::global() ),
	};

	/**
	 * This is the mirror of the previous shared message, it receives
	 * this message from the preceding tick. Again, should only have
	 * a singe target.
	 */
	static Finfo* prevShared[] = 
	{
		// This first entry is for the incrementTick function
		new DestFinfo( "increment", Ftype2< ProcInfo, double >::global(), 
			RFCAST( &Tick::incrementTick ) ),
		// The second entry handles requests to send nextTime_ back
		// to the previous tick.
		new DestFinfo( "nextTime", Ftype0::global(), 
			RFCAST( &Tick::handleNextTimeRequest ) ),
		// The third entry sends nextTime_ value
		// to the previous tick.
		new SrcFinfo( "nextTimeSrc", Ftype1< double >::global() ),
		// The fourth entry is for receiving the resched call
		new DestFinfo( "resched", Ftype0::global(), 
			RFCAST( &Tick::resched ) ),
		// The fifth one is for receiving the reinit call.
		new DestFinfo( "reinit", Ftype1< ProcInfo >::global(), 
			RFCAST( &Tick::reinit ) ),
	};

	/**
	 * This goes to all scheduled objects to call their process events.
	 */
	static Finfo* processShared[] = 
	{
		// The process function call
		new SrcFinfo( "process", Ftype1< ProcInfo >::global() ),
		// The reinit function call
		new SrcFinfo( "reinit", Ftype1< ProcInfo >::global() ),
	};

	static Finfo* tickFinfos[] =
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		new ValueFinfo( "dt", ValueFtype1< double >::global(),
			GFCAST( &Tick::getDt ),
			RFCAST( &Tick::setDt )
		),
		new ValueFinfo( "stage", ValueFtype1< int >::global(),
			GFCAST( &Tick::getStage ), 
			RFCAST( &Tick::setStage )
		),
		new ValueFinfo( "ordinal", ValueFtype1< int >::global(),
			GFCAST( &Tick::getOrdinal ),
			&dummyFunc
		),
		new ValueFinfo( "nextTime", ValueFtype1< double >::global(),
			GFCAST( &Tick::getNextTime ), 
			&dummyFunc
		),
		new ValueFinfo( "path", ValueFtype1< string >::global(),
			GFCAST( &Tick::getPath ),
			RFCAST( &Tick::setPath )
		),
	///////////////////////////////////////////////////////
	// Shared message definitions
	///////////////////////////////////////////////////////
		new SharedFinfo( "next", nextShared, 
			sizeof( nextShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "prev", prevShared, 
			sizeof( prevShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "process", processShared, 
			sizeof( processShared ) / sizeof( Finfo* ) ),
	
	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
		/**
		 * The process message does the main business of the Tick:
		 * Sends out the Process call to all objects scheduled at
		 * this tick.
		 */
		new SrcFinfo( "processSrc", Ftype1< ProcInfo >::global() ),
		/**
		 * The updateDt message goes to the parent ClockJob and
		 * initiates a resched, because when dt changes the order
		 * of the ticks needs to be updated.
		 */
		new SrcFinfo( "updateDtSrc", Ftype1< double >::global() ),

	///////////////////////////////////////////////////////
	// MsgDest definitions
	///////////////////////////////////////////////////////
		/**
		 * newObject handles addition of new objects to the list on
		 * this tick. I don't know if it is needed. Arguments are
		 * object id and process field name
		new DestFinfo( "newObject",
			Ftype2< unsigned int, string >::global(),
			RFCAST( &Tick::schedNewObject ) ),
		 */
		/**
		 * The start function sets of a simulation to run for 
		 * the specified runtime.
		 */
		new DestFinfo( "start",
			Ftype2< ProcInfo, double >::global(),
			RFCAST( &Tick::start ) ),
	};
	
	static Cinfo tickCinfo(
		"Tick",
		"Upinder S. Bhalla, Mar 2007, NCBS",
		"Tick: Tick class. Controls execution of objects on a given dt.",
		initNeutralCinfo(),
		tickFinfos,
		sizeof(tickFinfos)/sizeof(Finfo *),
		ValueFtype1< Tick >::global()
	);

	return &tickCinfo;
}

static const Cinfo* tickCinfo = initTickCinfo();

static const Slot nextSlot = initTickCinfo()->getSlot( "next.increment" );
static const Slot requestNextTimeSlot = 
	initTickCinfo()->getSlot( "next.nextTimeSrc" );
static const Slot reschedSlot = initTickCinfo()->getSlot( "next.resched" );
static const Slot reinitNextSlot = 
	initTickCinfo()->getSlot( "next.reinit" );

static const Slot returnNextTimeSlot = 
	initTickCinfo()->getSlot( "prev.nextTimeSrc" );
	
static const Slot updateDtSlot = initTickCinfo()->getSlot( "updateDt" );
static const Slot processSlot = 
	initTickCinfo()->getSlot( "process.process" );
static const Slot reinitSlot = initTickCinfo()->getSlot( "process.reinit" );

///////////////////////////////////////////////////
// Tick class definition functions
///////////////////////////////////////////////////

bool Tick::operator<( const Tick& other ) const
{
	if ( dt_ < other.dt_ ) return 1;
		if ( dt_ == other.dt_ && stage_ < other.stage_ )
			return 1;
	return 0;
}


bool Tick::operator==( const Tick& other ) const
{
	return ( dt_ == other.dt_ && stage_ == other.stage_ );
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

/**
 * This is called when dt is set on the local Tick.
 * We first fix up local values for nextTime and dt, then
 * we ask the parent ClockJob to re-sort the clock ticks to
 * put them back in order.
 */
void Tick::setDt( const Conn* c, double newdt )
{
	Tick* t = static_cast< Tick* >( c->data() );
	t->nextTime_ += newdt - t->dt_;
	t->dt_ = newdt;
	send0( c->target(), updateDtSlot );
}
/**
 * The getDt just looks up the local dt, much less involved than
 * the setDt function.
 */
double Tick::getDt( Eref e )
{
	return static_cast< Tick* >( e.data() )->dt_;
}

/**
 * This is called when stage is set on the local Tick.
 * Like the setDt, it has to ask the parent ClockJob to
 * re-sort the clock ticks to put them back in order.
 */
void Tick::setStage( const Conn* c, int v )
{
	
	static_cast< Tick* >( c->data() )->stage_ = v;
	send0( c->target(), updateDtSlot );
}

/**
 * The getStage just looks up the local stage, much less involved than
 * the setStage function.
 */
int Tick::getStage( Eref e )
{
	return static_cast< Tick* >( e.data() )->stage_;
}

/**
 * The getOrdinal function returns the ordinal number which has 
 * something to do with which node we are on, if I remember.
 * Not sure what it does.
 */
int Tick::getOrdinal( Eref e )
{
	return static_cast< Tick* >( e.data() )->ordinal_;
}

/**
 * nextTime is here to peek into when the tick is due to fire next.
 * Not clear if it should become private.
 */
double Tick::getNextTime( Eref e )
{
	return static_cast< Tick* >( e.data() )->nextTime_;
}

/**
 * set and get Path are problematic. Their goal is to assign the 
 * targets for this Tick. As framed, they fit in with the older
 * GENESIS syntax. For now, put in dummy functions.
 * \todo How to do this operation with the new MOOSE syntax?
 */
void Tick::setPath( const Conn* c, string v )
{
	static_cast< Tick* >( c->data() )->path_ = v;
}
string Tick::getPath( Eref e )
{
	return static_cast< Tick* >( e.data() )->path_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

/**
 * The receiveNextTime message comes from the 'next' tick, as a return
 * message when we need to update the nextTickTime_
 */
void Tick::receiveNextTime( const Conn* c, double v )
{
	static_cast< Tick* >( c->data() )->nextTickTime_ = v;
}

/**
 * This is the key function for Ticks. It moves the time along
 * and calls all the target object Process functions,
 * and also ensures that all ticks get called, in order.
 */
void Tick::incrementTick( const Conn* c, ProcInfo p, double v )
{
	static_cast< Tick* >( c->data() )->innerIncrementTick( 
					c->target(), p, v );
}

void Tick::innerIncrementTick(
		Eref e, ProcInfo info, double prevTickTime )
{
	if ( next_ ) {
		if ( nextTime_ <= nextTickTime_ ) {
			info->currTime_ = nextTime_;
			info->dt_ = dt_;
			// send1< ProcInfo >( e, processSlot, info );
			this->innerProcessFunc( e, info );
			nextTime_ += dt_;
		}
		if ( nextTime_ > nextTickTime_ &&
						prevTickTime > nextTickTime_ ) {
			// This call strictly cannot be parallelized. It
			// requires that the send operation get a return message
			// which updates the nextTickTime_
			send2< ProcInfo, double >(
							e, nextSlot, info, nextTime_ );
		}
		if ( nextTickTime_ < nextTime_ ) {
			send1< double >( e, returnNextTimeSlot, nextTickTime_ );
			return;
		}
	} else {
		info->currTime_ = nextTime_;
		info->dt_ = dt_;
		// send1< ProcInfo >( e, processSlot, info );
		this->innerProcessFunc( e, info );
		nextTime_ += dt_;
	}
	send1< double >( e, returnNextTimeSlot, nextTime_ );
}

/**
 * Resched is used to rebuild the scheduling. It does NOT mean that
 * the timings have to be updated: we may need to resched during a
 * run without missing a beat.
 */
void Tick::resched( const Conn* c )
{
	static_cast< Tick* >( c->data() )->
			updateNextTickTime( c->target() );
}

/**
 * updateNextTickTime cascades down the ticks to initialize their
 * nextTime_ field by querying the next one.
 * Invoked whenever there is a rescheduling.
 */
void Tick::updateNextTickTime( Eref e )
{
	// As with all of these, we cannot do any of this in parallel
	// Here we are simply setting the local value of nextTickTime_
	// to the nextTime_ value of the next tick.
	// SimpleElement* se = static_cast< SimpleElement* >( e );
	
	next_ = ( e.e->msg( nextSlot.msg() )->size() > 0 );
	if ( next_ ) {
		// This asks for the nextTime_ of the next tick
		send0( e, requestNextTimeSlot );

		// This sends local nextTime_ to previous tick.
		send1< double >( e, returnNextTimeSlot, nextTime_ );

		// This asks the next Tick to resched itself.
		send0( e, reschedSlot );
	} else {
		nextTickTime_ = 0;
	}
}

/**
 * Reinit is used to set the simulation time back to zero for itself,
 * and to trigger reinit in all targets, and to go on to the next tick
 */
void Tick::reinit( const Conn* c, ProcInfo info )
{
	Tick* t = static_cast< Tick* >( c->data() );
	t->nextTime_ = 0.0;
	t->nextTickTime_ = 0.0;
	info->currTime_ = 0.0;
	info->dt_ = t->dt_;
	// send1< ProcInfo >( c.targetElement(), reinitSlot, info );
	t->innerReinitFunc( c->target(), info );
	if ( t->next_ )
		send1< ProcInfo >( c->target(), reinitNextSlot, info );
}

/**
 * The handleNextTimeRequest function returns the local
 * nextTime_ field to the requesting tick, which is the previous
 * one in the sequence.
 */
void Tick::handleNextTimeRequest( const Conn* c )
{
	send1< double >( c->target(), returnNextTimeSlot,
					static_cast< Tick* >( c->data() )->nextTime_ );
}

/**
 * The 'start' function begins the simulation. It is called only on the
 * first tick, which is the one with the smallest dt and the smallest
 * stage_ number. The start function is the main loop for
 * advancing time through itself and all the ticks.
 */
void Tick::start( const Conn* c, ProcInfo info, double maxTime )
{
	static_cast< Tick* >( c->data() )->innerStart( 
					c->target(), info, maxTime );
}

void Tick::innerStart( Eref e, ProcInfo info, double maxTime )
{
	static double NEARLY_ONE = 0.999999999999;
	static double JUST_OVER_ONE = 1.000000000001;
	double endTime;
	maxTime = maxTime * NEARLY_ONE;

	while ( info->currTime_ < maxTime ) {
		endTime = maxTime + dt_;
		if ( next_ && endTime > nextTickTime_ )
			endTime = nextTickTime_ * JUST_OVER_ONE;
		info->dt_ = dt_;

		while ( nextTime_ <= endTime ) {
			info->currTime_ = nextTime_;

			// Send back the nextTime (which is now the current time)
			// to the ClockJob.
			send1< double >( e, returnNextTimeSlot, nextTime_ );

			// Send out the process message to all and sundry
			// send1< ProcInfo >( e, processSlot, info );
			this->innerProcessFunc( e, info );
			nextTime_ += dt_;
		}

		if ( next_ ) {
			// This loop strictly cannot be parallelized. It
			// requires that the send operation get a return message
			// which updates the nextTickTime_
			while ( nextTickTime_ < nextTime_ )
				send2< ProcInfo, double >( 
								e, nextSlot, info, nextTime_ );
		}
	}
}


///////////////////////////////////////////////////
// Virtual function definitions for actually sending out the 
// process and reinit calls.
///////////////////////////////////////////////////
/**
 * This sends out the process call. It is virtualized
 * because derived classes (ParTick) need to do much more complicated
 * things to send out data and interleave communication and 
 * computation.
 */
void Tick::innerProcessFunc( Eref e, ProcInfo info )
{
	send1< ProcInfo >( e, processSlot, info );
}

/**
 * This sends out the call to reinit objects. It is virtualized
 * because derived classes (ParTick) need to do much more complicated
 * things to coordinate the reinit.
 */
void Tick::innerReinitFunc( Eref e, ProcInfo info )
{
	send1< ProcInfo >( e, reinitSlot, info );
}

///////////////////////////////////////////////////
// Other function definitions
///////////////////////////////////////////////////
int Tick::ordinalCounter_ = 0;

