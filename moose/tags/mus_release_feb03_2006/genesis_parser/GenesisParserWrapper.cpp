/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003 Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <math.h>
#include <setjmp.h>
#include <FlexLexer.h>
#include "script.h"
#include "Shell.h"
#include "ShellWrapper.h"
#include "GenesisParser.h"
#include "GenesisParserWrapper.h"

Finfo* GenesisParserWrapper::fieldArray_[] =
{
	new ValueFinfo< string >( "shell",
		&GenesisParserWrapper::getShell,
		&GenesisParserWrapper::setShell,
		"string" ),
	new Dest1Finfo< string >( "readline",
		&GenesisParserWrapper::readlineFunc, 
		&GenesisParserWrapper::getReadlineConn, ""),

	new Dest0Finfo( "listcommands",
		&GenesisParserWrapper::listCommandsFunc, 
		&GenesisParserWrapper::getShellInputConn, ""),

	new Dest2Finfo< string, string> ( "alias",
		&GenesisParserWrapper::aliasFunc, 
		&GenesisParserWrapper::getShellInputConn, ""),

	new Dest0Finfo( "process",
		&GenesisParserWrapper::processFunc, 
		&GenesisParserWrapper::getProcessConn, ""),

	new Dest1Finfo< string >( "parse",
		&GenesisParserWrapper::parseFunc, 
		&GenesisParserWrapper::getParseConn, ""),

	new SingleSrc1Finfo< string >( "echo",
		&GenesisParserWrapper::getEchoSrc, ""),

	new SingleSrc2Finfo< int, const char** >( "commandOut",
		&GenesisParserWrapper::getCommandSrc, ""),
};

const Cinfo GenesisParserWrapper::cinfo_(
	"GenesisParser",
	"Upinder S. Bhalla, NCBS, 2004-2005",
	"Object to handle the old Genesis parser",
	"Element",
	GenesisParserWrapper::fieldArray_,
	sizeof(GenesisParserWrapper::fieldArray_)/sizeof(Finfo *),
	&GenesisParserWrapper::create
);

//////////////////////////////////////////////////////////////////
// GenesisParserWrapper friend functions
//////////////////////////////////////////////////////////////////

Element* lookupEchoConn( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET( GenesisParserWrapper, echoConn_ );
	return ( Element * )( ( unsigned long )c - OFFSET );
}

Element* lookupCommandConn( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET( GenesisParserWrapper, commandConn_ );
	return ( Element * )( ( unsigned long )c - OFFSET );
}

// This provides the Conn for a variety of input commands from the shell
Element* lookupShellInputConn( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET( GenesisParserWrapper, shellInputConn_ );
	return reinterpret_cast< GenesisParserWrapper* >(
		( unsigned long )c - OFFSET );
}

/*
Element* lookupProcessConn( const Conn* c )
{
	static const unsigned long OFFSET =
		(unsigned long) ( &GenesisParserWrapper::processConn_ );
	return ( Element * )( ( unsigned long )c - OFFSET );
}
*/

//////////////////////////////////////////////////////////////////
// GenesisParserWrapper utilities
//////////////////////////////////////////////////////////////////

char* copyString( const string& s )
{
	char* ret = ( char* ) calloc ( s.length() + 1, sizeof( char ) );
	strcpy( ret, s.c_str() );
	return ret;
}

void GenesisParserWrapper::plainCommand( int argc, const char** argv )
{
	commandSrc_.send( argc, argv );
}

char* GenesisParserWrapper::returnCommand( int argc, const char** argv )
{
	commandSrc_.send( argc, argv );
	// The Shell immediately sends back the return value as a string.
	return copyString( returnCommandValue_.c_str() );
}

//////////////////////////////////////////////////////////////////
// GenesisParserWrapper Builtin commands
//////////////////////////////////////////////////////////////////

