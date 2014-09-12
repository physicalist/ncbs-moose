/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ChanBase.h"

SrcFinfo1< double >* ChanBase::permeability()
{
	static SrcFinfo1< double > permeability( "permeabilityOut", 
		"Conductance term going out to GHK object" );
	return &permeability;
}

SrcFinfo2< double, double >* ChanBase::channelOut()
{
	static SrcFinfo2< double, double > channelOut( "channelOut", 
		"Sends channel variables Gk and Ek to compartment" );
	return &channelOut;
}

SrcFinfo1< double >* ChanBase::IkOut()
{
	static SrcFinfo1< double > IkOut( "IkOut", 
		"Channel current. This message typically goes to concen"
		"objects that keep track of ion concentration." );
	return &IkOut;
}

const Cinfo* ChanBase::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// Shared messages
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	/// ChannelOut SrcFinfo defined above.
	static DestFinfo Vm( "Vm", 
		"Handles Vm message coming in from compartment",
		new OpFunc1< ChanBase, double >( &ChanBase::handleVm ) );

	static Finfo* channelShared[] =
	{
		channelOut(), &Vm
	};
	static SharedFinfo channel( "channel", 
		"This is a shared message to couple channel to compartment. "
		"The first entry is a MsgSrc to send Gk and Ek to the compartment "
		"The second entry is a MsgDest for Vm from the compartment.",
		channelShared, sizeof( channelShared ) / sizeof( Finfo* )
	);

	///////////////////////////////////////////////////////
	// Here we reuse the Vm DestFinfo declared above.

	/// Permability SrcFinfo defined above.
	static Finfo* ghkShared[] =
	{
		&Vm, permeability()
	};
	static SharedFinfo ghk( "ghk", 
		"Message to Goldman-Hodgkin-Katz object",
		ghkShared, sizeof( ghkShared ) / sizeof( Finfo* ) );

///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////

		static ValueFinfo< ChanBase, double > Gbar( "Gbar",
			"Maximal channel conductance",
			&ChanBase::setGbar,
			&ChanBase::getGbar
		);
		static ValueFinfo< ChanBase, double > Ek( "Ek", 
			"Reversal potential of channel",
			&ChanBase::setEk,
			&ChanBase::getEk
		);
		static ValueFinfo< ChanBase, double > Gk( "Gk",
			"Channel conductance variable",
			&ChanBase::setGk,
			&ChanBase::getGk
		);
		static ReadOnlyValueFinfo< ChanBase, double > Ik( "Ik",
			"Channel current variable",
			&ChanBase::getIk
		);

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	// IkOut SrcFinfo defined above.

///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	
///////////////////////////////////////////////////////
	static Finfo* ChanBaseFinfos[] =
	{
		&channel,			// Shared
		&ghk,				// Shared
		&Gbar,				// Value
		&Ek,				// Value
		&Gk,				// Value
		&Ik,				// ReadOnlyValue
		IkOut(),				// Src
	};
	
	static string doc[] =
	{
		"Name", "ChanBase",
		"Author", "Upinder S. Bhalla, 2007-2014, NCBS",
		"Description", "ChanBase: Base class for assorted ion channels."
		"Presents a common interface for all of them. ",
	};

        static  ZeroSizeDinfo< int > dinfo;
        
	static Cinfo ChanBaseCinfo(
		"ChanBase",
		Neutral::initCinfo(),
		ChanBaseFinfos,
		sizeof( ChanBaseFinfos )/sizeof(Finfo *),
                &dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &ChanBaseCinfo;
}

static const Cinfo* chanBaseCinfo = ChanBase::initCinfo();
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
ChanBase::ChanBase()
{ ; }

ChanBase::~ChanBase()
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////
//
void ChanBase::setGbar( double Gbar )
{
	// Call virtual functions of derived classes for this operation.
	vSetGbar( Gbar );
}

double ChanBase::getGbar() const
{
	return vGetGbar();
}

void ChanBase::setEk( double Ek )
{
	vSetEk( Ek );
}
double ChanBase::getEk() const
{
	return vGetEk();
}

void ChanBase::setGk( double Gk )
{
	vSetGk( Gk );
}
double ChanBase::getGk() const
{
	return vGetGk();
}

void ChanBase::setIk( double Ik )
{
	vSetIk( Ik );
}
double ChanBase::getIk() const
{
	return vGetIk();
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void ChanBase::handleVm( double Vm )
{
	vHandleVm( Vm );
}

///////////////////////////////////////////////////
// Looks like a dest function, but it is only called
// from the child class. Sends out various messages.
///////////////////////////////////////////////////

void ChanBase::process(  const Eref& e, const ProcPtr info )
{
	vProcess( e, info );
}


void ChanBase::reinit(  const Eref& e, const ProcPtr info )
{
	vReinit( e, info );
}
