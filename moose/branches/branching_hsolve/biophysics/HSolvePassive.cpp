/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "SynInfo.h"		// for SynChanStruct in BioScan. Remove eventually.
#include <queue>			// for SynChanStruct in BioScan. Remove eventually.
#include "HSolveStruct.h"	// for SynChanStruct in BioScan. Remove eventually.
#include "BioScan.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"

void HSolvePassive::setup( Id seed, double dt ) {
	dt_ = dt;
	
	walkTree( seed );
	initialize( );
	storeTree( );
	
	HinesMatrix::setup( children_, Ga_, CmByDt_ );
}

void HSolvePassive::solve( ) {
	forwardEliminate( );
	backwardSubstitute( );
}

//////////////////////////////////////////////////////////////////////
// Setup of data structures
//////////////////////////////////////////////////////////////////////

void HSolvePassive::walkTree( Id seed ) {
	// Find leaf node
	Id previous;
	vector< Id > recent;
	BioScan::adjacent( seed, recent );
	if ( recent.size() != 1 )
		while ( !recent.empty() ) {
			previous = seed;
			seed = recent[ 0 ];
			
			recent.clear();
			BioScan::adjacent( seed, previous, recent );
		}
	
	// Depth-first search
	vector< vector< Id > > cstack;
	Id above;
	Id current;
	cstack.resize( 1 );
	cstack[ 0 ].push_back( seed );
	while ( !cstack.empty() ) {
		vector< Id >& top = cstack.back();
		
		if ( top.empty() ) {
			cstack.pop_back();
			if ( !cstack.empty() )
				cstack.back().pop_back();
		} else {
			if ( cstack.size() > 1 )
				above = cstack[ cstack.size() - 2 ].back();
			
			current = top.back();
			compartmentId_.push_back( current );
			
			cstack.resize( cstack.size() + 1 );
			BioScan::adjacent( current, above, cstack.back() );
		}
	}
	
	// Compartments get ordered according to their hines' indices once this
	// list is reversed.
	reverse( compartmentId_.begin(), compartmentId_.end() );
}

void HSolvePassive::initialize( ) {
	double Vm, Cm, Em, Rm, Ra, inject;
	vector< Id >::iterator ic;
	for ( ic = compartmentId_.begin(); ic != compartmentId_.end(); ++ic ) {
		get< double >( ( *ic )(), "initVm", Vm );
		get< double >( ( *ic )(), "Cm", Cm );
		get< double >( ( *ic )(), "Em", Em );
		get< double >( ( *ic )(), "Rm", Rm );
		get< double >( ( *ic )(), "Ra", Ra );
		get< double >( ( *ic )(), "inject", inject );
		
		CompartmentStruct compartment (
			2.0 * Cm / dt_,
			Em / Rm,
			inject
		);
		
		V_.push_back( Vm );
		Ga_.push_back( 2.0 / Ra );
		compartment_.push_back( compartment );
	}
}

void HSolvePassive::storeTree( ) {
	// Create a map from the MOOSE Id to Hines' index.
	map< Id, unsigned int > hinesIndex;
	for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic )
		hinesIndex[ compartmentId_[ ic ] ] = ic;
	
	vector< Id > childId;
	vector< Id >::iterator child;
	
	children_.resize( compartmentId_.size() );
	for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic ) {
		childId.clear();
		// Find current compartment's children
		BioScan::children( compartmentId_[ ic ], childId );
		
		// Push hines' indices of children
		for ( child = childId.begin(); child != childId.end(); ++child )
			children_[ ic ].push_back( hinesIndex[ *child ] );
		
		CmByDt_.push_back( compartment_[ ic ].CmByDt );
	}
}

//////////////////////////////////////////////////////////////////////
// Numerical integration
//////////////////////////////////////////////////////////////////////

void HSolvePassive::forwardEliminate( ) {
	unsigned int ic = 0;
	vector< double >::iterator ihs = HS_.begin();
	vector< double* >::iterator iop = operand_.begin();
	vector< JunctionStruct >::iterator junction;
	
	double *l;
	double *b;
	double pivot;
	unsigned int index;
	unsigned int rank;
	for ( junction = junction_.begin(); junction != junction_.end(); ++junction ) {
		index = junction->index;
		rank = junction->rank;
		
		while ( ic < index ) {
			*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
			*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
			
			++ic, ihs += 4;
		}
		
		pivot = *ihs;
		if ( rank == 1 ) {
			*( ihs + 4 ) -= *( *iop + 1 ) / pivot * **iop;
			*( ihs + 7 ) -= *( *iop + 1 ) / pivot * *( ihs + 3 );
			
			iop += 1;
		} else if ( rank == 2 ) {
			l = *iop;
			b = *( iop + 1 );
			
			*l         -= *( b + 1 ) / pivot * *b;
			*( l + 3 ) -= *( b + 1 ) / pivot * *( ihs + 3 );
			*( l + 4 ) -= *( b + 3 ) / pivot * *( b + 2 );
			*( l + 7 ) -= *( b + 3 ) / pivot * *( ihs + 3 );
			*( b + 4 ) -= *( b + 1 ) / pivot * *( b + 2 );
			*( b + 5 ) -= *( b + 3 ) / pivot * *b;
			
			iop += 3;
		} else {
			vector< double* >::iterator
				end = iop + 3 * rank * ( rank + 1 );
			for ( ; iop < end; iop += 3 )
				**iop -= **( iop + 2 ) / pivot * **( iop + 1 );
		}
		
		++ic, ihs += 4;
	}
	
	while ( ic < nCompt_ ) {
		*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
		*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
		
		++ic, ihs += 4;
	}
}

