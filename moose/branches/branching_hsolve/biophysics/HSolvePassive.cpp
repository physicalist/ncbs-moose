/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "BioScan.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"

void HSolvePassive::setup( Id seed, double dt ) {
	refresh( );

	dt_ = dt;
	walkTree( seed );
	initialize( );
	storeTree( );
	
	HinesMatrix::setup( tree_, dt_ );
}

void HSolvePassive::solve( ) {
	updateMatrix( );
	forwardEliminate( );
	backwardSubstitute( );
}

//////////////////////////////////////////////////////////////////////
// Setup of data structures
//////////////////////////////////////////////////////////////////////

void HSolvePassive::refresh( ) {
	dt_ = 0.0;
	compartment_.clear( );
	compartmentId_.clear( );
	V_.clear( );
	tree_.clear( );
}

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
	nCompt_ = compartmentId_.size();
	
	double Vm, Cm, Em, Rm, inject;
	
	vector< Id >::iterator ic;
	for ( ic = compartmentId_.begin(); ic != compartmentId_.end(); ++ic ) {
		get< double >( ( *ic )(), "initVm", Vm );
		get< double >( ( *ic )(), "Cm", Cm );
		get< double >( ( *ic )(), "Em", Em );
		get< double >( ( *ic )(), "Rm", Rm );
		get< double >( ( *ic )(), "inject", inject );
		
		CompartmentStruct compartment (
			2.0 * Cm / dt_,
			Em / Rm,
			inject
		);
		
		V_.push_back( Vm );
		compartment_.push_back( compartment );
	}
}

void HSolvePassive::storeTree( ) {
	double Ra, Cm;
	
	// Create a map from the MOOSE Id to Hines' index.
	map< Id, unsigned int > hinesIndex;
	for ( unsigned int ic = 0; ic < nCompt_; ++ic )
		hinesIndex[ compartmentId_[ ic ] ] = ic;
	
	vector< Id > childId;
	vector< Id >::iterator child;
	
	vector< Id >::iterator ic;
	for ( ic = compartmentId_.begin(); ic != compartmentId_.end(); ++ic ) {
		childId.clear();
		
		BioScan::children( *ic, childId );
		get< double >( ( *ic )(), "Ra", Ra );
		get< double >( ( *ic )(), "Cm", Cm );
		
		TreeNode node;
		// Push hines' indices of children
		for ( child = childId.begin(); child != childId.end(); ++child )
			node.children.push_back( hinesIndex[ *child ] );
		
		node.Ra = Ra;
		node.Cm = Cm;
		
		tree_.push_back( node );
	}
}

//////////////////////////////////////////////////////////////////////
// Numerical integration
//////////////////////////////////////////////////////////////////////

void HSolvePassive::updateMatrix( ) {
	HJ_.assign( HJCopy_.begin(), HJCopy_.end() );
	
	vector< double >::iterator ihs = HS_.begin();
	vector< double >::iterator iv  = V_.begin();
	
	vector< CompartmentStruct >::iterator ic;
	for ( ic = compartment_.begin(); ic != compartment_.end(); ++ic ) {
		*( 3 + ihs ) = *iv * ic->CmByDt + ic->EmByRm + ic->inject;
		
		ihs += 4, ++iv;
	}
}

void HSolvePassive::forwardEliminate( ) {
	stage_ = 0;    // Forward elimination begins.
	
	unsigned int ic = 0;
	vector< double >::iterator ihs = HS_.begin();
	vector< vdIterator >::iterator iop = operand_.begin();
	vector< JunctionStruct >::iterator junction;
	
	double pivot;
	double division;
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
			vdIterator j = *iop;
			vdIterator s = *( iop + 1 );
			
			division = *( j + 1 ) / pivot;
			*( s )     -= division * *j;
			*( s + 3 ) -= division * *( ihs + 3 );
			
			iop += 3;
		} else if ( rank == 2 ) {
			vdIterator j = *iop;
			vdIterator s;
			
			s = *( iop + 1 );
			division = *( j + 1 ) / pivot;
			*( s )     -= division * *j;
			*( j + 4 ) -= division * *( j + 2 );
			*( s + 3 ) -= division * *( ihs + 3 );
			
			s = *( iop + 3 );
			division = *( j + 3 ) / pivot;
			*( j + 5 ) -= division * *j;
			*( s )     -= division * *( j + 2 );
			*( s + 3 ) -= division * *( ihs + 3 );
			
			iop += 5;
		} else {
			vector< vdIterator >::iterator
				end = iop + 3 * rank * ( rank + 1 );
			for ( ; iop < end; iop += 3 )
				**iop -= **( iop + 2 ) / pivot * **( iop + 1 );
		}
		
		++ic, ihs += 4;
	}
	
	while ( ic < nCompt_ - 1 ) {
		*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
		*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
		
		++ic, ihs += 4;
	}
	
	stage_ = 1;    // Forward elimination done.
}

