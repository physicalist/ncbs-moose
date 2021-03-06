/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#include "moose.h"
#include "KinCompt.h"
#include "KineticManager.h"
#include "Molecule.h"
#include "Reaction.h"
#include "Enzyme.h"

void rescaleTree( Eref e, double ratio );

/**
 * The KinCompt is a compartment for kinetic calculations. It doesn't
 * really correspond to a single Smoldyn concept, but it encapsulates
 * many of them into the traditional compartmental view. It connects up
 * with one or more surfaces which collectively define its volume and
 * kinCompt.
 * It also maps onto the SBML concept for compartments. It permits
 * the formation of compartments without surfaces but this is 
 * discouraged.
 */
const Cinfo* initKinComptCinfo()
{
	// Connects to molecules, which trigger a request for volume info.
	// Volume here may be size?
	static Finfo* extentShared[] =
	{
		// args are size, nDimensions
		new SrcFinfo( "returnExtent", 
			Ftype2< double, unsigned int >::global() ),
		new DestFinfo( "requestExtent", Ftype0::global(),
			RFCAST( &KinCompt::requestExtent ) ),
	};

	static Finfo* kinComptFinfos[] =
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		// Volume is the compartment volume. If there are no
		// messages to surfaces this returns the local value.
		// The setVolume only works if there are no surfaces anywhere.
		// Otherwise the surfaces override it
		// Assigning any of the following four parameters causes automatic
		// rescaling of rates throughout the model.
		new ValueFinfo( "volume", 
			ValueFtype1< double >::global(),
			GFCAST( &KinCompt::getVolume ), 
			RFCAST( &KinCompt::setVolume )
		),
		new ValueFinfo( "area", 
			ValueFtype1< double >::global(),
			GFCAST( &KinCompt::getArea ), 
			RFCAST( &KinCompt::setArea )
		),
		new ValueFinfo( "perimeter", 
			ValueFtype1< double >::global(),
			GFCAST( &KinCompt::getPerimeter ), 
			RFCAST( &KinCompt::setPerimeter )
		),

		// This takes whichever of the above is appropriate for the # of
		// dimensions. Means the same thing as the SBML size.
		new ValueFinfo( "size", 
			ValueFtype1< double >::global(),
			GFCAST( &KinCompt::getSize ), 
			RFCAST( &KinCompt::setSize )
		),


		new ValueFinfo( "numDimensions", 
			ValueFtype1< unsigned int >::global(),
			GFCAST( &KinCompt::getNumDimensions ), 
			RFCAST( &KinCompt::setNumDimensions )
		),
	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
		// This goes to the compartment that encloses the current one.
		// Appropriate even for 2d enclosed by 3d and so on.
		new SrcFinfo( "outside", Ftype0::global() ),
	///////////////////////////////////////////////////////
	// MsgDest definitions
	///////////////////////////////////////////////////////
		// This handles the 'outside' message from interior compartments.
		new DestFinfo( "inside", Ftype0::global(),
			&dummyFunc ),

		/**
	 	* Gets input from a single exterior surface
	 	*/
		new DestFinfo( "exterior", 
			Ftype3< double, double, double >::global(),
			RFCAST( &KinCompt::exteriorFunction ) ),
			
		/**
	 	* Gets input from possibly many interior surfaces
	 	*/
		new DestFinfo( "interior", 
			Ftype3< double, double, double >::global(),
			RFCAST( &KinCompt::interiorFunction ) ),

		/**
	 	* Rescales the volume by the specified ratio. NewVol = ratio * old.
	 	*/
		new DestFinfo( "rescaleSize", 
			Ftype1< double >::global(),
			RFCAST( &KinCompt::rescaleFunction ) ),

		// Assigns size without rescaling the entire model.
		new DestFinfo( "sizeWithoutRescale", 
			Ftype1< double >::global(),
			RFCAST( &KinCompt::setSizeWithoutRescale ) ),
	///////////////////////////////////////////////////////
	// Synapse definitions
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////
		new SharedFinfo( "extent", extentShared,
			sizeof( extentShared ) / sizeof( Finfo* ) ),
	};

	static Cinfo kinComptCinfo(
		"KinCompt",
		"Upinder S. Bhalla, 2007, NCBS",
		"KinCompt: Kinetic compartment. Has its on volume, or gets +ve or negative contributions from varous surface objects",
		initNeutralCinfo(),
		kinComptFinfos,
		sizeof( kinComptFinfos )/sizeof(Finfo *),
		ValueFtype1< KinCompt >::global()
	);

	return &kinComptCinfo;
}