void HSolvePassive::backwardSubstitute( ) {
	int ic = nCompt_ - 1;
	vector< double >::reverse_iterator ivmid = VMid_.rbegin();
	vector< double >::reverse_iterator iv = V_.rbegin();
	vector< double >::reverse_iterator ihs = HS_.rbegin();
	vector< double* >::reverse_iterator iop = operand_.rbegin();
	vector< double* >::reverse_iterator ibop = backOperand_.rbegin();
	vector< JunctionStruct >::reverse_iterator junction;
	
	*ivmid = *ihs / *( ihs + 3 );
	*iv = 2 * *ivmid - *iv;
	--ic, ++ivmid, ++iv, ihs += 4;
	
	double *b;
	double *v;
	int index;
	int rank;
	for ( junction = junction_.rbegin(); junction != junction_.rend(); ++junction ) {
		index = junction->index;
		rank = junction->rank;
		
		while ( ic > index ) {
			*ivmid = ( *ihs - *( ihs + 2 ) * *( ivmid - 1 ) ) / *( ihs + 3 );
			*iv = 2 * *ivmid - *iv;
			
			--ic, ++ivmid, ++iv, ihs += 4;
		}
		
		if ( rank == 1 ) {
			*ivmid = ( *ihs - **iop * *( ivmid - 1 ) ) / *( ihs + 3 );
			
			iop += 1;
		} else if ( rank == 2 ) {
			v = *iop;
			b = *( iop + 1 );
			
			*ivmid = ( *ihs
			           - *b * *v
			           - *( b + 2 ) * *( v + 1 )
			         ) / *( ihs + 3 );
			
			iop += 3;
		} else {
			*ivmid = *ihs;
			for ( int i = 0; i < rank; ++i ) {
				*ivmid -= **ibop * **( ibop + 1 );
				ibop += 2;
			}
			*ivmid /= *( ihs + 3 );
			
			iop += 3 * rank * ( rank + 1 );
		}
		
		*iv = 2 * *ivmid - *iv;
		--ic, ++ivmid, ++iv, ihs += 4;
	}
	
	while ( ic >= 0 ) {
		*ivmid = ( *ihs - *( ihs + 2 ) * *( ivmid - 1 ) ) / *( ihs + 3 );
		*iv = 2 * *ivmid - *iv;
		
		--ic, ++ivmid, ++iv, ihs += 4;
	}
}

///////////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

#include "../element/Neutral.h"
#include "../utility/utility.h"
#include <sstream>
#include "TestHSolve.h"

/**
 *  We test if the Hines' matrix is correctly setup for the following cell:
 * 
 *   Soma--->  15 - 14 - 13 - 12
 *              |    |
 *              |    L 11 - 10
 *              |
 *              L 16 - 17 - 18 - 19
 *                      |
 *                      L 9 - 8 - 7 - 6 - 5
 *                      |         |
 *                      |         L 4 - 3
 *                      |
 *                      L 2 - 1 - 0
 * 
 *  The numbers are the hines indices of compartments. Compartment X is the
 *  child of compartment Y if X is one level further away from the soma (#15)
 *  than Y. So #17 is the parent of #'s 2, 9 and 18.
 */
