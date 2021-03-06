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
#include <music.hh>
// #include "Music.h"
#include "OutputEventChannel.h"

const Cinfo* initOutputEventChannelCinfo()
{


  static Finfo* outputEventChannelFinfos[] =
    {
      //////////////////////////////////////////////////////////////////
      // SharedFinfos
      //////////////////////////////////////////////////////////////////
      // process,

      ///////////////////////////////////////////////////////
      // MsgSrc definitions
      ///////////////////////////////////////////////////////
      // Sends out a trigger for an event. The time is not
      // sent - everyone knows the time.
      new DestFinfo( "event", Ftype1< double >::global() ,
                     RFCAST( &OutputEventChannel::insertEvent)),      
      new DestFinfo("initialise", 
                    Ftype2< unsigned int, 
                    MUSIC::event_output_port* >::global(),
                    RFCAST( &OutputEventChannel::initialise))

    };

  
  static Cinfo outputEventChannelCinfo("OutputEventChannel",
                                      "Niraj Dudani and Johannes Hjorth",
                                      "OutputEventChannel for communciation with the MUSIC API",
                                      initNeutralCinfo(),
                                      outputEventChannelFinfos,
                                      sizeof( outputEventChannelFinfos ) / sizeof( Finfo* ),
                                      ValueFtype1< OutputEventChannel >::global() );
  
  
  return &outputEventChannelCinfo;

}

static const Cinfo* outputEventChannelCinfo = initOutputEventChannelCinfo();

void OutputEventChannel::insertEvent(const Conn* c, double time) 
{
  static_cast < OutputEventChannel* > (c->data())->innerInsertEvent(time);
}

void OutputEventChannel::innerInsertEvent(double time)
{
  mPort_->insert_event(time, localId_);
}

void OutputEventChannel::initialise(const Conn* c, unsigned int id, 
                                    MUSIC::event_output_port* mPort) 
{
  static_cast < OutputEventChannel* > (c->data())->localId_ 
    = MUSIC::local_index(id);
  static_cast < OutputEventChannel* > (c->data())->mPort_ = mPort;
}