static const Cinfo* kinComptCinfo = initKinComptCinfo();

static const Slot extentSlot =
	initKinComptCinfo()->getSlot( "extent.returnExtent" );

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

KinCompt::KinCompt()
	: 	size_( 1.6666666667e-21 ), 
		volume_( 1.666666667e-21 ), 
		area_( 1.0 ), 
		perimeter_( 1.0 ), 
		numDimensions_( 3 )
{
		;
}

///////////////////////////////////////////////////
// Field access functions.
///////////////////////////////////////////////////
		
void KinCompt::setVolume( const Conn* c, double value )
{
	static_cast< KinCompt* >( c->data() )->innerSetSize( 
		c->target(), value );
}

double KinCompt::getVolume( Eref e )
{
	return static_cast< KinCompt* >( e.data() )->volume_;
}

void KinCompt::setArea( const Conn* c, double value )
{
	static_cast< KinCompt* >( c->data() )->innerSetSize(
		c->target(), value );
}

double KinCompt::getArea( Eref e )
{
	return static_cast< KinCompt* >( e.data() )->area_;
}

void KinCompt::setPerimeter( const Conn* c, double value )
{
	static_cast< KinCompt* >( c->data() )->innerSetSize(
		c->target(), value );
}

double KinCompt::getPerimeter( Eref e )
{
	return static_cast< KinCompt* >( e.data() )->perimeter_;
}

void KinCompt::setSize( const Conn* c, double value )
{
	static_cast< KinCompt* >( c->data() )->innerSetSize( 
		c->target(), value );
}

void KinCompt::setSizeWithoutRescale( const Conn* c, double value )
{
	static_cast< KinCompt* >( c->data() )->innerSetSize( 
		c->target(), value, 1 );
}

void KinCompt::innerSetSize( Eref e, double value, bool ignoreRescale )
{
	assert( size_ > 0.0 );
	double ratio = value/size_;
	size_ = value;
	if ( numDimensions_ == 3 )
		volume_ = value;
	else if ( numDimensions_ == 2 )
		area_ = value;
	else if ( numDimensions_ == 1 )
		perimeter_ = value;

	// Here we scan through all children telling them to rescale.
	if ( !ignoreRescale )
		rescaleTree( e, ratio );
}

double KinCompt::getSize( Eref e )
{
	return static_cast< KinCompt* >( e.data() )->size_;
}

void KinCompt::setNumDimensions( const Conn* c, unsigned int value )
{
	if ( value == 0 || value > 3 )
		return;
	static_cast< KinCompt* >( c->data() )->numDimensions_ = value;
}

unsigned int KinCompt::getNumDimensions( Eref e )
{
	return static_cast< KinCompt* >( e.data() )->numDimensions_;
}


///////////////////////////////////////////////////
// MsgDest functions.
///////////////////////////////////////////////////

void KinCompt::requestExtent( const Conn* c )
{
	static_cast< KinCompt* >( c->data() )->
		innerRequestExtent( c->target() );
}

void KinCompt::innerRequestExtent( Eref e ) const 
{
	send2< double, unsigned int >( e, extentSlot, size_, numDimensions_ );
}

void KinCompt::localExteriorFunction( double v1, double v2, double v3 )
{
	volume_ = v1;
	area_ = v2;
	perimeter_ = v3;
	if ( numDimensions_ == 3 )
		size_ = volume_;
	else if ( numDimensions_ == 2 )
		size_ = area_;
	else if ( numDimensions_ == 1 )
		size_ = perimeter_;
}

void KinCompt::exteriorFunction( 
	const Conn* c, double v1, double v2, double v3 )
{
	static_cast< KinCompt* >( c->data() )->localExteriorFunction( v1, v2, v3 );
}

void KinCompt::localInteriorFunction( double v1, double v2, double v3 )
{
	if ( numDimensions_ == 3 ) {
		volume_ -= v1;
		area_ += v2;
		size_ = volume_;
	} else if ( numDimensions_ == 2 ) {
		area_ -= v2;
		size_ = area_;
	} else if ( numDimensions_ == 1 ) {
		perimeter_ -= v3;
		size_ = perimeter_;
	}
}

