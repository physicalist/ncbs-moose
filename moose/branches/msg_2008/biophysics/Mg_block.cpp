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
#include "Mg_block.h"
#include "../element/Neutral.h"
// #include "DeletionMarkerFinfo.h"
// #include "GlobalMarkerFinfo.h"

const Cinfo* initMg_blockCinfo()
{
	static Finfo* processShared[] =
	{
		new DestFinfo( "process", Ftype1< ProcInfo >::global(),
				RFCAST( &Mg_block::processFunc ) ),
		new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
				RFCAST( &Mg_block::reinitFunc ) ),
	};
	static Finfo* process =	new SharedFinfo( "process", processShared, 
			sizeof( processShared ) / sizeof( Finfo* ) );
	static Finfo* channelShared[] =
	{
		new SrcFinfo( "channel", Ftype2< double, double >::global() ),
		new DestFinfo( "Vm", Ftype1< double >::global(), 
				RFCAST( &Mg_block::channelFunc ) ),
	};
	static Finfo* Mg_blockFinfos[] =
	{
		new ValueFinfo( "KMg_A", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getKMg_A ), 
			RFCAST( &Mg_block::setKMg_A )
		),
		new ValueFinfo( "KMg_B", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getKMg_B ), 
			RFCAST( &Mg_block::setKMg_B )
		),
		new ValueFinfo( "CMg", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getCMg ), 
			RFCAST( &Mg_block::setCMg )
		),
		new ValueFinfo( "Ik", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getIk ), 
			RFCAST( &Mg_block::setIk )
		),
		new ValueFinfo( "Gk", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getGk ), 
			RFCAST( &Mg_block::setGk )
		),
		new ValueFinfo( "Ek", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getEk ), 
			RFCAST( &Mg_block::setEk )
		),
		new ValueFinfo( "Zk", ValueFtype1< double >::global(),
			GFCAST( &Mg_block::getZk ), 
			RFCAST( &Mg_block::setZk )
		),
		process,
		new SharedFinfo( "process", processShared, 
			sizeof( processShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "channel", channelShared,
			sizeof( channelShared ) / sizeof( Finfo* ) ),
		new DestFinfo( "origChannel", Ftype2< double, double >::global(),
				RFCAST( &Mg_block::origChannelFunc ) ),
	};

	static SchedInfo schedInfo[] = { { process, 0, 1 } };

	static Cinfo Mg_blockCinfo(
		"Mg_block",
		"Upinder S. Bhalla, 2007, NCBS",
		"Mg_block: Hodgkin-Huxley type voltage-gated Ion channel. Something\nlike the old tabchannel from GENESIS, but also presents\na similar interface as hhchan from GENESIS. ",
		initNeutralCinfo(),
		Mg_blockFinfos,
		sizeof( Mg_blockFinfos )/sizeof(Finfo *),
		ValueFtype1< Mg_block >::global(),
		schedInfo, 1
	);

	return &Mg_blockCinfo;
}

static const Cinfo* Mg_blockCinfo = initMg_blockCinfo();

static const Slot channelSlot =
	initMg_blockCinfo()->getSlot( "channel.channel" );


///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void Mg_block::setKMg_A( const Conn* c, double KMg_A )
{
	static_cast< Mg_block* >( c->data() )->KMg_A_ = KMg_A;
}
double Mg_block::getKMg_A( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->KMg_A_;
}
void Mg_block::setKMg_B( const Conn* c, double KMg_B )
{
	static_cast< Mg_block* >( c->data() )->KMg_B_ = KMg_B;
}
double Mg_block::getKMg_B( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->KMg_B_;
}
void Mg_block::setCMg( const Conn* c, double CMg )
{
	static_cast< Mg_block* >( c->data() )->CMg_ = CMg;
}
double Mg_block::getCMg( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->CMg_;
}
void Mg_block::setIk( const Conn* c, double Ik )
{
	static_cast< Mg_block* >( c->data() )->Ik_ = Ik;
}
double Mg_block::getIk( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->Ik_;
}
void Mg_block::setGk( const Conn* c, double Gk )
{
	static_cast< Mg_block* >( c->data() )->Gk_ = Gk;
}
double Mg_block::getGk( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->Gk_;
}
void Mg_block::setEk( const Conn* c, double Ek )
{
	static_cast< Mg_block* >( c->data() )->Ek_ = Ek;
}
double Mg_block::getEk( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->Ek_;
}
double Mg_block::getZk( Eref e )
{
	return static_cast< Mg_block* >( e.data() )->Zk_;
}
void Mg_block::setZk( const Conn* c, double Zk )
{
	static_cast< Mg_block* >( c->data() )->Zk_ = Zk;
}


void Mg_block::processFunc( const Conn* c, ProcInfo p )
{
	Eref e = c->target();
	static_cast< Mg_block* >( c->data() )->innerProcessFunc( e, p );
}

void Mg_block::innerProcessFunc( Eref e, ProcInfo info )
{
	
	double KMg = KMg_A_ * exp(Vm_/KMg_B_);
	Gk_ = Gk_ * KMg / (KMg + CMg_);
	send2< double, double >( e, channelSlot, Gk_, Ek_ );
	Ik_ = Gk_ * (Ek_ - Vm_);
}

void Mg_block::reinitFunc( const Conn* c, ProcInfo p )
{
	Eref e = c->target();
	static_cast< Mg_block* >( c->data() )->innerReinitFunc( e, p );
}

void Mg_block::innerReinitFunc( Eref e, ProcInfo info )
{
	Zk_ = 0;
	CMg_ = 0;
	KMg_A_ = 0;
	KMg_B_ = 0;
}

void Mg_block::channelFunc( const Conn* c, double Vm )
{
	static_cast< Mg_block* >( c->data() )->Vm_ = Vm;
}

void Mg_block::origChannelFunc( const Conn* c, double Gk, double Ek )
{
	Mg_block *e = static_cast< Mg_block* >( c->data() );
	e->Gk_ = Gk;
	e->Ek_ = Ek;
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////