map< string, string >& sliSrcLookup()
{
	static map< string, string > src;

	if ( src.size() > 0 )
		return src;
	
	src[ "REAC A B" ] = "sub";	// for reactions
	src[ "SUBSTRATE n" ] = "";
	src[ "SUBSTRATE n vol" ] = "reac"; // For concchans.
	src[ "PRODUCT n vol" ] = "reac"; // For concchans.
	src[ "NUMCHAN n" ] = "nOut"; // From molecules to concchans.
	src[ "REAC B A" ] = "prd";
	src[ "PRODUCT n" ] = "";

	src[ "REAC sA B" ] = "sub";	// for enzymes
	src[ "SUBSTRATE n" ] = "";
	src[ "REAC eA B" ] = "";	// Target is molecule. Ignore as it
								// only applies to explicit enz.
	src[ "ENZYME n" ] = "reac"; // target is an enzyme. Use it.
	src[ "PRODUCT n" ] = "";
	src[ "MM_PRD pA" ] = "prdOut";

	src[ "SUMTOTAL n nInit" ] = "nOut";	// for molecules
	src[ "SUMTOTAL output output" ] = "out";	// for tables
	src[ "SLAVE output" ] = "out";	// for tables
	src[ "INTRAMOL n" ] = "nOut"; 	// target is an enzyme.
	src[ "CONSERVE n nInit" ] = ""; 	// Deprecated
	src[ "CONSERVE nComplex nComplexInit" ] = ""; 	// Deprecated

	return src;
}

map< string, string >& sliDestLookup()
{
	static map< string, string > dest;

	if ( dest.size() > 0 )
		return dest;

	dest[ "SUBSTRATE n vol" ] = "influx"; // For channels.
	dest[ "PRODUCT n vol" ] = "efflux";
	dest[ "NUMCHAN n" ] = "nIn"; // From molecules to concchans.
	
	dest[ "REAC A B" ] = "reac";	// for reactions
	dest[ "SUBSTRATE n" ] = "";
	dest[ "REAC B A" ] = "reac";
	dest[ "PRODUCT n" ] = "";

	dest[ "REAC sA B" ] = "reac";	// for enzymes
	dest[ "SUBSTRATE n" ] = "";
	dest[ "REAC eA B" ] = "";		// Target is enzyme, but only used
									// for explicit enzymes. Ignore.
	dest[ "ENZYME n" ] = "enz"; 	// Used both for explicit and MM.
	dest[ "PRODUCT n" ] = "";
	dest[ "MM_PRD pA" ] = "prdIn";

	dest[ "SUMTOTAL n nInit" ] = "sumTotalIn";	// for molecules
	dest[ "SUMTOTAL output output" ] = "sumTotalIn";	// for molecules
	dest[ "SLAVE output" ] = "sumTotalIn";	// for molecules
	dest[ "INTRAMOL n" ] = "intramolIn"; 	// target is an enzyme.
	dest[ "CONSERVE n nInit" ] = ""; 	// Deprecated
	dest[ "CONSERVE nComplex nComplexInit" ] = ""; 	// Deprecated

	return dest;
}

map< string, string >& sliClassNameConvert()
{
	static map< string, string > classnames;

	if ( classnames.size() > 0 )
		return classnames;
	
	classnames[ "neutral" ] = "Neutral";
	classnames[ "pool" ] = "Molecule";
	classnames[ "kpool" ] = "Molecule";
	classnames[ "reac" ] = "Reaction";
	classnames[ "kreac" ] = "Reaction";
	classnames[ "enz" ] = "Enzyme";
	classnames[ "kenz" ] = "Enzyme";
	classnames[ "kchan" ] = "ConcChan";
	classnames[ "conc_chan" ] = "ConcChan";
	classnames[ "Ca_concen" ] = "CaConc";
	classnames[ "compartment" ] = "Compartment";
	classnames[ "hh_channel" ] = "HHChannel";
	classnames[ "tabchannel" ] = "HHChannel";
	classnames[ "vdep_channel" ] = "HHChannel";
	classnames[ "vdep_gate" ] = "HHGate";

	return classnames;
}

/*
map< string, string >& sliFieldNameConvert()
{
	static map< string, string > fieldnames;

	if ( fieldnames.size() > 0 )
		return fieldnames;

	return fieldnames;
}
*/


string sliMessage( const char* elmName, 
	const string& msgType, map< string, string >& converter )
{
	map< string, string >::iterator i;
	i = converter.find( msgType );
	if ( i != converter.end() ) {
		if ( i->second.length() == 0 ) // A redundant message 
			return "";
		else 
			return string( elmName ) + "/" + i->second; // good message.
	} else 
		cout << "Error:sliMessage: Unknown message " <<
			elmName << " " << msgType << "\n";
	return "";
}

