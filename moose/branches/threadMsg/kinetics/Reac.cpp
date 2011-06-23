/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "Reac.h"

#define EPSILON 1e-15

static SrcFinfo2< double, double > toSub( 
		"toSub", 
		"Sends out increment of molecules on product each timestep"
	);
static SrcFinfo2< double, double > toPrd( 
		"toPrd", 
		"Sends out increment of molecules on product each timestep"
	);

static SrcFinfo1< double > requestSize( 
		"requestSize", 
		"Requests size (volume) in which reaction is embedded. Used for"
		"conversion to concentration units from molecule # units,"
		"and for calculations when resized."
	);

static DestFinfo sub( "subDest",
		"Handles # of molecules of substrate",
		new OpFunc1< Reac, double >( &Reac::sub ) );

static DestFinfo prd( "prdDest",
		"Handles # of molecules of product",
		new OpFunc1< Reac, double >( &Reac::prd ) );
	
static Finfo* subShared[] = {
	&toSub, &sub
};

static Finfo* prdShared[] = {
	&toPrd, &prd
};

const Cinfo* Reac::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< Reac, double > kf(
			"kf",
			"Forward rate constant, in # units",
			&Reac::setKf,
			&Reac::getKf
		);

		static ValueFinfo< Reac, double > kb(
			"kb",
			"Reverse rate constant, in # units",
			&Reac::setKb,
			&Reac::getKb
		);

		static ElementValueFinfo< Reac, double > Kf(
			"Kf",
			"Forward rate constant, in concentration units",
			&Reac::setConcKf,
			&Reac::getConcKf
		);

		static ElementValueFinfo< Reac, double > Kb(
			"Kb",
			"Reverse rate constant, in concentration units",
			&Reac::setConcKb,
			&Reac::getConcKb
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< Reac >( &Reac::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< Reac >( &Reac::reinit ) );

		static DestFinfo group( "group",
			"Handle for group msgs. Doesn't do anything",
			new OpFuncDummy() );

		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions
		//////////////////////////////////////////////////////////////
		static SharedFinfo sub( "sub",
			"Connects to substrate pool",
			subShared, sizeof( subShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo prd( "prd",
			"Connects to substrate pool",
			prdShared, sizeof( prdShared ) / sizeof( const Finfo* )
		);
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);


	static Finfo* reacFinfos[] = {
		&kf,	// Value
		&kb,	// Value
		&requestSize,		// SrcFinfo
		&sub,				// SharedFinfo
		&prd,				// SharedFinfo
		&proc,				// SharedFinfo
	};

	static Cinfo reacCinfo (
		"Reac",
		Neutral::initCinfo(),
		reacFinfos,
		sizeof( reacFinfos ) / sizeof ( Finfo* ),
		new Dinfo< Reac >()
	);

	return &reacCinfo;
}

 static const Cinfo* reacCinfo = Reac::initCinfo();

//////////////////////////////////////////////////////////////
// Reac internal functions
//////////////////////////////////////////////////////////////


Reac::Reac( )
	: kf_( 0.1 ), kb_( 0.2 ), sub_( 0.0 ), prd_( 0.0 )
{
	;
}

Reac::Reac( double kf, double kb )
	: kf_( kf ), kb_( kb ), sub_( 0.0 ), prd_( 0.0 )
{
	;
}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void Reac::sub( double v )
{
	sub_ *= v;
}

void Reac::prd( double v )
{
	prd_ *= v;
}

void Reac::process( const Eref& e, ProcPtr p )
{
	toPrd.send( e, p, sub_, prd_ );
	toSub.send( e, p, prd_, sub_ );
	
	sub_ = kf_;
	prd_ = kb_;
}

void Reac::reinit( const Eref& e, ProcPtr p )
{
	sub_ = kf_;
	prd_ = kb_;
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void Reac::setKf( double v )
{
	sub_ = kf_ = v;
}

double Reac::getKf() const
{
	return kf_;
}

void Reac::setKb( double v )
{
	prd_ = kb_ = v;
}

double Reac::getKb() const
{
	return kb_;
}


/// Utility function
static double lookupSize( const Eref& e, const SrcFinfo* sf )
{
	const vector< MsgFuncBinding >* mfb = 
		e.element()->getMsgAndFunc( sf->getBindIndex() );
	if ( !mfb ) return 1.0;
	if ( mfb->size() == 0 ) return 1.0;

	double size = 
		Field< double >::fastGet( e, (*mfb)[0].mid, (*mfb)[0].fid );

	if ( size <= 0 ) size = 1.0;

	return size;
}

static unsigned int findNumReactants( const Eref& e, const SrcFinfo* sf )
{
	const vector< MsgFuncBinding >* mfb = 
		e.element()->getMsgAndFunc( sf->getBindIndex() );
	if ( !mfb ) return 0;
	return mfb->size();
}

static double volScale( const Eref& e, 
	const SrcFinfo *sizeF, const SrcFinfo *numF )
{
	unsigned int n = findNumReactants( e, &toPrd );
	if ( n == 0 ) return 1.0;
	double size = lookupSize( e, &requestSize );
	double scale = pow( 1e-3 * NA * size, n-1 );
	return scale;
}

void Reac::setConcKf( const Eref& e, const Qinfo* q, double v )
{
	sub_ = kf_ = v * volScale( e, &requestSize, &toPrd );
}

double Reac::getConcKf( const Eref& e, const Qinfo* q ) const
{
	return kf_ / volScale( e, &requestSize, &toPrd );
}

void Reac::setConcKb( const Eref& e, const Qinfo* q, double v )
{
	prd_ = kf_ = v * volScale( e, &requestSize, &toSub );
}

double Reac::getConcKb( const Eref& e, const Qinfo* q ) const
{
	return kb_ / volScale( e, &requestSize, &toSub );
}

