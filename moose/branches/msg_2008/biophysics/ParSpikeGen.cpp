/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include <math.h>
#include <mpi.h>

#include "SpikeGen.h"
#include "ParSpikeGen.h"


const Cinfo* initParSpikeGenCinfo()
{
	/**
	 * This is a shared message to receive Process messages from
	 * the scheduler objects.
	 */
	static Finfo* processShared[] =
	{
		new DestFinfo( "process", Ftype1< ProcInfo >::global(),
				RFCAST( &ParSpikeGen::processFunc ) ),
		new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
				RFCAST( &SpikeGen::reinitFunc ) ),
	};
	static Finfo* process =	new SharedFinfo( "process", processShared, 
			sizeof( processShared ) / sizeof( Finfo* ) );
	
	static Finfo* spikeGenFinfos[] = 
	{
		new ValueFinfo( "threshold", ValueFtype1< double >::global(),
			GFCAST( &SpikeGen::getThreshold ),
			RFCAST( &SpikeGen::setThreshold )
		),
		new ValueFinfo( "refractT", ValueFtype1< double >::global(),
			GFCAST( &SpikeGen::getRefractT ),
			RFCAST( &SpikeGen::setRefractT )
		),
		new ValueFinfo( "abs_refract", ValueFtype1< double >::global(),
			GFCAST( &SpikeGen::getRefractT ),
			RFCAST( &SpikeGen::setRefractT )
		),

		/**
		 * The amplitude field of the spikeGen is never used.
		 * \todo: perhaps should deprecate this.
		 */
		new ValueFinfo( "amplitude", ValueFtype1< double >::global(),
			GFCAST( &SpikeGen::getAmplitude ),
			RFCAST( &SpikeGen::setAmplitude )
		),
		new ValueFinfo( "state", ValueFtype1< double >::global(),
			GFCAST( &SpikeGen::getState ),
			RFCAST( &SpikeGen::setState )
		),

	//////////////////////////////////////////////////////////////////
	// SharedFinfos
	//////////////////////////////////////////////////////////////////
		process,
		/*
		new SharedFinfo( "process", processShared, 
			sizeof( processShared ) / sizeof( Finfo* ) ),
			*/

	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
		// Sends out a trigger for an event. The time is not 
		// sent - everyone knows the time.
		new SrcFinfo( "event", Ftype1< double >::global() ),

	//////////////////////////////////////////////////////////////////
	// Dest Finfos.
	//////////////////////////////////////////////////////////////////
		new DestFinfo( "Vm", Ftype1< double >::global(),
			RFCAST( &SpikeGen::VmFunc ) ),

               new DestFinfo( "sendRank", Ftype1< int >::global(),
                        RFCAST( &ParSpikeGen::sendRank ) ),
	};

	// We want the spikeGen to update after the compartments have done so
	static SchedInfo schedInfo[] = { { process, 0, 1 } };

	static Cinfo spikeGenCinfo(
				"ParSpikeGen",
				"Mayuresh Kulkarni",
				"Parallel version of SpikeGen",
				initNeutralCinfo(),
				spikeGenFinfos,
				sizeof( spikeGenFinfos ) / sizeof( Finfo* ),
				ValueFtype1< ParSpikeGen >::global(),
				schedInfo, 1
	);

	return &spikeGenCinfo;
}

static const int SPIKE_TAG = 3;
static const Cinfo* spikeGenCinfo = initParSpikeGenCinfo();
static const Slot eventSlot = initParSpikeGenCinfo()->getSlot( "event" );

ParSpikeGen::ParSpikeGen()
{
}

ParSpikeGen::~ParSpikeGen()
{
	for(unsigned int i=0; i<request_.size(); i++)
		delete request_[i];

	request_.clear();
}

void ParSpikeGen::sendRank( const Conn* c, int rank )
{
        static_cast< ParSpikeGen* >( c->data() )->sendRank_.push_back(rank);
        static_cast< ParSpikeGen* >( c->data() )->request_.push_back( new MPI_Request);
}

void ParSpikeGen::innerProcessFunc( const Conn* c, ProcInfo p )
{
        static double t;
	unsigned int i;
        t = p->currTime_;
	static bool bSpikeSent = false;
	//int iMyRank;

        if ( V_ > threshold_ && t >= lastEvent_ + refractT_ ) {
		//MPI_Comm_rank(MPI_COMM_WORLD, &iMyRank);
                //cout<<endl<<"V_ "<<V_<<" threshold "<<threshold_<<" t "<<t<<flush;

		if(bSpikeSent == true)
		{
	                for(i=0; i<sendRank_.size(); i++)
			{
                	        MPI_Wait(request_[i], MPI_STATUS_IGNORE);
			}
			bSpikeSent = false;
		}


                for(i=0; i<sendRank_.size(); i++)
                {
                        //cout<<endl<<"Process: "<<iMyRank<<" sent a tick to rank "<< sendRank_[i]<<" Tag: "<<SPIKE_TAG<<flush;
                        //MPI_Send(&t, 1, MPI_DOUBLE, sendRank_[i], SPIKE_TAG, MPI_COMM_WORLD);
			
                        MPI_Isend(&t, 1, MPI_DOUBLE, sendRank_[i], SPIKE_TAG, MPI_COMM_WORLD, request_[i]);
			bSpikeSent = true;
                }


                send1< double >( c->targetElement(), eventSlot, t );
                lastEvent_ = t;
                state_ = amplitude_;
        } else {
                state_ = 0.0;
        }
}

void ParSpikeGen::processFunc( const Conn* c, ProcInfo p )
{
        static_cast< ParSpikeGen* >( c->data() )->innerProcessFunc( c, p );
}