void KinCompt::interiorFunction( 
	const Conn* c, double v1, double v2, double v3 )
{
	static_cast< KinCompt* >( c->data() )->localInteriorFunction( v1, v2, v3 );
}

void KinCompt::rescaleFunction( 
	const Conn* c, double ratio )
{
	double size = static_cast< KinCompt* >( c->data() )->size_;
	assert( size > 0.0 );
	static_cast< KinCompt* >( c->data() )->innerSetSize(
		c->target(), size * ratio );
}

///////////////////////////////////////////////////
// Utility functions for volume and size management.
///////////////////////////////////////////////////

/**
 * This is an extern function used by molecules, enzymes and reacs.
 * Traverses toward the root till it finds a KinCompt to get volScale.
 * If it runs into / return 1
 * If it runs into a KineticManager (happens with legacy kkit simulations)
 * uses the volScale on the kinetic manager.
 */
double getVolScale( Eref e )
{
	static const Finfo* parentFinfo = 
		initNeutralCinfo()->findFinfo( "parent" );
	static const Finfo* sizeFinfo = 
		initKinComptCinfo()->findFinfo( "size" );
	static const Finfo* managerSizeFinfo = 
		initKineticManagerCinfo()->findFinfo( "volume" );
	Id pa;
	bool ret = get< Id >( e, parentFinfo, pa );
	assert( ret );
	while( pa != Id() ) {
		Eref e = pa.eref();
		if ( e.e->cinfo()->isA( initKinComptCinfo() ) ) {
			double size = 1.0;
			ret = get< double >( e, sizeFinfo, size );
			assert( ret );
			assert( size > 0.0 );
			// Here we need to put in units too.
			return 6e20 * size;
		}
		if ( e.e->cinfo()->isA( initKineticManagerCinfo() ) ) {
			double size = 1.0;
			ret = get< double >( e, managerSizeFinfo, size );
			assert( ret );
			assert( size > 0.0 );
			// Here we need to put in units too.
			return 6e20 * size;
		}
		ret = get< Id >( e, parentFinfo, pa );
		assert( ret );
	}
	cout << "KinCompt.cpp:getVolScale: Failed to find KinCompt for volume\n";
	return 1.0;
}

/**
 * Recursively goes through all children, rescaling volumes.
 * Does NOT rescale current Eref.
 */
void rescaleTree( Eref e, double ratio )
{
	static const Finfo* childListFinfo = 
		initNeutralCinfo()->findFinfo( "childList" );
	static const Finfo* rescaleMolFinfo = 
		initMoleculeCinfo()->findFinfo( "rescaleVolume" );
	static const Finfo* rescaleReacFinfo = 
		initReactionCinfo()->findFinfo( "rescaleRates" );
	static const Finfo* rescaleEnzFinfo = 
		initEnzymeCinfo()->findFinfo( "rescaleRates" );
	static const Finfo* rescaleKinComptFinfo = 
		initKinComptCinfo()->findFinfo( "rescaleSize" );

	assert( childListFinfo != 0 && rescaleMolFinfo != 0 && rescaleReacFinfo != 0 && rescaleEnzFinfo != 0 && rescaleKinComptFinfo != 0 );

	vector< Id > kids;
	get< vector< Id > >( e, childListFinfo, kids );

	for( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i ) {
		if ( i->eref().e->cinfo()->isA( initReactionCinfo() ) )
			set< double >( i->eref(), rescaleReacFinfo, ratio );
		else if ( i->eref().e->cinfo()->isA( initEnzymeCinfo() ) )
			set< double >( i->eref(), rescaleEnzFinfo, ratio );
		else if ( i->eref().e->cinfo()->isA( initMoleculeCinfo() ) )
			set< double >( i->eref(), rescaleMolFinfo, ratio );

		// This sets off its own rescale recursive command.
		if ( i->eref().e->cinfo()->isA( initKinComptCinfo() ) )
			set< double >( i->eref(), rescaleKinComptFinfo, ratio );
		else 
			rescaleTree( i->eref(), ratio );
	}
}