void HSolvePassive::backwardSubstitute( ) {
	int ic = nCompt_ - 1;
	vector< double >::reverse_iterator ivmid = VMid_.rbegin();
	vector< double >::reverse_iterator iv = V_.rbegin();
	vector< double >::reverse_iterator ihs = HS_.rbegin();
	vector< vdIterator >::reverse_iterator iop = operand_.rbegin();
	vector< vdIterator >::reverse_iterator ibop = backOperand_.rbegin();
	vector< JunctionStruct >::reverse_iterator junction;
	
	*ivmid = *ihs / *( ihs + 3 );
	*iv = 2 * *ivmid - *iv;
	--ic, ++ivmid, ++iv, ihs += 4;
	
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
			*ivmid = ( *ihs - **iop * **( iop + 2 ) ) / *( ihs + 3 );
			
			iop += 3;
		} else if ( rank == 2 ) {
			vdIterator v0 = *( iop );
			vdIterator v1 = *( iop + 2 );
			vdIterator j  = *( iop + 4 );
			
			*ivmid = ( *ihs
			           - *v0 * *( j + 2 )
			           - *v1 * *j
			         ) / *( ihs + 3 );
			
			iop += 5;
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
	
	stage_ = 2;    // Backward substitution done.
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
	vector< int > N;
	vector< int* > childArray;
	vector< unsigned int > childArraySize;
	
	N.push_back( 20 );
	
	int childArray_1[ ] =
	{
		/* c0  */  -1, 
		/* c1  */  -1, 0,
		/* c2  */  -1, 1,
		/* c3  */  -1, 
		/* c4  */  -1, 3,
		/* c5  */  -1, 
		/* c6  */  -1, 5,
		/* c7  */  -1, 4, 6,
		/* c8  */  -1, 7,
		/* c9  */  -1, 8,
		/* c10 */  -1, 
		/* c11 */  -1, 10,
		/* c12 */  -1, 
		/* c13 */  -1, 12,
		/* c14 */  -1, 11, 13,
		/* c15 */  -1, 14, 16,
		/* c16 */  -1, 17,
		/* c17 */  -1, 2, 9, 18,
		/* c18 */  -1, 19,
		/* c19 */  -1, 
	};
	
	childArray.push_back( childArray_1 );
	childArraySize.push_back( sizeof( childArray_1 ) / sizeof( int ) );
	
	////////////////////////////////////////////////////////////////////////////
	// Run tests
	////////////////////////////////////////////////////////////////////////////
	/*
	 * Solver instance.
	 */
	HSolvePassive HP;
	
	/* 
	 * This is the full reference matrix which will be compared to its sparse
	 * implementation.
	 */
	vector< vector< double > > matrix;
	
	/*
	 * Model details.
	 */
	double dt = 50e-6;
	vector< TreeNode > tree;
	vector< double > Em;
	vector< double > Rm;
	vector< double > B;
	vector< double > V;
	vector< double > VMid;
	
	/*
	 * Loop over cells.
	 */
	int i;
	int j;
	bool success;
	int nCompt;
	int* array;
	unsigned int arraySize;
	for ( unsigned int cell = 0; cell < childArray.size(); cell++ ) {
		nCompt = N[ cell ];
		array = childArray[ cell ];
		arraySize = childArraySize[ cell ];
		
		//////////////////////////////////////////
		// Prepare local information on cell
		//////////////////////////////////////////
		tree.clear();
		tree.resize( nCompt );
		Em.clear();
		Rm.clear();
		V.clear();
		for ( i = 0; i < nCompt; i++ ) {
			tree[ i ].Ra = 15.0 + 3.0 * i;
			tree[ i ].Cm = 500.0 + 200.0 * i * i;
			Em.push_back( -0.06 );
			Rm.push_back( 400.0 + 50 * i * i );
			V.push_back( -0.06 + 0.01 * i );
		}
		
		int count = -1;
		for ( unsigned int a = 0; a < arraySize; a++ )
			if ( array[ a ] == -1 )
				count++;
			else		
				tree[ count ].children.push_back( array[ a ] );
		
		//////////////////////////////////////////
		// Create cell inside moose; setup solver.
		//////////////////////////////////////////
		Element* n =
			Neutral::create( "Neutral", "n", Element::root()->id(), Id::scratchId() );
		
		vector< Id > c( nCompt );
		for ( i = 0; i < nCompt; i++ ) {
			ostringstream name;
			name << "c" << i;
			c[ i ] = Neutral::create( "Compartment", name.str(), n->id(),
				Id::scratchId() )->id();
			
			set< double >( c[ i ](), "Ra", tree[ i ].Ra );
			set< double >( c[ i ](), "Cm", tree[ i ].Cm );
			set< double >( c[ i ](), "Em", Em[ i ] );
			set< double >( c[ i ](), "Rm", Rm[ i ] );
			set< double >( c[ i ](), "initVm", V[ i ] );
		}
		
		for ( i = 0; i < nCompt; i++ ) {
			vector< unsigned int >& child = tree[ i ].children;
			for ( j = 0; j < ( int )( child.size() ); j++ ) {
				success = Eref( c[ i ]() ).add( "axial", c[ child[ j ] ](), "raxial" );
				ASSERT( success, "Creating test model" );
			}
		}
		
		HP.setup( c[ 0 ], dt );
		
		/*
		 * Here we check if the cell was read in correctly by the solver.
		 * This test only checks if all the created compartments were read in.
		 * It doesn't check if they have been assigned hines' indices correctly.
		 */
		vector< Id >& hc = HP.compartmentId_;
		ASSERT( ( int )( hc.size() ) == nCompt, "Tree traversal" );
		for ( i = 0; i < nCompt; i++ )
			ASSERT(
				find( hc.begin(), hc.end(), c[ i ] ) != hc.end(), "Tree traversal"
			);
		
		//////////////////////////////////////////
		// Setup local matrix
		//////////////////////////////////////////
		
		/*
		 * First we need to ensure that the hines' indices for the local model
		 * and those inside the solver match. If the numbering is different,
		 * then the matrices will not agree.
		 * 
		 * In the following, we find out the indices assigned by the solver, 
		 * and impose them on the local data structures.
		 */
		
		// Figure out new indices
		vector< unsigned int > permutation( nCompt );
		for ( i = 0; i < nCompt; i++ ) {
			unsigned int newIndex =
				find( hc.begin(), hc.end(), c[ i ] ) - hc.begin();
			permutation[ i ] = newIndex;
		}
		
		// Shuffle compartment properties according to new order
		permute< TreeNode >( tree, permutation );
		permute< double >( Em, permutation );
		permute< double >( Rm, permutation );
		permute< double >( V, permutation );
		
		// Update indices of children
		for ( i = 0; i < nCompt; i++ ) {
			vector< unsigned int >& child = tree[ i ].children;
			for ( j = 0; j < ( int )( child.size() ); j++ )
				child[ j ] = permutation[ child[ j ] ];
		}
		
		// Create local reference matrix
		makeFullMatrix(	tree, dt, matrix );
		VMid.resize( nCompt );
		B.resize( nCompt );
		for ( i = 0; i < nCompt; i++ )
			B[ i ] =
				V[ i ] * tree[ i ].Cm / ( dt / 2.0 ) +
				Em[ i ] / Rm[ i ];
		
		//////////////////////////////////////////
		// Run comparisons
		//////////////////////////////////////////
		
		/*
		 * Compare initial matrices
		 */
		for ( i = 0; i < nCompt; ++i )
			for ( j = 0; j < nCompt; ++j ) {
				ostringstream error;
				error << "Testing matrix construction: (" << i << ", " << j << ")";
				ASSERT (
					isClose< double >( HP.getA( i, j ), matrix[ i ][ j ], 2.0 ),
					error.str()
				);
			}
		
		/* 
		 * 
		 * Gaussian elimination
		 * 
		 */
		
		/*
		 * First update terms in the equation. This involves setting up the B 
		 * in Ax = B, using the latest voltage values.
		 */
		HP.updateMatrix( );
		
		for ( i = 0; i < nCompt; ++i ) {
			ostringstream error;
			error << "Updating right-hand side values: B(" << i << ")";
			ASSERT (
				isClose< double >( HP.getB( i ), B[ i ], 2.0 ),
				error.str()
			);
		}
		
		/*
		 *  Forward elimination..
		 */
		
		// ..in solver..
		HP.forwardEliminate( );
		
		// ..and locally..
		int k;
		for ( i = 0; i < nCompt - 1; i++ )
			for ( j = i + 1; j < nCompt; j++ ) {
				double div = matrix[ j ][ i ] / matrix[ i ][ i ];
				for ( k = 0; k < nCompt; k++ )
					matrix[ j ][ k ] -= div * matrix[ i ][ k ];
				B[ j ] -= div * B[ i ];
			}
		
		// ..then compare A..
		for ( i = 0; i < nCompt; ++i )
			for ( j = 0; j < nCompt; ++j ) {
				ostringstream error;
				error << "Forward elimination: A(" << i << ", " << j << ")";
				ASSERT (
					isClose< double >( HP.getA( i, j ), matrix[ i ][ j ], 2.0 ),
					error.str()
				);
			}
		
		// ..and also B.
		for ( i = 0; i < nCompt; ++i ) {
			ostringstream error;
			error << "Forward elimination: B(" << i << ")";
			ASSERT (
				isClose< double >( HP.getB( i ), B[ i ], 2.0 ),
				error.str()
			);
		}
		
		/*
		 *  Backward substitution..
		 */
		
		// ..in solver..
		HP.backwardSubstitute( );
		
		// ..and full back-sub on local matrix equation..
		for ( i = nCompt - 1; i >= 0; i-- ) {
			VMid[ i ] = B[ i ];
			
			for ( j = nCompt - 1; j > i; j-- )
				VMid[ i ] -= VMid[ j ] * matrix[ i ][ j ];
			
			VMid[ i ] /= matrix[ i ][ i ];
		}
		
		// ..and then compare VMid.
		for ( i = nCompt - 1; i >= 0; i-- ) {
			ostringstream error;
			error << "Back substitution: VMid(" << i << ")";
			ASSERT (
				isClose< double >( HP.getVMid( i ), VMid[ i ], 2.0 ),
				error.str()
			);
		}
		
		// cleanup
		set( n, "destroy" );
	}
}

#endif // DO_UNIT_TESTS
