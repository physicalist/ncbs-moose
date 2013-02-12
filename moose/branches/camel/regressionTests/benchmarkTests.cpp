/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include  <unistd.h>

extern double testGslIntegrator( string modelName, string plotName,
	double plotDt, double simtime );

extern void speedTestMultiNodeIntFireNetwork( 
	unsigned int size, unsigned int runsteps );

extern void rtHHnetwork( unsigned int numCopies );

extern void perfTestMarkovSolver();			

extern void testGsolver( string modelName, string plotName,
	double plotDt, double simtime );

// Defined in testMsg.cpp
extern void benchmarkMsg( unsigned int, string msgType);			

void testKsolve()
{
	double ktime1 = testGslIntegrator( "Kholodenko", "conc1/MAPK-PP.Co",
		10.0, 1e6 );
	cout << "Kholodenko		" << ktime1 << endl;
}

void innerBenchmarks( const string& optarg )
{
	//char* cwd = get_current_dir_name();
	// get_current_dir_name is not available on all platforms
	char *cwd = getcwd(NULL,0); // same behaviour but this also resolves symbolic links
	string currdir = cwd;
	free( cwd );
	if ( currdir.substr( currdir.find_last_of( "/" ) ) != 
		"/regressionTests" ) {
		cout << "\nBenchmarks can only be run from regression test directory.\n";
		return;
	}

	cout << "\nBenchmark Tests:";
	if ( string( "ksolve" ) == optarg  )
		testKsolve();
	else if ( string( "intFire" ) == optarg  )
		speedTestMultiNodeIntFireNetwork( 2048, 2000 );
	else if ( string( "hhNet" ) == optarg  )
		rtHHnetwork( 1000 );
	else if ( string("markovSolver") == optarg )
		perfTestMarkovSolver( );
	else if ( optarg.substr( 0, 3 ) == "msg" ) {
		// split optarg here
		string::size_type pos = optarg.find_first_of( '_' );
		assert ( pos != string::npos );
		string::size_type pos2 = optarg.find_last_of( '_' );
		unsigned int numObj = atoi( optarg.substr( pos2 + 1, string::npos ).c_str() );
		string msgType = optarg.substr( pos + 1, pos2 - pos - 1 );
		benchmarkMsg( numObj, msgType );
	}

	cout << "Completed benchmark\n";
}

bool benchmarkTests( int argc, char** argv )
{
	int opt;
	bool doPlot = 0;
	optind = 0; // forces reinit of getopt

	while ( ( opt = getopt( argc, argv, "shiqurn:t:b:B:" ) ) != -1 ) {
		switch ( opt ) {
			case 'B': // Benchmark
				doPlot = 1; // fall through to case b.
			case 'b': // Benchmark
				innerBenchmarks( optarg );
				return 1;
				break;
		}
	}
	return 0;
}

