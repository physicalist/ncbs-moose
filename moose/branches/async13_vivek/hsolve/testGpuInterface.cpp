#ifdef DO_UNIT_TESTS

#include "cudaLibrary/GpuInterface.h"
#include "../shell/Shell.h"

#include "testGpuInterface.h"

/**
 * This program simply tests whether setup and unsetup are working correctly.
 */
void testGpuInterface()
{
#if 0
	HSolve *hsolve = new HSolve;
	hsolve->nCompt_ = 0;
	hsolve->stage_ = 0;

	// Setup
	GpuInterface gpu( hsolve );

	// Unsetup
	gpu.unsetup();
	ASSERT( hsolve->HS_ == gpu.hsolve_->HS, "Gpu Interface, HS" );
	ASSERT( hsolve->HJ_ == gpu.hsolve_->HJ, "Gpu Interface, HJ" );
	ASSERT( hsolve->HJCopy_ == gpu.hsolve_->HJCopy, "Gpu Interface, HJCopy" );
	ASSERT( hsolve->V_ == gpu.hsolve_->V, "Gpu Interface, V" );
	ASSERT( hsolve->VMid_ == gpu.hsolve_->VMid, "Gpu Interface, VMid" );
#endif

	cout << endl << "Testing GpuInterface: " << flush;

	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );

	vector< int* > childArray;
	vector< unsigned int > childArraySize;

	/**
	 *  We test passive-cable solver for the following cell:
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

	/**
	 *  Cell 2:
	 *
	 *             3
	 *             |
	 *   Soma--->  2
	 *            / \
	 *           /   \
	 *          1     0
	 *
	 */

	int childArray_2[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1,
		/* c2  */  -1, 0, 1, 3,
		/* c3  */  -1,
	};

	childArray.push_back( childArray_2 );
	childArraySize.push_back( sizeof( childArray_2 ) / sizeof( int ) );

	/**
	 *  Cell 3:
	 *
	 *             3
	 *             |
	 *             2
	 *            / \
	 *           /   \
	 *          1     0  <--- Soma
	 *
	 */

	int childArray_3[ ] =
	{
		/* c0  */  -1, 2,
		/* c1  */  -1,
		/* c2  */  -1, 1, 3,
		/* c3  */  -1,
	};

	childArray.push_back( childArray_3 );
	childArraySize.push_back( sizeof( childArray_3 ) / sizeof( int ) );

	/**
	 *  Cell 4:
	 *
	 *             3  <--- Soma
	 *             |
	 *             2
	 *            / \
	 *           /   \
	 *          1     0
	 *
	 */

	int childArray_4[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1,
		/* c2  */  -1, 0, 1,
		/* c3  */  -1, 2,
	};

	childArray.push_back( childArray_4 );
	childArraySize.push_back( sizeof( childArray_4 ) / sizeof( int ) );

	/**
	 *  Cell 5:
	 *
	 *             1  <--- Soma
	 *             |
	 *             2
	 *            / \
	 *           4   0
	 *          / \
	 *         3   5
	 *
	 */

	int childArray_5[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1, 2,
		/* c2  */  -1, 0, 4,
		/* c3  */  -1,
		/* c4  */  -1, 3, 5,
		/* c5  */  -1,
	};

	childArray.push_back( childArray_5 );
	childArraySize.push_back( sizeof( childArray_5 ) / sizeof( int ) );

	/**
	 *  Cell 6:
	 *
	 *             3  <--- Soma
	 *             L 4
	 *               L 6
	 *               L 5
	 *               L 2
	 *               L 1
	 *               L 0
	 *
	 */

	int childArray_6[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1,
		/* c2  */  -1,
		/* c3  */  -1, 4,
		/* c4  */  -1, 0, 1, 2, 5, 6,
		/* c5  */  -1,
		/* c6  */  -1,
	};

	childArray.push_back( childArray_6 );
	childArraySize.push_back( sizeof( childArray_6 ) / sizeof( int ) );

	/**
	 *  Cell 7: Single compartment
	 */

	int childArray_7[ ] =
	{
		/* c0  */  -1,
	};

	childArray.push_back( childArray_7 );
	childArraySize.push_back( sizeof( childArray_7 ) / sizeof( int ) );

	/**
	 *  Cell 8: 3 compartments; soma is in the middle.
	 */

	int childArray_8[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1, 0, 2,
		/* c2  */  -1,
	};

	childArray.push_back( childArray_8 );
	childArraySize.push_back( sizeof( childArray_8 ) / sizeof( int ) );

	/**
	 *  Cell 9: 3 compartments; first compartment is soma.
	 */

	int childArray_9[ ] =
	{
		/* c0  */  -1, 1,
		/* c1  */  -1, 2,
		/* c2  */  -1,
	};

	childArray.push_back( childArray_9 );
	childArraySize.push_back( sizeof( childArray_9 ) / sizeof( int ) );

	////////////////////////////////////////////////////////////////////////////
	// Run tests
	////////////////////////////////////////////////////////////////////////////
	/*
	 * Solver instance.
	 */
	HSolve *hsolve = new HSolve;

	// Initialize the lookup tables in hsolve to zero right now. This should be
	// done in the LookupTable constructor, really.
	hsolve->vTable_.nPts_ = 0;
	hsolve->vTable_.nColumns_ = 0;
	hsolve->caTable_.nPts_ = 0;
	hsolve->caTable_.nColumns_ = 0;

	/*
	 * Model details.
	 */
	double dt = 1.0;
	vector< TreeNodeStruct > tree;
	vector< double > Em;
	vector< double > B;
	vector< double > V;
	vector< double > VMid;

	/*
	 * Loop over cells.
	 */
	int i;
	int j;
	//~ bool success;
	int nCompt;
	int* array;
	unsigned int arraySize;
	for ( int cell = childArray.size()-1; cell >= 0; cell-- ) {
		//cout << "Cell number: " << cell << endl;

		array = childArray[ cell ];
		arraySize = childArraySize[ cell ];
		nCompt = count( array, array + arraySize, -1 );

		//////////////////////////////////////////
		// Prepare local information on cell
		//////////////////////////////////////////
		tree.clear();
		tree.resize( nCompt );
		Em.clear();
		V.clear();
		//cout << "First for loop" << endl;
		for ( i = 0; i < nCompt; i++ ) {
			tree[ i ].Ra = 15.0 + 3.0 * i;
			tree[ i ].Rm = 45.0 + 15.0 * i;
			tree[ i ].Cm = 500.0 + 200.0 * i * i;
			Em.push_back( -0.06 );
			V.push_back( -0.06 + 0.01 * i );
		}

		int count = -1;
		//cout << "Second for loop; arraysize=" << arraySize << endl;
		for ( unsigned int a = 0; a < arraySize; a++ ) {
			if ( array[ a ] == -1 )
				count++;
			else
				tree[ count ].children.push_back( array[ a ] );
		}

		//////////////////////////////////////////
		// Create cell inside moose; setup solver.
		//////////////////////////////////////////
		//cout << "Creating id" << endl;
		Id n = shell->doCreate( "Neutral", Id(), "n" );

		vector< Id > c( nCompt );
		//cout << "Third for loop" << endl;
		for ( i = 0; i < nCompt; i++ ) {
			ostringstream name;
			name << "c" << i;
			c[ i ] = shell->doCreate( "Compartment", n, name.str() );

			Field< double >::set( c[ i ], "Ra", tree[ i ].Ra );
			Field< double >::set( c[ i ], "Rm", tree[ i ].Rm );
			Field< double >::set( c[ i ], "Cm", tree[ i ].Cm );
			Field< double >::set( c[ i ], "Em", Em[ i ] );
			Field< double >::set( c[ i ], "initVm", V[ i ] );
			Field< double >::set( c[ i ], "Vm", V[ i ] );
		}

		//cout << "Fourth for loop" << endl;
		for ( i = 0; i < nCompt; i++ ) {
			vector< unsigned int >& child = tree[ i ].children;
			for ( j = 0; j < ( int )( child.size() ); j++ ) {
				MsgId mid = shell->doAddMsg(
						"Single", c[ i ], "axial", c[ child[ j ] ], "raxial" );
				ASSERT( mid != Msg::bad, "Creating test model" );
			}
		}

		hsolve->HSolvePassive::setup( c[ 0 ], dt );
		HSolve hsolve_copy = *hsolve;

		//cout << "Starting setup" << endl;
		GpuInterface gpu( &hsolve_copy );
		//cout << "Starting unsetup" << endl;
		gpu.unsetup();
		//cout << "Asserting..." << endl;
		ASSERT( hsolve_copy == *hsolve, "GpuInterface setup error" );

		// cleanup
		shell->doDelete( n );

		cout << ".";
	}

	cout << endl;
}

