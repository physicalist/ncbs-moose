#ifdef DO_UNIT_TESTS

#include "testGpuKernels.h"
#include "GpuInterface.h"
#include "GpuKernels.h"

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

const double tolerance = 1.0;

void testHSonly()
{
	// Fabricate a GpuDataStruct for testing purposes and check whether or
	// not forward elimination and backward substitution occur properly.

	GpuDataStruct d;

	// First create a test matrix.
	const int n = 4;				// Start with a 4x4 matrix
	double testHS[] = {	2, 1, 0, 4,
						3, 1, 0, 10,
						4, 1, 0, 18,
						5, 0, 0, 23 };	// Equivalent of HS => tridiagonal
	double V[] = { 0, 0, 0, 0 };
	double VMid[] = { 0, 0, 0, 0 };

	// Allocate memory as required for d.
	cudaMalloc( (void **)&d.HS, 4 * n * sizeof(double) );
	d.HJ = NULL;
	cudaMalloc( (void **)&d.V, n * sizeof(double) );
	cudaMalloc( (void **)&d.VMid, n * sizeof(double) );
	d.HJCopy = NULL;
	d.operand = NULL;		// Not required if there are no junctions!
	d.backOperand = NULL;	// Likewise!
	d.compartment = NULL;	// Not used by fwdElim and bwdSubs.
	d.junction = NULL;		// No junctions for now.

	d.nCompts = n;
	d.HJSize = 0;
	d.operandSize = 0;
	d.backOperandSize = 0;
	d.junctionSize = 0;

	// Copy HS data into d.
	cudaMemcpy( d.HS, testHS, 4 * n * sizeof(double), cudaMemcpyHostToDevice );
	cudaMemcpy( d.V, V, n * sizeof(double), cudaMemcpyHostToDevice );
	cudaMemcpy( d.VMid, VMid, n * sizeof(double), cudaMemcpyHostToDevice );

	// Call forward eliminate and backward substitute.
	dim3 numBlocks(1);
	dim3 numThreads(1);
	cudaStream_t stream;
	cudaStreamCreate( &stream );
	forwardEliminateKernel<<< numBlocks, numThreads >>>(d);

	// Check the matrix after forward eliminate
	cudaMemcpy( testHS, d.HS, 4 * n * sizeof(double), cudaMemcpyDeviceToHost );
	//std::cout << "HS after forward eliminate" << std::endl;
	int i;
	for( i = 0 ; i < 4 * n ; i++ ) {
		//std::cout << testHS[i] << " ";
		if( i % 4 == 3 ) {
			//std::cout << std::endl;
		}
	}

	backwardSubstituteKernel<<< numBlocks, numThreads >>>(d);

	// Copy results back into CPU
	cudaMemcpy( V, d.V, n * sizeof(double), cudaMemcpyDeviceToHost );
	cudaMemcpy( VMid, d.VMid, n * sizeof(double), cudaMemcpyDeviceToHost );

	//std::cout << "V" << std::endl;
	for( i = 0 ; i < n ; i++ ) {
		//std::cout << V[i] << " ";
	}

	//std::cout << std::endl << "VMid" << std::endl;
	for( i = 0 ; i < n ; i++ ) {
		//std::cout << VMid[i] << " ";
	}

	ASSERT( isClose( VMid[0], 1.0, tolerance ), "Error: testHSonly, VMid[0]");
	ASSERT( isClose( VMid[1], 2.0, tolerance ), "Error: testHSonly, VMid[1]");
	ASSERT( isClose( VMid[2], 3.0, tolerance ), "Error: testHSonly, VMid[2]");
	ASSERT( isClose( VMid[3], 4.0, tolerance ), "Error: testHSonly, VMid[3]");

	std::cout << ".";
}

