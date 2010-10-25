/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

extern double testGslIntegrator( string modelName, string plotName,
	double plotDt, double simtime );

extern void speedTestMultiNodeIntFireNetwork( 
	unsigned int size, unsigned int runsteps );

void testKsolve()
{
	double ktime1 = testGslIntegrator( "Kholodenko", "conc1/MAPK-PP.Co",
		10.0, 1e4 );
	cout << "Kholodenko		" << ktime1 << endl;
}

void innerBenchmarks( const string& optarg )
{
	char* cwd = get_current_dir_name();
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
	if ( string( "intFire" ) == optarg  )
		speedTestMultiNodeIntFireNetwork( 2048, 2000 );

	cout << "Completed benchmark\n";
}

bool benchmarkTests( int argc, char** argv )
{
	int opt;
	bool doPlot = 0;
	optind = 0; // forces reinit of getopt

	while ( ( opt = getopt( argc, argv, "shin:c:b:B:" ) ) != -1 ) {
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