void do_add( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->addFuncLocal( argv[1], argv[2] );
	} else if ( argc > 3 ) { 
	// Old-fashioned addmsg. Backward Compatibility conversions here.
	// usage: addmsg source-element dest-delement msg-type [msg-fields]
	// Most of these are handled using the info in the msg-type and
	// msg fields. Often there are redundant messages which are now
	// handled by shared messages. The redundant one is ignored.
		string msgType = argv[3];
	// A particularly ugly special case. PLOT messages are turned
	// around in MOOSE because we now use trigPlots where the
	// plot is the message source and the field the dest.
	// In principle we may need to do a separate lookup for the plotted
	// field, here we just plug in argv[4].
		if ( msgType == "PLOT" ) {
			string plotfield = argv[4];
			if (plotfield == "Co")
				plotfield = "conc";
			s->addFuncLocal( string( argv[2] ) + "/trigPlot", 
				string( argv[ 1 ] ) + "/" + plotfield );
			return;
		}
	

		for ( int i = 4; i < argc; i++ )
			msgType = msgType + " " + argv[ i ];
		string src = sliMessage( argv[1], msgType, sliSrcLookup() );
		string dest = sliMessage( argv[2], msgType, sliDestLookup() );
		if ( src.length() > 0 && dest.length() > 0 )
			s->addFuncLocal( src, dest );
	} else {
		cout << "usage:: " << argv[0] << " src dest\n";
		cout << "deprecated usage:: " << argv[0] << " source-element dest-element msg-type [msg-fields]";
	}
}

void do_drop( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->dropFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " src dest\n";
	}
}

void do_set( int argc, const char** argv, Shell* s )
{
	string path;
	int start = 2;
	if ( argc < 3 ) {
		cout << argv[0] << ": Too few command arguments\n";
		cout << "usage:: " << argv[0] << " [path] field value ...\n";
		return;
	}
	if ( argc % 2 == 1 ) { // 'path' is left out, use current object.
		path = ".";
		start = 1;
	} else  {
		path = argv[ 1 ];
		start = 2;
	}

	// Hack here to deal with the special case of filling tables
	// in tabchannels. Example command looks like:
	// 		setfield Ca Y_A->table[{i}] {y}
	// so here we need to do setfield Ca/Y/A->table[{i}] {y}
	for ( int i = start; i < argc; i += 2 ) {
		if ( strncmp (argv[ i ] + 1, "_A->", 4) == 0  ||
			strncmp (argv[ i ] + 1, "_B->", 4) == 0 ) {
			string fieldname = argv[ i ];
			fieldname[1] = '/';
			s->setFuncLocal( path + "/" + fieldname, argv[ i + 1 ] );
		} else {
			s->setFuncLocal( path + "/" + argv[ i ], argv[ i + 1 ] );
		}
	}
}

void do_call( int argc, const char** argv, Shell* s )
{
	if ( argc < 3 ) {
		cout << "usage:: " << argv[0] << " path field/Action [args...]\n";
		return;
	}
	// Ugly hack to avoid LOAD call for notes on kkit dumpfiles
	if ( strcmp ( argv[2], "LOAD" ) == 0 )
		return;
	
	// Ugly hack to handle the TABCREATE calls, which do not go through
	// the normal message destination route.
	if ( strcmp ( argv[2], "TABCREATE" ) == 0 ) {
		s->tabCreateFunc( argc, argv );
		return;
	}

	// Ugly hack to handle the TABFILL call, which need to be redirected
	// to the two interpols of the HHGates.
	// Deprecated.
	if ( strcmp ( argv[2], "TABFILL" ) == 0 ) {
		s->tabFillFunc( argc, argv );
		return;
	}
	string field;
	string value = "";
	field = string( argv[ 1 ] ) + "/" + argv[ 2 ];
	for ( int i = 3; i < argc; i++ ) {
		if ( i > 3 )
			value = value + ",";
		value = value + argv[ i ];
	}
	s->setFuncLocal( field, value );
}

int do_isa( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		return s->isaFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " type field\n";
	}
	return 0;
}

int do_exists( int argc, const char** argv, Shell* s )
{
	if ( argc == 2 ) {
		string temp = argv[1];
		temp = temp + "/name";
		return s->existsFuncLocal( temp );
	} else if ( argc == 3 ) {
		string temp = argv[1];
		temp = temp + "/" + argv[2];
		return s->existsFuncLocal( temp );
	} else {
		cout << "usage:: " << argv[0] << " element [field]\n";
	}
	return 0;
}

