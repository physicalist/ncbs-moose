/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
**             and Niraj Dudani and Johannes Hjorth, KTH, Stockholm
**
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include <mpi.h>
#include <music.hh>
#include "maindir/MMPI.h"
#include "Music.h"
#include "element/Neutral.h"

const Cinfo* initMusicCinfo()
{
	/**
	 * This is a shared message to receive Process messages from
	 * the scheduler objects.
	 */
	static Finfo* processShared[] =
	{
		new DestFinfo(
			"process", Ftype1< ProcInfo >::global(),
			RFCAST( &Music::processFunc ) ),
		new DestFinfo(
			"reinit", Ftype1< ProcInfo >::global(),
			RFCAST( &Music::reinitFunc ) ),
	};
	
	static Finfo* process = new SharedFinfo(
		"process",
		processShared,
		sizeof( processShared ) / sizeof( Finfo* ) );
	
	static Finfo* musicFinfos[] =
	{
	//////////////////////////////////////////////////////////////////
	// Field definitions
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// SharedFinfos
	//////////////////////////////////////////////////////////////////
		process,
	//////////////////////////////////////////////////////////////////
	// Dest Finfos.
	//////////////////////////////////////////////////////////////////
		new DestFinfo(
			"setup",
			Ftype1< MUSIC::setup* >::global(),
			RFCAST( &Music::setupFunc ) ),
		new DestFinfo(
			"finalize",
			Ftype0::global(),
			RFCAST( &Music::finalizeFunc ) ),
		new DestFinfo(
			"addPort", 
			Ftype3< string, string, string >::global(),
			RFCAST( &Music::addPort ) ),
	};
	
	static SchedInfo schedInfo[] = { { process, 0, 1 } };
	
	static Cinfo musicCinfo(
		"Music",
		"Niraj Dudani and Johannes Hjorth",
		"Moose Music object for communciation with the MUSIC API",
		initNeutralCinfo(),
		musicFinfos,
		sizeof( musicFinfos ) / sizeof( Finfo* ),
		ValueFtype1< Music >::global(),
		schedInfo, 1
	);
	
	return &musicCinfo;
}

static const Cinfo* musicCinfo = initMusicCinfo();

//////////////////////////////////////////////////////////////////
// Field access functions
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Message dest functions.
//////////////////////////////////////////////////////////////////
void Music::innerProcessFunc( const Conn* c, ProcInfo p ) 
{
  runtime_->tick();
}

void Music::processFunc( const Conn* c, ProcInfo p ) 
{
	static_cast < Music* > (c->data() )->innerProcessFunc( c, p );
}
  
void Music::reinitFunc( const Conn* c, ProcInfo p ) 
{
  static_cast < Music* > (c->data() )->innerReinitFunc(c->target(),p);
}

void Music::innerReinitFunc( Eref e, ProcInfo p ) 
{
  if(setup_) {
    

    runtime_ = new MUSIC::runtime(setup_, p->dt_ );
    setup_ = 0;
  }

}

void Music::setupFunc( const Conn* c, MUSIC::setup* setup )
{
	static_cast< Music* >( c->data() )->
		innerSetupFunc( c->target(), setup );
}

void Music::innerSetupFunc( Eref e, MUSIC::setup* setup )
{
	setup_ = setup;
}

void Music::finalizeFunc( const Conn* c )
{
	static_cast< Music* >( c->data() )->innerFinalizeFunc(c->target());
}

void Music::innerFinalizeFunc( Eref e )
{
	delete runtime_;
	runtime_ = 0;
}

void Music::addPort (
	const Conn* c,
	string name,
	string direction,
	string type ) 
{
  static_cast < Music* > ( c->data() )->innerAddPort(c->target(), 
                                                     name, direction, type );
}

void Music::innerAddPort (
	Eref e,
	string direction,
	string type,
	string name ) 
{
  if(direction == "in" && type == "event") {

    // Create the event input port
    Element* port = 
      Neutral::create("InputEventPort", name, e.id(), Id::scratchId() );

    // Publish the event input port to music
    MUSIC::event_input_port* mPort = setup_->publish_event_input(name);
    int width = mPort->width();
    
    int numNodes = MMPI::INTRA_COMM().Get_size();
    int myRank = MMPI::INTRA_COMM().Get_rank();

    // Calculate base offset and width for our process
    // last node gets any extra channels left.
    int avgWidth = width / numNodes;
    int myWidth = (myRank < numNodes-1) ? 
      avgWidth : width - avgWidth*(numNodes-1);

    int myOffset = myRank * avgWidth;

    set< unsigned int >(port,"initialise", myWidth, myOffset, mPort);
    
    // Map the input from MUSIC to data channels local to this process
    // is done in InputEventPort

  }
  else if(direction == "out" && type == "event"){
     // Create the event output port
    Element* port = 
      Neutral::create("OutputEventPort", name, e.id(), Id::scratchId() );

    // Publish the event output port to music
    MUSIC::event_output_port* mPort = setup_->publish_event_output(name);
    int width = mPort->width();
    
    int numNodes = MMPI::INTRA_COMM().Get_size();
    int myRank = MMPI::INTRA_COMM().Get_rank();

    // Calculate base offset and width for our process
    // last node gets any extra channels left.
    int avgWidth = width / numNodes;
    int myWidth = (myRank < numNodes-1) ? 
      avgWidth : width - avgWidth*(numNodes-1);

    int myOffset = myRank * avgWidth;

    set< unsigned int >(port,"initialise", myWidth, myOffset, mPort);
    
    // Map the output from MUSIC to data channels local to this process
    // is done in OutputEventPort

  }
  else {
    cerr << "Music::innerAddPort: " << direction << " " << type 
         << " Not supported yet";

  }
}
