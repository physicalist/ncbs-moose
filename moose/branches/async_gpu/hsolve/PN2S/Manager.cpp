///////////////////////////////////////////////////////////
//  Manager.cpp
//  Implementation of the Class Manager
//
//  Created on:      26-Dec-2013 4:18:17 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "Manager.h"

using namespace pn2s;
static vector<models::Model> _models;

static double _dt;
static bool _isInitialized = false;
static DeviceManager _deviceManager;

/**
 * Initialize the manager and set main parameters
 */
Error_PN2S Manager::Setup(double dt){
	_dt = dt;
	_models.clear();
	DeviceManager::CkeckAvailableDevices();
	return  Error_PN2S::NO_ERROR;
}

Error_PN2S Manager::Allocate(){
	if (!_isInitialized)
		_isInitialized = true;

	DeviceManager::Allocate(_models,_dt);
	return  Error_PN2S::NO_ERROR;
}

Error_PN2S Manager::PrepareSolvers(){
	if (!_isInitialized)
		return Error_PN2S::NOT_INITIALIZED_Error;
	DeviceManager::PrepareSolvers();
	return  Error_PN2S::NO_ERROR;
}

bool Manager::IsInitialized(){
	return  _isInitialized;
}

void Manager::InsertModelShape(models::Model &neutral){
	_models.push_back(neutral);
}


///**
// * Create and add a task to scheduler
// */
//hscError Manager::AddInputTask(uint id){
//	//make taskID
//	TaskInfo task;
//	task.modelPack = _solver.LocateDataByID(id);
//
//	//Add to scheduler
//	_scheduler.AddInputTask(task);
//
//	return  NO_ERROR;
//}




/**
 * A time increment process for each object
 */


Error_PN2S Manager::Process(){
	//Process for all devices
	_deviceManager.Process();

	return  Error_PN2S::NO_ERROR;
}


#ifdef DO_UNIT_TESTS0

#include <cassert>
#include <assert.h>
#include "core/models_HHChannel.h"
#include "core/models_Compartment.h"

#ifdef DO_UNIT_TESTS
# define ASSERT( isOK, message ) \
	if ( !(isOK) ) { \
		cerr << "\nERROR: Assert '" << #isOK << "' failed on line " << __LINE__ << "\nin file " << __FILE__ << ": " << message << endl; \
		exit( 1 ); \
	} else { \
		cout << ""; \
	}
#else
# define ASSERT( unused, message ) do {} while ( false )
#endif

#include <limits>
/**
 * Check 2 floating-point numbers for "equality".
 * Algorithm (from Knuth) 'a' and 'b' are close if:
 *      | ( a - b ) / a | < e AND | ( a - b ) / b | < e
 * where 'e' is a small number.
 *
 * In this function, 'e' is computed as:
 * 	    e = tolerance * machine-epsilon
 */
template< class T >
bool isClose( T a, T b, T tolerance )
{
	T epsilon = std::numeric_limits< T >::epsilon();

	if ( a == b )
		return true;

	if ( a == 0 || b == 0 )
		return ( fabs( a - b ) < tolerance * epsilon );

	return (
		fabs( ( a - b ) / a ) < tolerance * epsilon
		&&
		fabs( ( a - b ) / b ) < tolerance * epsilon
	);
}