char* do_get( int argc, const char** argv, Shell* s )
{
	string field;
	string value;
	if ( argc == 3 ) {
		field = string( argv[ 1 ] ) + "/" + argv[ 2 ];
	} else if ( argc == 2 ) {
		field = argv[ 1 ];
	} else {
		cout << "usage:: " << argv[0] << " [element] field\n";
		return "";
	}
	return copyString( s->getFuncLocal( field ) );
}

char* do_getmsg( int argc, const char** argv, Shell* s )
{
	static string space = " ";
	if ( argc < 3 ) {
		cout << "usage:: " << argv[0] << " element -incoming -outgoing -slot msg-number slot-number -count -type msg-number -destination msg-number -source msg-number -find srcelem type\n";
		return "";
	}
	string field = argv[ 1];
	string options = argv[ 2 ];
	for ( int i = 3; i < argc; i++ )
		options += space + argv[ i ];

	return copyString( s->getmsgFuncLocal( field, options ) );
}

void do_create( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		map< string, string >::iterator i = 
			sliClassNameConvert().find( argv[1] );
		if ( i != sliClassNameConvert().end() )
			s->createFuncLocal( i->second, argv[2] );
		else
			s->createFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " class name\n";
	}
}

void do_delete( int argc, const char** argv, Shell* s )
{
	if ( argc == 2 ) {
		s->deleteFuncLocal( argv[1] );
	} else {
		cout << "usage:: " << argv[0] << " Element/path\n";
	}
}

void do_move( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->moveFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " src dest\n";
	}
}

void do_copy( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->copyFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " src dest\n";
	}
}

void do_copy_shallow( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->copyShallowFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " src dest\n";
	}
}

void do_copy_halo( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->copyHaloFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " src dest\n";
	}
}

void do_ce( int argc, const char** argv, Shell* s )
{
	if ( argc == 2 ) {
		s->ceFuncLocal( argv[1] );
	} else {
		cout << "usage:: " << argv[0] << " Element\n";
	}
}

void do_pushe( int argc, const char** argv, Shell* s )
{
	if ( argc == 2 ) {
		s->pusheFuncLocal( argv[1] );
	} else {
		cout << "usage:: " << argv[0] << " Element\n";
	}
}

void do_pope( int argc, const char** argv, Shell* s )
{
	if ( argc == 1 ) {
		s->popeFuncLocal( );
	} else {
		cout << "usage:: " << argv[0] << "\n";
	}
}

void do_alias( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 )
		s->aliasFuncLocal( argv[ 1 ], argv[ 2 ] );
	else if ( argc == 2 )
		s->aliasFuncLocal( argv[ 1 ], "" );
	else if ( argc == 1 )
		s->aliasFuncLocal( "", "" );
}

void do_quit( int argc, const char** argv, Shell* s )
{
	s->quitFuncLocal( );
}

void do_stop( int argc, const char** argv, Shell* s )
{
	if ( argc == 1 ) {
		s->stopFuncLocal( );
	} else {
		cout << "usage:: " << argv[0] << "\n";
	}
}

void do_reset( int argc, const char** argv, Shell* s )
{
	if ( argc == 1 ) {
		s->resetFuncLocal( );
	} else {
		cout << "usage:: " << argv[0] << "\n";
	}
}

void do_step( int argc, const char** argv, Shell* s )
{
	if ( argc == 2 ) {
		s->stepFuncLocal( argv[1], "" );
	} else if ( argc == 3 ) {
		s->stepFuncLocal( argv[1], argv[2] );
	} else {
		cout << "usage:: " << argv[0] << " time/nsteps [-t -s(default ]\n";
	}
}

void do_setclock( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->setClockFuncLocal( argv[1], argv[2], "0" );
	} else if ( argc == 4 ) {
		s->setClockFuncLocal( argv[1], argv[2], argv[3] );
	} else {
		cout << "usage:: " << argv[0] << " clockNum dt [stage]\n";
	}
}

void do_showclocks( int argc, const char** argv, Shell* s )
{
	if ( argc == 1 ) {
		s->showClocksFuncLocal( );
	} else {
		cout << "usage:: " << argv[0] << "\n";
	}
}

void do_useclock( int argc, const char** argv, Shell* s )
{
	if ( argc == 3 ) {
		s->useClockFuncLocal( string( argv[1] ) + "/process", argv[2] );
	} else if ( argc == 4 ) {
		s->useClockFuncLocal( string( argv[1] ) + "/" + argv[2],
			argv[3] );
	} else {
		cout << "usage:: " << argv[0] << " path [funcname] clockNum\n";
	}
}