#include <limits>
/**
 * Check 2 floating-point numbers for "equality".
 * Algorithm (from Knuth) 'a' and 'b' are close if:
 *      | ( a - b ) / a | < e AND | ( a - b ) / b | < e
 * where 'e' is a small number.
 *
 * In this function, 'e' is computed as:
 *      e = tolerance * machine-epsilon
 */
template< class T >
bool isClose( T a, T b, T tolerance )
{
	T epsilon = std::numeric_limits< T >::epsilon();

	if ( a == b )
		return true;

	if ( a == 0 || b == 0 )
		return ( fabs( a - b ) < tolerance * epsilon );

	return (    fabs( ( a - b ) / a ) < tolerance * epsilon
			 && fabs( ( a - b ) / b ) < tolerance * epsilon );
}

void testSetupWorking()
{
	cout << "Testing setup working: " << flush;

	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );

	vector< int* > childArray;
	vector< unsigned int > childArraySize;

	/**
	 *  Cell 0: Single compartment
	 */

	int childArray_7[ ] =
	{
		/* c0  */  -1,
	};

	childArray.push_back( childArray_7 );
	childArraySize.push_back( sizeof( childArray_7 ) / sizeof( int ) );

	/**
	 *  Cell 1: 3 compartments, Y configuration
	 *
	 *                  1
	 *                  |
	 *                  |
	 *                 / \
	 *                /   \
	 *               0     2
	 *
	 *  Note that these indices are NOT the Hines indices! The Hines indices
	 *  are in fact the reverse, since we shall do a depth-first-search
	 *  starting with the compartment labelled here as 0, which will then
	 *  receive the largest Hines index.
	 */

	int childArray_8[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1, 0, 2,
		/* c2  */  -1,
	};

	childArray.push_back( childArray_8 );
	childArraySize.push_back( sizeof( childArray_8 ) / sizeof( int ) );

	/**
	 *  Cell 9: 3 compartments; series configuration
	 *
	 *                  0
	 *                  |
	 *                  1
	 *                  |
	 *                  2
	 */

	int childArray_9[ ] =
	{
		/* c0  */  -1, 1,
		/* c1  */  -1, 2,
		/* c2  */  -1,
	};

	childArray.push_back( childArray_9 );
	childArraySize.push_back( sizeof( childArray_9 ) / sizeof( int ) );

	////////////////////////////////////////////////////////////////////////////
	// Run tests
	////////////////////////////////////////////////////////////////////////////
	/*
	 * Solver instance.
	 */
	HSolve *hsolve = new HSolve;

	// Initialize the lookup tables in hsolve to zero right now. This should be
	// done in the LookupTable constructor, really.
	hsolve->vTable_.nPts_ = 0;
	hsolve->vTable_.nColumns_ = 0;
	hsolve->caTable_.nPts_ = 0;
	hsolve->caTable_.nColumns_ = 0;

	/*
	 * Model details.
	 */
	double dt = 1.0;
	double tolerance = 1.0;			// Tolerance for double comparisons: gets
									// multiplied with numeric_limits::epsilon
	vector< TreeNodeStruct > tree;
	vector< double > Em;
	vector< double > B;
	vector< double > V;
	vector< double > VMid;

	/*
	 * Loop over cells.
	 */
	int i;
	int j;
	//~ bool success;
	int nCompt;
	int* array;
	unsigned int arraySize;
	for ( int cell = childArray.size()-1; cell >= 0; cell-- ) {
		//cout << "Cell number: " << cell << endl;

		array = childArray[ cell ];
		arraySize = childArraySize[ cell ];
		nCompt = count( array, array + arraySize, -1 );

		//////////////////////////////////////////
		// Prepare local information on cell
		//////////////////////////////////////////
		tree.clear();
		tree.resize( nCompt );
		Em.clear();
		V.clear();
		//cout << "First for loop" << endl;
		for ( i = 0; i < nCompt; i++ ) {
			tree[ i ].Ra = 2.0;
			tree[ i ].Rm = 0.5 + 0.5 * i;
			tree[ i ].Cm = 2.0 + 1.0 * i * i;
			Em.push_back( -0.06 );
			V.push_back( -0.06 + 0.01 * i );
		}

		int count = -1;
		//cout << "Second for loop; arraysize=" << arraySize << endl;
		for ( unsigned int a = 0; a < arraySize; a++ ) {
			if ( array[ a ] == -1 )
				count++;
			else
				tree[ count ].children.push_back( array[ a ] );
		}

		//////////////////////////////////////////
		// Create cell inside moose; setup solver.
		//////////////////////////////////////////
		//cout << "Creating id" << endl;
		Id n = shell->doCreate( "Neutral", Id(), "n" );

		vector< Id > c( nCompt );
		//cout << "Third for loop" << endl;
		for ( i = 0; i < nCompt; i++ ) {
			ostringstream name;
			name << "c" << i;
			c[ i ] = shell->doCreate( "Compartment", n, name.str() );

			Field< double >::set( c[ i ], "Ra", tree[ i ].Ra );
			Field< double >::set( c[ i ], "Rm", tree[ i ].Rm );
			Field< double >::set( c[ i ], "Cm", tree[ i ].Cm );
			Field< double >::set( c[ i ], "Em", Em[ i ] );
			Field< double >::set( c[ i ], "initVm", V[ i ] );
			Field< double >::set( c[ i ], "Vm", V[ i ] );
		}

		//cout << "Fourth for loop" << endl;
		for ( i = 0; i < nCompt; i++ ) {
			vector< unsigned int >& child = tree[ i ].children;
			for ( j = 0; j < ( int )( child.size() ); j++ ) {
				MsgId mid = shell->doAddMsg(
						"Single", c[ i ], "axial", c[ child[ j ] ], "raxial" );
				ASSERT( mid != Msg::bad, "Creating test model" );
			}
		}

		hsolve->HSolvePassive::setup( c[ 0 ], dt );

		//cout << "Starting setup" << endl;
		GpuInterface gpu( hsolve );

		// Manually check each setup data structure, especially operands
		if ( cell == 0 ) {
			// Single compartment

			// First check sizes of various data elements.
			ASSERT( gpu.data_.nCompts == 1, "Setup error: cell 0, nCompts" );
			ASSERT( gpu.data_.HJSize == 0, "Setup error: cell 0, HJSize" );
			ASSERT( gpu.data_.operandSize == 0,
					"Setup error: cell 0, operandSize" );
			ASSERT( gpu.data_.backOperandSize == 0,
					"Setup error: cell 0, backOperandSize" );
			ASSERT( gpu.data_.junctionSize == 0,
					"Setup error: cell 0, junctionSize" );

			// This tests setup and unsetup in one fell swoop!
			// Unless both setup and unsetup are *silently* failing. In which
			// case, this entire test case is only telling you whether or not
			// HSolvePassive::setup is working correctly :P
			gpu.unsetup();

			// Check other elements one by one.
			double testHS[] = { 6.0, 0, 6.0, 0 };
			for ( i = 0 ; i < 4 ; ++i ) {
				ASSERT( isClose( gpu.hsolve_->HS_[i], testHS[i], tolerance ),
						"Setup error: cell 1, HS" );
			}

			ASSERT( gpu.data_.HJ == NULL, "Setup error: cell 0, HJ" );
			ASSERT( gpu.data_.HJCopy == NULL, "Setup error: cell 0, HJCopy" );
			ASSERT( gpu.data_.operand == NULL,
					"Setup error: cell 0, operand" );
			ASSERT( gpu.data_.backOperand == NULL,
					"Setup error: cell 0, backOperand" );
			ASSERT( gpu.data_.junction == NULL,
					"Setup error: cell 0, junction" );
			ASSERT( gpu.hsolve_->V_[0] == -0.06, "Setup error: cell 0, V" );

		}
		else if ( cell == 1 ) {
			// Y configuration

			// First check sizes of various data elements.
			ASSERT( gpu.data_.nCompts == 3, "Setup error: cell 1, nCompts" );
			ASSERT( gpu.data_.HJSize == 6, "Setup error: cell 1, HJSize" );
			ASSERT( gpu.data_.operandSize == 8,
					"Setup error: cell 1, operandSize" );
			ASSERT( gpu.data_.backOperandSize == 0,
					"Setup error: cell 1, backOperandSize" );
			ASSERT( gpu.data_.junctionSize == 2,
					"Setup error: cell 1, junctionSize" );

			gpu.unsetup();

			double testHS[] = {
				13.0 + 1 / 3.0,		0,		13.0 + 1 / 3.0,		0,
				7.0 + 2 / 3.0,		0,		7.0 + 2 / 3.0,		0,
				6.0 + 2 / 3.0,		0,		6.0 + 2 / 3.0,		0,
			};
			for ( i = 0 ; i < 12 ; ++i ) {
				ASSERT( isClose( gpu.hsolve_->HS_[i], testHS[i], tolerance ),
						"Setup error: cell 1, HS" );
			}

			double testHJ[] = { -1 / 3.0, -1 / 3.0, -1 / 3.0,
								-1 / 3.0, -1 / 3.0, -1 / 3.0 };
			for ( i = 0 ; i < 6 ; ++i ) {
				ASSERT( isClose( gpu.hsolve_->HJ_[i], testHJ[i], tolerance ),
						"Setup error: cell 1, HJ" );
			}

			double testV[] = { -0.04, -0.05, -0.06 };
			for ( i = 0 ; i < 3 ; ++i ) {
				ASSERT( isClose( gpu.hsolve_->V_[i], testV[i], tolerance ),
						"Setup error: cell 1, V" );
			}

			GpuDataStruct& d = gpu.data_;
			double **testOperand = new double*[8];
			testOperand[0] = d.HJ;
			testOperand[1] = d.HS + 4;
			testOperand[2] = d.VMid + 1;
			testOperand[3] = d.HS + 8;
			testOperand[4] = d.VMid + 2;
			testOperand[5] = d.HJ + 4;
			testOperand[6] = d.HS + 8;
			testOperand[7] = d.VMid + 2;
			vector< double* > operand;
			operand.assign( testOperand, testOperand + 8 );
			ASSERT( gpu.operand_ == operand, "Setup error: cell 1, operand");

		}
		else if ( cell == 2 ) {
			// Series configuration

			// First check sizes of various data elements.
			ASSERT( gpu.data_.nCompts == 3, "Setup error: cell 1, nCompts" );
			ASSERT( gpu.data_.HJSize == 0, "Setup error: cell 1, HJSize" );
			ASSERT( gpu.data_.operandSize == 0,
					"Setup error: cell 1, operandSize" );
			ASSERT( gpu.data_.backOperandSize == 0,
					"Setup error: cell 1, backOperandSize" );
			ASSERT( gpu.data_.junctionSize == 0,
					"Setup error: cell 1, junctionSize" );

			gpu.unsetup();

			double testHS[] = {
				12.5 + 2 / 3.0,		-0.5,		12.5 + 2 / 3.0,		0,
				8.0,				-0.5,		8.0,				0,
				6.5,				0,			6.5,				0,
			};
			for ( i = 0 ; i < 12 ; ++i ) {
				ASSERT( isClose( gpu.hsolve_->HS_[i], testHS[i], tolerance ),
						"Setup error: cell 1, HS" );
			}

			double testV[] = { -0.04, -0.05, -0.06 };
			for ( i = 0 ; i < 3 ; ++i ) {
				ASSERT( isClose( gpu.hsolve_->V_[i], testV[i], tolerance ),
						"Setup error: cell 1, V" );
			}

			ASSERT( gpu.data_.HJ == NULL, "Setup error: cell 0, HJ" );
			ASSERT( gpu.data_.HJCopy == NULL, "Setup error: cell 0, HJCopy" );
			ASSERT( gpu.data_.operand == NULL,
					"Setup error: cell 0, operand" );
			ASSERT( gpu.data_.backOperand == NULL,
					"Setup error: cell 0, backOperand" );
			ASSERT( gpu.data_.junction == NULL,
					"Setup error: cell 0, junction" );

		}

		// cleanup
		shell->doDelete( n );

		cout << ".";
	}

	cout << endl;
}

#endif // DO_UNIT_TESTS
