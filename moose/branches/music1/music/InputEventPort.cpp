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
#include "Music.h"
#include "InputEventPort.h"
#include "InputEventChannel.h"
#include <sstream>
#include "element/Neutral.h"

const Cinfo* initInputEventPortCinfo()
{

  /**
   * This is a shared message to receive Process messages from
   * the scheduler objects.
   */

  static Finfo* processShared[] =
    {
      new DestFinfo( "process", Ftype1< ProcInfo >::global(),
                     dummyFunc ),
      new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
                     RFCAST( &InputEventPort::reinitFunc ) )
    };

  static Finfo* process = 
    new SharedFinfo( "process", processShared,
                     sizeof( processShared ) / sizeof( Finfo* ) );


  static Finfo* inputEventPortFinfos[] =
    {
      new ValueFinfo( "width", ValueFtype1< unsigned int >::global(),
                      GFCAST( &InputEventPort::getWidth ),
                      &dummyFunc
                      ),
      new ValueFinfo( "accLatency", ValueFtype1< double >::global(),
                      GFCAST( &InputEventPort::getAccLatency ),
                      RFCAST( &InputEventPort::setAccLatency )
                      ),
      new ValueFinfo( "maxBuffered", ValueFtype1< int >::global(),
                      GFCAST( &InputEventPort::getMaxBuffered ),
                      RFCAST( &InputEventPort::setMaxBuffered )
                      ),
      new DestFinfo( "initialise", 
                     Ftype3< unsigned int, unsigned int, 
                             MUSIC::event_input_port* >::global(),
                     RFCAST( &InputEventPort::initialiseFunc )
                     ),
      //////////////////////////////////////////////////////////////////
      // SharedFinfos
      //////////////////////////////////////////////////////////////////
      process,

    };

  
  static Cinfo inputEventPortCinfo("InputEventPort",
                             "Niraj Dudani and Johannes Hjorth",
                             "InputEventPort for communciation with the MUSIC API",
                             initNeutralCinfo(),
                             inputEventPortFinfos,
                             sizeof( inputEventPortFinfos ) / sizeof( Finfo* ),
                             ValueFtype1< InputEventPort >::global() );
  
  
  return &inputEventPortCinfo;

}

static const Cinfo* inputEventPortCinfo = initInputEventPortCinfo();

// This is from Channel and not from the port
static const Slot eventSlot =
        initInputEventChannelCinfo()->getSlot( "event" );



void InputEventPort::reinitFunc( const Conn* c, ProcInfo p ) 
{
  static_cast < InputEventPort* > (c->data())->innerReinitFunc();
}

void InputEventPort::innerReinitFunc() 
{
  // Map the input from MUSIC to data channels local to this process
  MUSIC::linear_index iMap(myOffset_, myWidth_);
  mPort_->map(&iMap, this, accLatency_, maxBuffered_);


}

// Event handler
void InputEventPort::operator () ( double t, MUSIC::local_index id ) 
{
  int localId = id;
  send1 < double > ( channels_[localId](), eventSlot, t );

}


void InputEventPort::initialiseFunc( const Conn* c, 
                                     unsigned int width,
                                     unsigned int offset,
                                     MUSIC::event_input_port* mPort)
{
  static_cast < InputEventPort* > 
    (c->data())->innerInitialiseFunc(c->target(), width, offset, mPort);
}

void InputEventPort::innerInitialiseFunc( Eref e, 
                                          unsigned int width, 
                                          unsigned int offset,
                                          MUSIC::event_input_port* mPort
) 
{

  mPort_ = mPort;
  myWidth_ = width;
  myOffset_ = offset;

  for(unsigned int i = channels_.size(); i < width; i++)
    {
      ostringstream name;
      
      name << "channel[" << i + offset << "]";

      Element* channel = Neutral::create( "InputEventChannel", name.str(),
                                          e.id(), Id::scratchId() );
      channels_.push_back(channel->id());
    }

}

unsigned int InputEventPort::getWidth( Eref e ) 
{
  return static_cast < InputEventPort* > (e.data())->channels_.size();
}

double InputEventPort::getAccLatency( Eref e )
{
  return static_cast < InputEventPort* > (e.data())->accLatency_;
}

void InputEventPort::setAccLatency( const Conn* c, double accLatency )
{
  static_cast < InputEventPort* > (c->data())->accLatency_ = accLatency;
}

int InputEventPort::getMaxBuffered( Eref e )
{
  return static_cast < InputEventPort* > (e.data())->maxBuffered_;
}

void InputEventPort::setMaxBuffered( const Conn* c, int maxBuffered )
{
  static_cast < InputEventPort* > (c->data())->maxBuffered_ = maxBuffered;
}