void do_show( int argc, const char** argv, Shell* s )
{
	string temp;
	if ( argc == 2 ) {
		temp = string( "./" ) + argv[1];
		s->showFuncLocal( temp );
	} else if ( argc == 3 ) {
		temp = string( argv[ 1 ] ) + "/" + argv[ 2 ];
		s->showFuncLocal( temp );
	} else {
		cout << "usage:: " << argv[0] << " [element] field\n";
	}
}

void do_le( int argc, const char** argv, Shell* s )
{
	if ( argc == 1 )
		s->leFuncLocal( "." );
	else if ( argc >= 2 )
		s->leFuncLocal( argv[1] );
}

void do_pwe( int argc, const char** argv, Shell* s )
{
	s->pweFuncLocal( );
}

void do_listcommands( int argc, const char** argv, Shell* s )
{
	s->listCommandsFuncLocal( );
}

void do_listobjects( int argc, const char** argv, Shell* s )
{
	s->listClassesFuncLocal( );
}

void do_echo( int argc, const char** argv, Shell* s )
{
	vector< string > vec;
	int options = 0;
	if ( argc > 1 && strncmp( argv[ argc - 1 ], "-n", 2 ) == 0 ) {
		options = 1; // no newline
		argc--;
	}

	for (int i = 1; i < argc; i++ )
		vec.push_back( argv[i] );

	s->echoFuncLocal( vec, options );
}

// Old GENESIS Usage: addfield [element] field-name -indirect element field -description text
// Here we'll have a subset of it:
// addfield [element] field-name -type field_type
void do_addfield( int argc, const char** argv, Shell* s )
{
	if ( argc == 2 ) {
		const char *nargv[] = { argv[0], ".", argv[1] };
		s->commandFuncLocal( 3, nargv );
	} else if ( argc == 3 ) {
		s->commandFuncLocal( argc, argv );
	} else if ( argc == 4 && strncmp( argv[2], "-f", 2 ) == 0 ) {
		const char *nargv[] = { argv[0], ".", argv[1], argv[3] };
		s->commandFuncLocal( 4, nargv );
	} else if ( argc == 5 && strncmp( argv[3], "-f", 2 ) == 0 ) {
		const char *nargv[] = { argv[0], argv[1], argv[3], argv[4] };
		s->commandFuncLocal( 4, nargv );
	} else {
		s->error( "usage: addfield [element] field-name -type field_type" );
	}
}

void doShellCommand ( int argc, const char** argv, Shell* s )
{
	s->commandFuncLocal( argc, argv );
}

void do_complete_loading( int argc, const char** argv, Shell* s )
{
}

float do_exp( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "exp: Too few command arguments\nusage: exp number" );
		return 0.0;
	} else {	
		return exp( atof( argv[ 1 ] ) );
	}
}

float do_log( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "log: Too few command arguments\nusage: log number" );
		return 0.0;
	} else {	
		return log( atof( argv[ 1 ] ) );
	}
}

float do_log10( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "log10: Too few command arguments\nusage: log10 number" );
		return 0.0;
	} else {	
		return log10( atof( argv[ 1 ] ) );
	}
}

float do_sin( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "sin: Too few command arguments\nusage: sin number" );
		return 0.0;
	} else {	
		return sin( atof( argv[ 1 ] ) );
	}
}

float do_cos( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "cos: Too few command arguments\nusage: cos number" );
		return 0.0;
	} else {	
		return cos( atof( argv[ 1 ] ) );
	}
}

float do_tan( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "tan: Too few command arguments\nusage: tan number" );
		return 0.0;
	} else {	
		return tan( atof( argv[ 1 ] ) );
	}
}

float do_sqrt( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "sqrt: Too few command arguments\nusage: sqrt number" );
		return 0.0;
	} else {	
		return sqrt( atof( argv[ 1 ] ) );
	}
}

float do_pow( int argc, const char** argv, Shell* s )
{
	if ( argc != 3 ) {
		s->error( "pow: Too few command arguments\nusage: exp base exponent" );
		return 0.0;
	} else {	
		return pow( atof( argv[ 1 ] ), atof( argv[ 2 ] ) );
	}
}

float do_abs( int argc, const char** argv, Shell* s )
{
	if ( argc != 2 ) {
		s->error( "abs: Too few command arguments\nusage: abs number" );
		return 0.0;
	} else {	
		return fabs( atof( argv[ 1 ] ) );
	}
}