void makeFullMatrix(
	const vector< models_Compartment >& tree,
	double dt,
	vector< vector< double > >& matrix )
{
	unsigned int size = tree.size();

	/*
	 * Some convenience variables
	 */
	vector< double > CmByDt;
	vector< double > Ga;
	for ( unsigned int i = 0; i < tree.size(); i++ ) {
		CmByDt.push_back( tree[ i ].Cm / ( dt / 2.0 ) );
		Ga.push_back( 2.0 / tree[ i ].Ra );
	}

	/* Each entry in 'coupled' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupled;
	for ( unsigned int i = 0; i < tree.size(); i++ )
		if ( tree[ i ].children.size() >= 1 ) {
			coupled.push_back( tree[ i ].children );
			coupled.back().push_back( i );
		}

	matrix.clear();
	matrix.resize( size );
	for ( unsigned int i = 0; i < size; ++i )
		matrix[ i ].resize( size );

	// Setting diagonal elements
	for ( unsigned int i = 0; i < size; i++ )
		matrix[ i ][ i ] = CmByDt[ i ] + 1.0 / tree[ i ].Rm;

	double gi;
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator ic;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
		double gsum = 0.0;

		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];

		for ( ic = group->begin(); ic != group->end(); ++ic ) {
			gi = Ga[ *ic ];

			matrix[ *ic ][ *ic ] += gi * ( 1.0 - gi / gsum );
		}
	}

	// Setting off-diagonal elements
	double gij;
	vector< unsigned int >::iterator jc;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
		double gsum = 0.0;

		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];

		for ( ic = group->begin(); ic != group->end() - 1; ++ic ) {
			for ( jc = ic + 1; jc != group->end(); ++jc ) {
				gij = Ga[ *ic ] * Ga[ *jc ] / gsum;

				matrix[ *ic ][ *jc ] = -gij;
				matrix[ *jc ][ *ic ] = -gij;
			}
		}
	}
}

void testmanager()
{
	cout << "testmanager" << endl <<flush;

	//Setup Manager
	assert(Manager::Setup(1) == NO_ERROR);


	/*********************
	 * Check Hines Solver
	 *********************/
	vector< int* > childArray;
	vector< unsigned int > childArraySize;

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

	////////////////////////////////////////////////////////////////////////////
	// Run tests
	////////////////////////////////////////////////////////////////////////////
	vector< models_Compartment > tree;
	vector< double > B;
	vector< double > VMid;


	double dt = 1.0;
	/*
	 * This is the full reference matrix which will be compared to its sparse
	 * implementation.
	 */
	vector< vector< double > > matrix;

	unsigned int nCompt;
	int* array;
	unsigned int arraySize;
	unsigned int i;
	unsigned int j;
	for (unsigned int cell = 0; cell < childArray.size(); cell++) {
		array = childArray[ cell ];
		arraySize = childArraySize[ cell ];
		nCompt = count( array, array + arraySize, -1 );

		// Generate model
		int id= cell;
		models neutral(id);
		for (uint i = 0; i < nCompt; ++i) {
			models_Compartment cmp;
			cmp.Ra = 15.0 + 3.0 * i;
			cmp.Rm = 45.0 + 15.0 * i;
			cmp.Cm = 500.0 + 200.0 * i * i;
			cmp.Em = -0.06;
			cmp.Vm =  -0.06 + 0.01 * i ;
			cmp.initVm = cmp.Vm;

			neutral.compts.push_back(cmp);
		}

		int count = -1;
		for (unsigned int a = 0; a < arraySize; a++)
		{
			if (array[a] == -1)
				count++;
			else
			{
				neutral.compts[count].children.push_back(array[a]);
			}
		}
		Manager::InsertModel(neutral);

	}
//Initialize Shell in the MOOSE
//	Id n = shell->doCreate( "Neutral", Id(), "n" );
//
//	vector< Id > c( nCompt );
//	for ( i = 0; i < nCompt; i++ ) {
//		ostringstream name;
//		name << "c" << i;
//		c[ i ] = shell->doCreate( "Compartment", n, name.str() );
//
//		Field< double >::set( c[ i ], "Ra", tree[ i ].Ra );
//		Field< double >::set( c[ i ], "Rm", tree[ i ].Rm );
//		Field< double >::set( c[ i ], "Cm", tree[ i ].Cm );
//		Field< double >::set( c[ i ], "Em", Em[ i ] );
//		Field< double >::set( c[ i ], "initVm", V[ i ] );
//		Field< double >::set( c[ i ], "Vm", V[ i ] );
//	}
//
//	for ( i = 0; i < nCompt; i++ ) {
//		vector< unsigned int >& child = tree[ i ].children;
//		for ( j = 0; j < ( int )( child.size() ); j++ ) {
//			MsgId mid = shell->doAddMsg(
//				"Single", c[ i ], "axial", c[ child[ j ] ], "raxial" );
//			ASSERT( mid != Msg::bad, "Creating test model" );
//		}
//	}
//	/*
//	 * Here we check if the cell was read in correctly by the solver.
//	 * This test only checks if all the created compartments were read in.
//	 * It doesn't check if they have been assigned hines' indices correctly.
//	 */
//	vector< Id >& hc = HP.compartmentId_;
//	ASSERT( ( int )( hc.size() ) == nCompt, "Tree traversal" );
//	for ( i = 0; i < nCompt; i++ )
//		ASSERT(
//			find( hc.begin(), hc.end(), c[ i ] ) != hc.end(), "Tree traversal"
//		);
//	//////////////////////////////////////////
//	// Setup local matrix
//	//////////////////////////////////////////
//
//	/*
//	 * First we need to ensure that the hines' indices for the local model
//	 * and those inside the solver match. If the numbering is different,
//	 * then the matrices will not agree.
//	 *
//	 * In the following, we find out the indices assigned by the solver,
//	 * and impose them on the local data structures.
//	 */
//
//	// Figure out new indices
//	vector< unsigned int > permutation( nCompt );
//	for ( i = 0; i < nCompt; i++ ) {
//		unsigned int newIndex =
//			find( hc.begin(), hc.end(), c[ i ] ) - hc.begin();
//		permutation[ i ] = newIndex;
//	}
//
//	// Shuffle compartment properties according to new order
//	permute< TreeNodeStruct >( tree, permutation );
//	permute< double >( Em, permutation );
//	permute< double >( V, permutation );
//
//	// Update indices of children
//	for ( i = 0; i < nCompt; i++ ) {
//		vector< unsigned int >& child = tree[ i ].children;
//		for ( j = 0; j < ( int )( child.size() ); j++ )
//			child[ j ] = permutation[ child[ j ] ];
//	}

	Manager::Reinit();
	Manager::PrepareSolver();

	for (unsigned int cell = 0; cell < childArray.size(); cell++) {
		// Generate model In CPU
		array = childArray[ cell ];
		arraySize = childArraySize[ cell ];
		nCompt = count( array, array + arraySize, -1 );

		tree.clear();
		tree.resize( nCompt );
		for ( i = 0; i < nCompt; ++i ) {
			tree[ i ] = _models[cell].compts[i];
		}

		// Prepare local matrix
		makeFullMatrix( tree, dt, matrix );
//		_printMatrix(matrix);

		VMid.resize( nCompt );
		B.resize( nCompt );

		vector< vector< double > > matrixCopy;
		matrixCopy.assign( matrix.begin(), matrix.end() );

		//////////////////////////////////////////
		// Run comparisons
		//////////////////////////////////////////
		double tolerance;

		/*
		 * Compare initial matrices
		 */

		tolerance = 2.0;

		for ( i = 0; i < nCompt; ++i )
		{
			for ( j = 0; j < nCompt; ++j ) {
				ostringstream error;
				error << "Testing matrix construction:"
				      << " Cell# " << cell + 1
				      << " A(" << i << ", " << j << ")";
				ASSERT (
					isClose< double >(
							_solver.LocateDataByID(0)->_compsSolver.GetA(cell,i,j),
							matrix[ i ][ j ], tolerance ),
						error.str()
					);
			}
		}

		for(int i = 1; i<1000;i++)
			Manager::Process();
		cout <<"Cell "<<cell<<endl<<flush;
//		tolerance = 4.0; // ratio to machine epsilon
//
//		/*
//		 * First update terms in the equation. This involves setting up the B
//		 * in Ax = B, using the latest voltage values. Also, the coefficients
//		 * stored in A have to be restored to their original values, since
//		 * the matrix is modified at the end of every pass of gaussian
//		 * elimination.
//		 */
//
//		// ..locally..
//		matrix.assign( matrixCopy.begin(), matrixCopy.end() );
//
//		for ( i = 0; i < nCompt; i++ )
//		{
//			B[ i ] = tree[ i ].Vm * tree[ i ].Cm / ( dt / 2.0 ) +
//					tree[ i ].Em / tree[ i ].Rm;
//		}
//
//		// ..and compare B.
//		for ( i = 0; i < nCompt; ++i ) {
//			cout << _solver.LocateDataByID(0)->_compsSolver.GetRHS(cell,i) << endl << flush;
//			cout << B[ i ] << endl << flush;
//			ostringstream error;
//			error << "Updating right-hand side values:"
//				  << " Cell# " << cell + 1
//				  << " B(" << i << ")";
//			ASSERT (
//				isClose< double >(
//						_solver.LocateDataByID(0)->_compsSolver.GetRHS(cell,i),
//						B[ i ],
//						tolerance ),
//					error.str()
//				);
//		}
//		// cleanup
//		shell->doDelete( n );
	}

	//Check Indexes

//	uint id = 10;
//	Manager::Process(id);
	//Check Values

//	int ndev= Device::GetNumberOfActiveDevices();
//	if(ndev > 0)
//	{
//		Device* d = new Device(1);
//		manager.Process(NULL, d);
//		delete d;
//	}

	cout <<"..... "<<endl<<flush;
}

#endif // DO_UNIT_TESTS