void testHSolvePassive()
{
	cout << "\nTesting HSolvePassive" << flush;
	
	bool success;
	unsigned int i;
	unsigned int j;
	unsigned int NCOMPT = 20;
	vector< double > Ga;
	vector< double > CmByDt;
	double dt = 50e-6;
	
	// The i-th entry in 'children' is the list of children of the i-th compt.
	vector< vector< unsigned int > > children;
	
	/* The i-th entry in 'adjacent' is the list of adjacent (children and parent)
	 * compts of the i-th compt.
	 */
	vector< vector< unsigned int > > adjacent;
	
	/* Each entry in 'coupling' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupling;
	
	/* This is the full reference matrix which will be compared to its sparse
	 *  implementation.
	 */
	vector< vector< double > > matrix;
	
	int ch_array[ ] =
	{
		/* c0  */    -1, 
		/* c1  */    -1, 0,
		/* c2  */    -1, 1,
		/* c3  */    -1, 
		/* c4  */    -1, 3,
		/* c5  */    -1, 
		/* c6  */    -1, 5,
		/* c7  */    -1, 4, 6,
		/* c8  */    -1, 7,
		/* c9  */    -1, 8,
		/* c10 */    -1, 
		/* c11 */    -1, 10,
		/* c12 */    -1, 
		/* c13 */    -1, 12,
		/* c14 */    -1, 11, 13,
		/* c15 */    -1, 14, 16,
		/* c16 */    -1, 17,
		/* c17 */    -1, 2, 9, 18,
		/* c18 */    -1, 19,
		/* c19 */    -1, 
	};
	
	int ad_array[ ] =
	{
		/* c0  */    -1, 1,
		/* c1  */    -1, 0, 2,
		/* c2  */    -1, 1, 17,
		/* c3  */    -1, 4,
		/* c4  */    -1, 3, 7,
		/* c5  */    -1, 6,
		/* c6  */    -1, 5, 7,
		/* c7  */    -1, 4, 6, 8,
		/* c8  */    -1, 7, 9,
		/* c9  */    -1, 8, 17,
		/* c10 */    -1, 11,
		/* c11 */    -1, 10, 14,
		/* c12 */    -1, 13,
		/* c13 */    -1, 12, 14,
		/* c14 */    -1, 11, 13, 15,
		/* c15 */    -1, 14, 16,
		/* c16 */    -1, 15, 17,
		/* c17 */    -1, 2, 9, 16, 18,
		/* c18 */    -1, 17, 19,
		/* c19 */    -1, 18,
	};
	
	int co_array[ ] =
	{
		-1, 0, 1,
		-1, 1, 2,
		-1, 2, 9, 17, 18,
		-1, 3, 4,
		-1, 4, 6, 7,
		-1, 5, 6,
		-1, 7, 8,
		-1, 8, 9,
		-1, 10, 11,
		-1, 11, 13, 14,
		-1, 12, 13,
		-1, 14, 15, 16,
		-1, 16, 17,
		-1, 18, 19
	};
	
	for ( i = 0; i < NCOMPT; i++ ) {
		Ga.push_back( 15.0 + 3.0 * i );
		CmByDt.push_back( 500.0 + 200.0 * i * i );
	}
	
	array2vec( ch_array, sizeof( ch_array ) / sizeof( int ), children );
	array2vec( ad_array, sizeof( ad_array ) / sizeof( int ), adjacent );
	array2vec( co_array, sizeof( co_array ) / sizeof( int ), coupling );
	
	Element* n =
		Neutral::create( "Neutral", "n", Element::root()->id(), Id::scratchId() );
	
	vector< Id > c( NCOMPT );
	for ( i = 0; i < NCOMPT; i++ ) {
		ostringstream name;
		name << "c" << i;
		c[ i ] = Neutral::create( "Compartment", name.str(), n->id(),
			Id::scratchId() )->id();
		
		set< double >( c[ i ](), "Ra", 2.0 / Ga[ i ] );
		set< double >( c[ i ](), "Cm", CmByDt[ i ] * ( dt / 2 ) );
	}
	
	for ( i = 0; i < NCOMPT; i++ ) {
		vector< unsigned int >& child = children[ i ];
		for ( j = 0; j < child.size(); j++ ) {
			success = Eref( c[ i ]() ).add( "axial", c[ child[ j ] ](), "raxial" );
			ASSERT( success, "Creating test model" );
		}
	}
	
	HSolvePassive HP;
	HP.setup( c[ 2 ], dt );
	
	vector< Id >& hc = HP.compartmentId_;
	ASSERT( hc.size() == NCOMPT, "Tree traversal" );
	for ( i = 0; i < NCOMPT; i++ )
		ASSERT(
			find( hc.begin(), hc.end(), c[ i ] ) != hc.end(), "Tree traversal"
		);
	
	vector< unsigned int > permutation( NCOMPT );
	for ( i = 0; i < NCOMPT; i++ ) {
		unsigned int newIndex =
			find( hc.begin(), hc.end(), c[ i ] ) - hc.begin();
		permutation[ i ] = newIndex;
	}
	
	permute< double >( Ga, permutation );
	permute< double >( CmByDt, permutation );
	permute< vector< unsigned int > >( children, permutation );
	
	updateIndices( children, permutation );
	updateIndices( coupling, permutation );
	
	makeFullMatrix(	NCOMPT, children, coupling, Ga, CmByDt, matrix );
	
	for ( i = 0; i < NCOMPT; ++i )
		for ( j = 0; j < NCOMPT; ++j ) {
			ostringstream error;
			error << "Testing matrix construction: (" << i << ", " << j << ")";
			ASSERT(
				isClose< double >( matrix[ i ][ j ], HP.entry( i, j ), 1.0 ),
				error.str()
			);
		}
	
	// cleanup
	set( n, "destroy" );
}

#endif // DO_UNIT_TESTS