void testYcompt()
{
	// Fabricate a GpuDataStruct for testing purposes and check whether or
	// not forward elimination and backward substitution occur properly.

	// The network in question is a simple Y-network with three compartments.

	GpuDataStruct d;

	// First create a test matrix along with associated data structures.
	const int n = 3;				// Start with a 3x3 matrix
	double testHS[] = {	1, 0, 0, -4,
						2, 0, 0, 0,
						3, 0, 0, 6 };	// Equivalent of HS => tridiagonal
	double testHJ[] = { -1, -1, -1, -1, -1, -1 };
	double **testOperand = new double*[8];
	JunctionStruct testJunction[2] = { JunctionStruct::JunctionStruct(0, 2),
									   JunctionStruct::JunctionStruct(1, 1) };
	double V[] = { 0, 0, 0 };
	double VMid[] = { 0, 0, 0 };

	// Allocate memory as required for d.
	cudaMalloc( (void **)&d.HS, 4 * n * sizeof(double) );
	cudaMalloc( (void **)&d.HJ, n * (n-1) * sizeof(double) );
	cudaMalloc( (void **)&d.V, n * sizeof(double) );
	cudaMalloc( (void **)&d.VMid, n * sizeof(double) );
	cudaMalloc( (void **)&d.HJCopy, n * (n-1) * sizeof(double) );
	cudaMalloc( (void **)&d.operand, 8 * sizeof(double *) ); // XXX n=3 only!
	d.backOperand = NULL;	// Not reqd if junctions have rank < 3
	d.compartment = NULL;	// Not needed unless updateMatrix is being called
	cudaMalloc( (void **)&d.junction, 2 * sizeof(JunctionStruct) );
									// XXX For this network only!

	d.nCompts = n;
	d.HJSize = n * (n-1);
	d.operandSize = 8;		// XXX
	d.backOperandSize = 0;
	d.junctionSize = 2;		// XXX

	// Set up operands
	testOperand[0] = d.HJ;
	testOperand[1] = d.HS + 4;
	testOperand[2] = d.VMid + 1;
	testOperand[3] = d.HS + 8;
	testOperand[4] = d.VMid + 2;
	testOperand[5] = d.HJ + 4;
	testOperand[6] = d.HS + 8;
	testOperand[7] = d.VMid + 2;

	//std::cout << "Addresses:" << std::endl;
	//std::cout << d.HS << " " << testOperand[6] << " " << d.HJ << std::endl;
	//std::cout << "Operand address: " << d.operand << std::endl;
	//std::cout << "V and VMid addresses: " << d.V << " " << d.VMid << std::endl;

	// Copy data into device.
	cudaMemcpy( d.HS, testHS, 4 * n * sizeof(double), cudaMemcpyHostToDevice );
	cudaMemcpy( d.HJ, testHJ, n * (n-1) * sizeof(double),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.operand, testOperand, 8 * sizeof(double *),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.junction, testJunction, 2 * sizeof(JunctionStruct),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.V, V, n * sizeof(double), cudaMemcpyHostToDevice );
	cudaMemcpy( d.VMid, VMid, n * sizeof(double), cudaMemcpyHostToDevice );

	cudaDeviceSynchronize();

	// Call forward eliminate and backward substitute.
	dim3 numBlocks(1);
	dim3 numThreads(1);
	cudaStream_t stream;
	cudaStreamCreate( &stream );
	forwardEliminateKernel<<< numBlocks, numThreads, 0, stream >>>(d);

	// Check the matrix after forward eliminate
	cudaMemcpy( testHS, d.HS, 4 * n * sizeof(double), cudaMemcpyDeviceToHost );
	//std::cout << "HS after forward eliminate" << std::endl;
	int i;
	for( i = 0 ; i < 4 * n ; i++ ) {
		//std::cout << testHS[i] << " ";
		if( i % 4 == 3 ) {
			//std::cout << std::endl;
		}
	}
	cudaMemcpy( testHJ, d.HJ, n * (n-1) * sizeof(double),
				cudaMemcpyDeviceToHost );
	//std::cout << "HJ after forward eliminate" << std::endl;
	for( i = 0 ; i < n * (n-1) ; i++ ) {
		//std::cout << testHJ[i] << " ";
	}
	//std::cout << endl;

	backwardSubstituteKernel<<< numBlocks, numThreads, 0, stream >>>(d);

	// Copy results back into CPU
	cudaMemcpy( V, d.V, n * sizeof(double), cudaMemcpyDeviceToHost );
	cudaMemcpy( VMid, d.VMid, n * sizeof(double), cudaMemcpyDeviceToHost );

	//std::cout << "V" << std::endl;
	for( i = 0 ; i < n ; i++ ) {
		//std::cout << V[i] << " ";
	}

	//std::cout << std::endl << "VMid" << std::endl;
	for( i = 0 ; i < n ; i++ ) {
		//std::cout << VMid[i] << " ";
	}

	ASSERT( isClose( VMid[0], 1.0, tolerance ), "Error: testYcompt, VMid[0]");
	ASSERT( isClose( VMid[1], 2.0, tolerance ), "Error: testYcompt, VMid[1]");
	ASSERT( isClose( VMid[2], 3.0, tolerance ), "Error: testYcompt, VMid[2]");

	std::cout << ".";
}