//////////////////////////////////////////////////////////////////
// GenesisParserWrapper load command
//////////////////////////////////////////////////////////////////

void GenesisParserWrapper::loadBuiltinCommands()
{
	AddFunc( "addmsg", do_add, "void" );
	AddFunc( "deletemsg", do_drop, "void" );
	AddFunc( "setfield", do_set, "void" );
	AddFunc( "getfield", reinterpret_cast< slifunc >( do_get ), "char*" );
	AddFunc( "getmsg", reinterpret_cast< slifunc >( do_getmsg ), "char*" );
	AddFunc( "call", do_call, "void" );
	AddFunc( "isa", reinterpret_cast< slifunc >( do_isa ), "int" );
	AddFunc( "exists", reinterpret_cast< slifunc >( do_exists ), "int");
	AddFunc( "showfield", do_show, "void" );
	AddFunc( "create", do_create, "void" );
	AddFunc( "delete", do_delete, "void" );
	AddFunc( "move", do_move, "void" );
	AddFunc( "copy", do_copy, "void" );
	AddFunc( "copy_shallow", do_copy_shallow, "void" );
	AddFunc( "copy_halo", do_copy_halo, "void" );
	AddFunc( "ce", do_ce, "void" );
	AddFunc( "pushe", do_pushe, "void" );
	AddFunc( "pope", do_pope, "void" );
	AddFunc( "alias", do_alias, "void" );
	AddFunc( "quit", do_quit, "void" );
	AddFunc( "stop", do_stop, "void" );
	AddFunc( "reset", do_reset, "void" );
	AddFunc( "step", do_step, "void" );
	AddFunc( "setclock", do_setclock, "void" );
	AddFunc( "useclock", do_useclock, "void" );
	AddFunc( "showclocks", do_showclocks, "void" );
	AddFunc( "le", do_le, "void" );
	AddFunc( "pwe", do_pwe, "void" );
	AddFunc( "listcommands", do_listcommands, "void" );
	AddFunc( "listobjects", do_listobjects, "void" );
	AddFunc( "echo", do_echo, "void" );
	AddFunc( "simdump", doShellCommand, "void" );
	AddFunc( "simundump", doShellCommand, "void" );
	AddFunc( "simobjdump", doShellCommand, "void" );
	AddFunc( "loadtab", doShellCommand, "void" );
	AddFunc( "readcell", doShellCommand, "void" );
	AddFunc( "setupalpha", doShellCommand, "void" );
	AddFunc( "addfield", do_addfield, "void" );
	AddFunc( "complete_loading", do_complete_loading, "void" );
	AddFunc( "exp", reinterpret_cast< slifunc>( do_exp ), "float" );
	AddFunc( "log", reinterpret_cast< slifunc>( do_log ), "float" );
	AddFunc( "log0", reinterpret_cast< slifunc>( do_log10 ), "float" );
	AddFunc( "sin", reinterpret_cast< slifunc>( do_sin ), "float" );
	AddFunc( "cos", reinterpret_cast< slifunc>( do_cos ), "float" );
	AddFunc( "tan", reinterpret_cast< slifunc>( do_tan ), "float" );
	AddFunc( "sqrt", reinterpret_cast< slifunc>( do_sqrt ), "float" );
	AddFunc( "pow", reinterpret_cast< slifunc>( do_pow ), "float" );
	AddFunc( "abs", reinterpret_cast< slifunc>( do_abs ), "float" );
}

//////////////////////////////////////////////////////////////////
// GenesisParserWrapper Field commands
//////////////////////////////////////////////////////////////////

void GenesisParserWrapper::setShell( Conn* c, string s )
{
	Element * temp = Element::root()->relativeFind( s );
	ShellWrapper* shell = dynamic_cast< ShellWrapper* >( temp );
	if ( !shell ) {
		cout << "Error:GenesisParserWrapper::setShell: Element " << 
			s << " not found\n";
		return;
	}
	static_cast<GenesisParserWrapper *>( c->parent() )->
		innerSetShell( shell );
}

string GenesisParserWrapper::getShell( const Element* e )
{
	Shell* temp = 
		static_cast< const GenesisParserWrapper *>( e )->
			innerGetShell( );
	ShellWrapper* shell = dynamic_cast< ShellWrapper* >( temp );
			if ( !shell ) {
		cout << "Error:GenesisParserWrapper::getShell: shell not defined\n";
		return "";
	}
	return shell->name();
}
