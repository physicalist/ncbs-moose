/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * \mainpage MOOSE Code documentation: Generated by Doxygen.
 * 
 * \section intro_sec Introduction
 * MOOSE is the Multiscale Object Oriented Simulation Environment.
 * This Doxygen-generated set of pages documents the source code of
 * MOOSE.
 *
 */

#include <iostream>
#include <basecode/header.h>
#include <basecode/moose.h>
#include <element/Neutral.h>
#include <basecode/IdManager.h>
#include <utility/utility.h>
#include "shell/Shell.h"

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif //USE_READLINE

extern void initMoose();
extern void initSched();

// Defined in mpiSetup.cpp
extern unsigned int initMPI( int argc, char** argv );
extern void initParSched();
extern void terminateMPI( unsigned int myNode );
extern void pollPostmaster();  // Defined in mpiSetup.cpp
extern void setupDefaultSchedule(Element* t0, Element* t1, Element* cj);

#ifdef DO_UNIT_TESTS
	extern void testBasecode();
	extern void testNeutral();
	extern void testSparseMatrix();
	extern void testShell();
	extern void testInterpol();
	extern void testTable();
	extern void testWildcard();
	extern void testSched();
	extern void testSchedProcess();
	extern void testBiophysics();
	extern void testKinetics();
//	extern void testAverage();
	extern void testParMsgOnSingleNode();
	extern void testPostMaster();
#endif

#ifdef USE_GENESIS_PARSER
	extern Element* makeGenesisParser( );
//	extern void nonblock( int state );
	extern bool nonBlockingGetLine( string& s );
#endif


int main(int argc, char** argv)
{
	// TODO : check the repurcussions of MPI command line
	ArgParser::parseArguments(argc, argv);

	Property::initialize(ArgParser::getConfigFile(),Property::PROP_FORMAT);
	PathUtility simpathHandler(ArgParser::getSimPath());
	simpathHandler.addPath(Property::getProperty(Property::SIMPATH)); // merge the SIMPATH from command line and property file
	Property::setProperty(Property::SIMPATH, simpathHandler.getAllPaths()); // put the updated path list in Property
	cout << "SIMPATH = " << Property::getProperty(Property::SIMPATH) << endl;
        
	///////////////////////////////////////////////////////////////////
	//	Initialization functions. Must be in this order.
	///////////////////////////////////////////////////////////////////
	unsigned int myNode = initMPI( argc, argv );
	initMoose();
	initSched();
	initParSched();
        
#ifdef DO_UNIT_TESTS
	if ( myNode == 0 )
	{
		/*
		testBasecode();
		testNeutral();
		testSparseMatrix();
		testShell();
		testWildcard();
		testInterpol();
		testTable();
		testSched();
		testSchedProcess();
		testBiophysics();
		testKinetics();
		*/
//		testAverage();
		testParMsgOnSingleNode();
	}
	// This is a special unit test: it MUST run on all nodes.
	testPostMaster(); // This is a dummy if no postMaster exists.
#endif


#ifdef USE_GENESIS_PARSER
	if ( myNode == 0 ) {
		string line = "";
                vector<string> scriptArgs = ArgParser::getScriptArgs();
                
                if ( scriptArgs.size() > 0 )
                {
                    line = "include";
                    for ( unsigned int i = 0; i < scriptArgs.size(); ++i )
                    {
                        line = line + " " + scriptArgs[i];
                    }
                    line.push_back('\n');
                }
                
		Element* sli = makeGenesisParser();
		assert( sli != 0 );
		// Need to do this before the first script is loaded, but
		// after the unit test for the parser.
                Id cj("/sched/cj");
                Id t0("/sched/cj/t0");
                Id t1("/sched/cj/t1");
                
		// Doesn't do much. Just sets dt and stage, and cals reset.
		setupDefaultSchedule( t0(), t1(), cj() );

		const Finfo* parseFinfo = sli->findFinfo( "parse" );
		assert ( parseFinfo != 0 );

		set< string >( sli, parseFinfo, line );
		set< string >( sli, parseFinfo, "\n" );

		/**
		 * Here is the key infinite loop for getting terminal input,
		 * parsing it, polling postmaster, managing GUI and other 
		 * good things.
		 */
		string s = "";
		unsigned int lineNum = 0;
		#ifndef USE_READLINE
		cout << "moose #" << lineNum << " > " << flush;
		#endif //n USE_READLINE
		while( Shell::running() ) {
			#ifdef USE_READLINE
			char cname[10];
			sprintf(cname, "moose #%d > ", lineNum);
			char * foo  = readline(cname);
			s = string (foo);
			set< string >( sli, parseFinfo, s );
			if ( s.find_first_not_of( " \t\n" ) != s.npos ){
				lineNum++;
				add_history(foo);
			}
			s = "";
			#else
			if ( nonBlockingGetLine( s ) ) {
				set< string >( sli, parseFinfo, s );
				if ( s.find_first_not_of( " \t\n" ) != s.npos )
					lineNum++;
				s = "";
				cout << "moose #" << lineNum << " > " << flush;
			}
			#endif //USE_READLINE
			pollPostmaster();
			
			// gui stuff here maybe.
		}
	} else { // All but master node.
		while( Shell::running() ) {
			pollPostmaster();
		}
	}
#endif
	terminateMPI( myNode );

	cout << "done" << endl;
}