void testRank3()
{
	// Fabricate a GpuDataStruct for testing purposes and check whether or
	// not forward elimination and backward substitution occur properly.

	// The network in question is a simple Y-network with three compartments.

	GpuDataStruct d;

	// First create a test matrix along with associated data structures.
	const int n = 4;				// Start with a 3x3 matrix
	double testHS[] = {	1, 0, 0, -8,
						2, 0, 0, -4,
						3, 0, 0, 2,
						4, 0, 0, 10 };	// Equivalent of HS => tridiagonal
	double testHJ[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	JunctionStruct testJunction[3] = { JunctionStruct::JunctionStruct(0, 3),
									   JunctionStruct::JunctionStruct(1, 2),
									   JunctionStruct::JunctionStruct(2, 1) };
	double V[] = { 0, 0, 0, 0 };
	double VMid[] = { 0, 0, 0, 0 };

	// Allocate memory as required for d.
	cudaMalloc( (void **)&d.HS, 4 * n * sizeof(double) );
	cudaMalloc( (void **)&d.HJ, n * (n-1) * sizeof(double) );
	cudaMalloc( (void **)&d.V, n * sizeof(double) );
	cudaMalloc( (void **)&d.VMid, n * sizeof(double) );
	cudaMalloc( (void **)&d.HJCopy, n * (n-1) * sizeof(double) );
	cudaMalloc( (void **)&d.operand, 44 * sizeof(double *) ); // XXX n=4 only!
	cudaMalloc( (void **)&d.backOperand, 6 * sizeof(double *) ); // XXX
	d.compartment = NULL;	// Not needed unless updateMatrix is being called
	cudaMalloc( (void **)&d.junction, 3 * sizeof(JunctionStruct) );
									// XXX For this network only!

	d.nCompts = n;
	d.HJSize = n * (n-1);
	d.operandSize = 44;		// XXX
	d.backOperandSize = 6;
	d.junctionSize = 3;		// XXX

	// Set up operands
	double *testOperand[44] = {
							// junction[0]
								d.HS + 4,	// 0
								d.HJ,		// 1
								d.HJ + 1,	// 2
								d.HS + 7,	// 3
								d.HS + 3,	// 4
								d.HJ + 1,	// 5
								d.HS + 8,	// 6
								d.HJ + 2,	// 7
								d.HJ + 3,	// 8
								d.HS + 11,	// 9
								d.HS + 3,	// 10
								d.HJ + 3,	// 11
								d.HS + 12,	// 12
								d.HJ + 4,	// 13
								d.HJ + 5,	// 14
								d.HS + 15,	// 15
								d.HS + 3,	// 16
								d.HJ + 5,	// 17
								d.HJ + 6,	// 18
								d.HJ + 2,	// 19
								d.HJ + 1,	// 20
								d.HJ + 8,	// 21
								d.HJ + 4,	// 22
								d.HJ + 1,	// 23
								d.HJ + 10,	// 24
								d.HJ + 4,	// 25
								d.HJ + 3,	// 26
								d.HJ + 7,	// 27
								d.HJ,		// 28
								d.HJ + 3,	// 29
								d.HJ + 9,	// 30
								d.HJ,		// 31
								d.HJ + 5,	// 32
								d.HJ + 11,	// 33
								d.HJ + 2,	// 34
								d.HJ + 5,	// 35
							// junction[1]
								d.HJ + 6,	// 36
								d.HS + 8,	// 37
								d.VMid + 2,	// 38
								d.HS + 12,	// 39
								d.VMid + 3,	// 40
							// junction[2]
								d.HJ + 10,	// 41
								d.HS + 12,	// 42
								d.VMid + 3,	// 43
							  };

	double *testBackOperand[6] = {
									d.HJ,		// 0
									d.VMid + 1,	// 1
									d.HJ + 2,	// 2
									d.VMid + 2,	// 3
									d.HJ + 4,	// 4
									d.VMid + 3	// 5
								 };

	//std::cout << "Addresses:" << std::endl;
	//std::cout << "HS: " << d.HS << " " << "HJ: " << d.HJ << std::endl;
	//std::cout << "Operand address: " << d.operand << std::endl;
	//std::cout << "V and VMid addresses: " << d.V << " " << d.VMid << std::endl;

	// Copy data into device.
	cudaMemcpy( d.HS, testHS, 4 * n * sizeof(double), cudaMemcpyHostToDevice );
	cudaMemcpy( d.HJ, testHJ, n * (n-1) * sizeof(double),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.operand, testOperand, 44 * sizeof(double *),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.backOperand, testBackOperand, 6 * sizeof(double *),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.junction, testJunction, 3 * sizeof(JunctionStruct),
				cudaMemcpyHostToDevice );
	cudaMemcpy( d.V, V, n * sizeof(double), cudaMemcpyHostToDevice );
	cudaMemcpy( d.VMid, VMid, n * sizeof(double), cudaMemcpyHostToDevice );

	// Call forward eliminate and backward substitute.
	dim3 numBlocks(1);
	dim3 numThreads(1);
	cudaStream_t stream;
	cudaStreamCreate( &stream );
	forwardEliminateKernel<<< numBlocks, numThreads, 0, stream >>>(d);

	// Check the matrix after forward eliminate
	cudaMemcpy( testHS, d.HS, 4 * n * sizeof(double), cudaMemcpyDeviceToHost );
	//std::cout << "HS after forward eliminate" << std::endl;
	int i;
	for( i = 0 ; i < 4 * n ; i++ ) {
		//std::cout << testHS[i] << " ";
		if( i % 4 == 3 ) {
			//std::cout << std::endl;
		}
	}
	cudaMemcpy( testHJ, d.HJ, n * (n-1) * sizeof(double),
				cudaMemcpyDeviceToHost );
	//std::cout << "HJ after forward eliminate" << std::endl;
	for( i = 0 ; i < n * (n-1) ; i++ ) {
		//std::cout << testHJ[i] << " ";
	}
	//std::cout << endl;

	backwardSubstituteKernel<<< numBlocks, numThreads, 0, stream >>>(d);

	// Copy results back into CPU
	cudaMemcpy( V, d.V, n * sizeof(double), cudaMemcpyDeviceToHost );
	cudaMemcpy( VMid, d.VMid, n * sizeof(double), cudaMemcpyDeviceToHost );

	//std::cout << "V" << std::endl;
	for( i = 0 ; i < n ; i++ ) {
		//std::cout << V[i] << " ";
	}

	//std::cout << std::endl << "VMid" << std::endl;
	for( i = 0 ; i < n ; i++ ) {
		//std::cout << VMid[i] << " ";
	}

	ASSERT( isClose( VMid[0], 1.0, tolerance ), "Error: testRank3, VMid[0]");
	ASSERT( isClose( VMid[1], 2.0, tolerance ), "Error: testRank3, VMid[1]");
	ASSERT( isClose( VMid[2], 3.0, tolerance ), "Error: testRank3, VMid[2]");
	ASSERT( isClose( VMid[3], 4.0, tolerance ), "Error: testRank3, VMid[3]");

	std::cout << ".";
}

void testGpuKernels()
{
	std::cout << "Testing GpuKernels: " << std::flush;
	testHSonly();
	testYcompt();
	testRank3();
	std::cout << std::endl;
}

#endif // DO_UNIT_TESTS
