/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#include <iomanip>
#include <fstream>
/*
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cassert>
#include <stdlib.h>

using namespace std;
typedef int Id; // dummy
*/
#include "header.h"
#include "ReadKkit.h"
#include "Mol.h"
#include "FuncMol.h"

const double ReadKkit::EPSILON = 1.0e-15;

unsigned int chopLine( const string& line, vector< string >& ret )
{
	ret.resize( 0 );
	stringstream ss( line );
	string arg;
	while ( ss >> arg ) {
		ret.push_back( arg );
	}
	return ret.size();
}

/*
int main( int argc, const char* argv[] )
{
	string fname = "dend_v26.g";
	if ( argc == 2 )
		fname = argv[1];
	ReadKkit rk;
	rk.read( fname, "cell", 0 );
}
*/

////////////////////////////////////////////////////////////////////////

ReadKkit::ReadKkit()
	:
	fastdt_( 0.001 ),
	simdt_( 0.01 ),
	controldt_( 0.1 ),
	plotdt_( 1 ),
	maxtime_( 1.0 ),
	transientTime_( 1.0 ),
	useVariableDt_( 0 ),
	defaultVol_( 1 ),
	version_( 11 ),
	initdumpVersion_( 3 ),
	numCompartments_( 0 ),
	numMols_( 0 ),
	numReacs_( 0 ),
	numEnz_( 0 ),
	numMMenz_( 0 ),
	numPlot_( 0 ),
	numOthers_( 0 ),
	lineNum_( 0 ),
	shell_( reinterpret_cast< Shell* >( Id().eref().data() ) )
{
	;
}

//////////////////////////////////////////////////////////////////
// Fields for readKkit
//////////////////////////////////////////////////////////////////

double ReadKkit::getMaxTime() const
{
	return maxtime_;
}

double ReadKkit::getPlotDt() const
{
	return plotdt_;
}

double ReadKkit::getDefaultVol() const
{
	return defaultVol_;
}

string ReadKkit::getBasePath() const
{
	return basePath_;
}

//////////////////////////////////////////////////////////////////
// The read functions.
//////////////////////////////////////////////////////////////////
/**
 * The readcell function implements the old GENESIS cellreader
 * functionality. Although it is really a parser operation, I
 * put it here in Kinetics because the cell format is independent
 * of parser and is likely to remain a legacy for a while.
 */
Id ReadKkit::read(
	const string& filename, 
	const string& modelname,
	Id pa )
{
	ifstream fin( filename.c_str() );
	if (!fin){
		cerr << "ReadKkit::read: could not open file " << filename << endl;
		return Id();
    }

	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< unsigned int > dims( 1,1 );
	Id base = s->doCreate( "Stoich", pa, modelname, dims );
	assert( base != Id() );

	baseId_ = base;
	basePath_ = base.path();

	innerRead( fin );

	assignCompartments();
	s->setCwe( base );
	Field< string >::set( base.eref(), "path", "##" );
	s->doReinit();
	return base;
}

void ReadKkit::run()
{
	shell_->doSetClock( 0, simdt_ );
	shell_->doSetClock( 1, simdt_ );
	shell_->doSetClock( 2, plotdt_ );
	string molpath = basePath_ + "/kinetics/##[ISA=Mol]";
	string reacpath = basePath_ + "/kinetics/##[ISA!=Mol]";
	string plotpath = basePath_ + "/graphs/##[TYPE=Table]," + 
		basePath_ + "/moregraphs/##[TYPE=Table]";
	shell_->doUseClock( reacpath, "process", 0 );
	shell_->doUseClock( molpath, "process", 1 );
	shell_->doUseClock( plotpath, "process", 2 );
	shell_->doReinit();
	if ( useVariableDt_ ) {
		shell_->doSetClock( 0, fastdt_ );
		shell_->doSetClock( 1, fastdt_ );
		shell_->doStart( transientTime_ );
		shell_->doSetClock( 0, simdt_ );
		shell_->doSetClock( 1, simdt_ );
		shell_->doStart( maxtime_ - transientTime_ );
	} else {
		shell_->doStart( maxtime_ );
	}
}

void ReadKkit::dumpPlots( const string& filename )
{
	// ofstream fout ( filename.c_str() );
	vector< Id > plots;
	string plotpath = basePath_ + "/graphs/##[TYPE=Table]," + 
		basePath_ + "/moregraphs/##[TYPE=Table]";
	Shell::wildcard( plotpath, plots );
	for ( vector< Id >::iterator i = plots.begin(); i != plots.end(); ++i )
		SetGet2< string, string >::set( i->eref(), "xplot",
			filename, (*i)()->getName() );
}

void ReadKkit::innerRead( ifstream& fin )
{
	string line;
	string temp;
	lineNum_ = 0;
	string::size_type pos;
	bool clearLine = 1;
	ParseMode parseMode = INIT;

	while ( getline( fin, temp ) ) {
		lineNum_++;
		if ( clearLine )
			line = "";

		if ( temp.length() == 0 )
				continue;
		pos = temp.find_last_not_of( "\t " );
		if ( pos == string::npos ) { 
			// Nothing new in line, go with what was left earlier, 
			// and clear out line for the next cycle.
			temp = "";
			clearLine = 1;
		} else {
			if ( temp[pos] == '\\' ) {
				temp[pos] = ' ';
				line.append( temp );
				clearLine = 0;
				continue;
			} else {
				line.append( temp );
				clearLine = 1;
			}
		}
			
		pos = line.find_first_not_of( "\t " );
		if ( pos == string::npos )
				continue;
		else
			line = line.substr( pos );
		if ( line.substr( 0, 2 ) == "//" )
				continue;
		if ( (pos = line.find("//")) != string::npos ) 
			line = line.substr( 0, pos );
		if ( line.substr( 0, 2 ) == "/*" ) {
				parseMode = COMMENT;
				line = line.substr( 2 );
		}

		if ( parseMode == COMMENT ) {
			pos = line.find( "*/" );
			if ( pos != string::npos ) {
				parseMode = DATA;
				if ( line.length() > pos + 2 )
					line = line.substr( pos + 2 );
			}
		}

		if ( parseMode == DATA )
				readData( line );
		else if ( parseMode == INIT ) {
				parseMode = readInit( line );
		}
	}
	
	/*
	cout << " innerRead: " <<
			lineNum_ << " lines read, " << 
			numCompartments_ << " compartments, " << 
			numMols_ << " molecules, " << 
			numReacs_ << " reacs, " << 
			numEnz_ << " enzs, " << 
			numMMenz_ << " MM enzs, " << 
			numOthers_ << " others," <<
			numPlot_ << " plots," <<
			" PlotDt = " << plotdt_ <<
			endl;
			*/
}

void ReadKkit::makeStandardElements()
{
	vector< unsigned int > dims( 1, 1 );
	Id kinetics = Neutral::child( baseId_.eref(), "kinetics" );
	if ( kinetics == Id() ) {
		kinetics = 
		shell_->doCreate( "Neutral", baseId_, "kinetics", dims );
	}
	assert( kinetics != Id() );
	assert( kinetics.eref().element()->getName() == "kinetics" );

	Id graphs = Neutral::child( baseId_.eref(), "graphs" );
	if ( graphs == Id() ) {
		graphs = 
		shell_->doCreate( "Neutral", baseId_, "graphs", dims );
	}
	assert( graphs != Id() );

	Id moregraphs = Neutral::child( baseId_.eref(), "moregraphs" );
	if ( moregraphs == Id() ) {
		moregraphs = 
		shell_->doCreate( "Neutral", baseId_, "moregraphs", dims );
	}
	assert( moregraphs != Id() );

	Id compartments = Neutral::child( baseId_.eref(), "compartments" );
	if ( compartments == Id() ) {
		compartments = 
		shell_->doCreate( "Neutral", baseId_, "compartments", dims );
	}
	assert( compartments != Id() );

	Id geometry = Neutral::child( baseId_.eref(), "geometry" );
	if ( geometry == Id() ) {
		geometry = 
		shell_->doCreate( "Neutral", baseId_, "geometry", dims );
	}
	assert( geometry != Id() );

	Id groups = Neutral::child( baseId_.eref(), "groups" );
	if ( groups == Id() ) {
		groups = 
		shell_->doCreate( "Neutral", baseId_, "groups", dims );
	}
	assert( groups != Id() );
}

ReadKkit::ParseMode ReadKkit::readInit( const string& line )
{
	vector< string > argv;
	chopLine( line, argv ); 
	if ( argv.size() < 3 )
		return INIT;

	if ( argv[0] == "FASTDT" ) {
		fastdt_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "SIMDT" ) {
		simdt_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "CONTROLDT" ) {
		controldt_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "PLOTDT" ) {
		plotdt_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "MAXTIME" ) {
		maxtime_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "TRANSIENT_TIME" ) {
		transientTime_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "VARIABLE_DT_FLAG" ) {
		useVariableDt_ = atoi( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "DEFAULT_VOL" ) {
		defaultVol_ = atof( argv[2].c_str() );
		return INIT;
	}
	if ( argv[0] == "VERSION" ) {
		version_ = atoi( argv[2].c_str() );
		return INIT;
	}

	if ( argv[0] == "initdump" ) {
		initdumpVersion_ = atoi( argv[2].c_str() );
		makeStandardElements();
		return DATA;
	}

	return INIT;
}


void ReadKkit::readData( const string& line )
{
	vector< string > argv;
	chopLine( line, argv ); 
	
	if ( argv[0] == "simundump" )
		undump( argv );
	else if ( argv[0] == "addmsg" )
		addmsg( argv );
	else if ( argv[0] == "call" )
		call( argv );
	else if ( argv[0] == "simobjdump" )
		objdump( argv );
	else if ( argv[0] == "xtextload" )
		textload( argv );
	else if ( argv[0] == "loadtab" )
		loadtab( argv );
}

string ReadKkit::pathTail( const string& path, string& head ) const
{
	string::size_type pos = path.find_last_of( "/" );
	assert( pos != string::npos );

	head = basePath_ + path.substr( 0, pos ); 
	return path.substr( pos + 1 );
}

/*
Id ReadKkit::findParent( const string& path ) const
{
	return 1;
}
*/

void assignArgs( map< string, int >& argConv, const vector< string >& args )
{
	for ( unsigned int i = 2; i != args.size(); ++i )
		argConv[ args[i] ] = i + 2;
}

void ReadKkit::objdump( const vector< string >& args)
{
	if ( args[1] == "kpool" )
		assignArgs( molMap_, args );
	else if ( args[1] == "kreac" )
		assignArgs( reacMap_, args );
	else if ( args[1] == "kenz" )
		assignArgs( enzMap_, args );
	else if ( args[1] == "group" )
		assignArgs( groupMap_, args );
	else if ( args[1] == "table" )
		assignArgs( tableMap_, args );
}

void ReadKkit::call( const vector< string >& args)
{
}

void ReadKkit::textload( const vector< string >& args)
{
}

void ReadKkit::loadtab( const vector< string >& args)
{
}

void ReadKkit::undump( const vector< string >& args)
{
	if ( args[1] == "kpool" )
		buildMol( args );
	else if ( args[1] == "kreac" )
		buildReac( args );
	else if ( args[1] == "kenz" )
		buildEnz( args );
	else if ( args[1] == "text" )
		buildText( args );
	else if ( args[1] == "xplot" )
		buildPlot( args );
	else if ( args[1] == "xgraph" )
		buildGraph( args );
	else if ( args[1] == "group" )
		buildGroup( args );
	else if ( args[1] == "geometry" )
		buildGeometry( args );
	else if ( args[1] == "xcoredraw" )
		;
	else if ( args[1] == "xtree" )
		;
	else if ( args[1] == "xtext" )
		;
	else
		cout << "ReadKkit::undump: Do not know how to build '" << args[1] <<
		"'\n";
}

Id ReadKkit::buildCompartment( const vector< string >& args )
{
	Id compt;
	numCompartments_++;
	return compt;
}

Id ReadKkit::buildReac( const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );

	string head;
	string tail = pathTail( args[2], head );
	Id pa = shell_->doFind( head );
	assert( pa != Id() );

	double kf = atof( args[ reacMap_[ "kf" ] ].c_str() );
	double kb = atof( args[ reacMap_[ "kb" ] ].c_str() );

	Id reac = shell_->doCreate( "Reac", pa, tail, dim );
	reacIds_[ args[2].substr( 10 ) ] = reac; 

	Field< double >::set( reac.eref(), "kf", kf );
	Field< double >::set( reac.eref(), "kb", kb );

	Id info = buildInfo( reac, reacMap_, args );

	numReacs_++;
	return reac;
}

void ReadKkit::separateVols( Id mol, double vol )
{
	static const double TINY = 1e-3;
	/*
	cout << mol << " vol = " << 
		( reinterpret_cast< const Mol* >( mol.eref().data() ) )->getSize() <<
		", v2 = " << vol << endl;
		*/

	for ( unsigned int i = 0 ; i < vols_.size(); ++i ) {
		if ( fabs( vols_[i] - vol ) / ( vols_[i] + vol ) < TINY ) {
			volCategories_[i].push_back( mol );
			return;
		}
	}
	vols_.push_back( vol );
	vector< Id > temp( 1, mol );
	volCategories_.push_back( temp );
}

// We assume that the biggest compartment contains all the rest.
// This is not true in synapses, where they are adjacent.
void ReadKkit::assignCompartments()
{
	double max = 0.0;
	unsigned int maxi;
	Id kinetics = Neutral::child( baseId_.eref(), "kinetics" );
	assert( kinetics != Id() );
	for ( unsigned int i = 0 ; i < vols_.size(); ++i ) {
		if ( max < vols_[i] ) {
			max = vols_[i];
			maxi = i;
		}
	}
	
	// Field< double >::set( kinetics.eref(), "size", max );
	vector< unsigned int > dims( 1, 1 );

	for ( unsigned int i = 0 ; i < volCategories_.size(); ++i ) {
		string name;
		if ( i == maxi ) {
			name = "main_compartment";
		} else {
			stringstream ss;
			ss << "compartment_" << i;
			name = ss.str();
		}
		Id compt = shell_->doCreate( 
			"ChemCompt", baseId_, name, dims );
		Field< double >::set( compt.eref(), "size", vols_[i] );
		compartments_.push_back( compt );
		for ( vector< Id >::iterator j = volCategories_[i].begin();
			j != volCategories_[i].end(); ++j ) {
			// get the group Ids that have a different vol in them
			MsgId ret = shell_->doAddMsg( "single", 
				FullId( compt, 0 ), "compartment",
				FullId( *j, 0 ), "setSize" ); 
			assert( ret != Msg::badMsg );
		}
	}
}

Id ReadKkit::buildEnz( const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );
	string head;
	string tail = pathTail( args[2], head );
	Id pa = shell_->doFind( head );
	assert ( pa != Id() );

	double k1 = atof( args[ enzMap_[ "k1" ] ].c_str() );
	double k2 = atof( args[ enzMap_[ "k2" ] ].c_str() );
	double k3 = atof( args[ enzMap_[ "k3" ] ].c_str() );
	double nComplexInit = atof( args[ enzMap_[ "nComplexInit" ] ].c_str());
	// double vol = atof( args[ enzMap_[ "vol" ] ].c_str());
	bool isMM = atoi( args[ enzMap_[ "usecomplex" ] ].c_str());

	if ( isMM ) {
		Id enz = shell_->doCreate( "MMenz", pa, tail, dim );
		assert( enz != Id () );
		string mmEnzPath = args[2].substr( 10 );
		mmEnzIds_[ mmEnzPath ] = enz; 

		assert( k1 > EPSILON );
		double Km = ( k2 + k3 ) / k1;
		Field< double >::set( enz.eref(), "Km", Km );
		Field< double >::set( enz.eref(), "kcat", k3 );
		Id info = buildInfo( enz, enzMap_, args );
		numMMenz_++;
		return enz;
	} else {
		Id enz = shell_->doCreate( "Enz", pa, tail, dim );
		double parentVol = Field< double >::get( pa.eref(), "size" );
		assert( enz != Id () );
		string enzPath = args[2].substr( 10 );
		enzIds_[ enzPath ] = enz; 

		Field< double >::set( enz.eref(), "k1", k1 );
		Field< double >::set( enz.eref(), "k2", k2 );
		Field< double >::set( enz.eref(), "k3", k3 );

		string cplxName = tail + "_cplx";
		string cplxPath = enzPath + "/" + cplxName;
		Id cplx = shell_->doCreate( "Mol", enz, cplxName, dim );
		assert( cplx != Id () );
		molIds_[ cplxPath ] = enz; 
		Field< double >::set( cplx.eref(), "nInit", nComplexInit );
		SetGet1< double >::set( cplx.eref(), "setSize", parentVol );

		separateVols( cplx, parentVol );

		bool ret = shell_->doAddMsg( "single", 
			FullId( enz, 0 ), "cplx",
			FullId( cplx, 0 ), "reac" ); 
		assert( ret != Msg::badMsg );

		// cplx()->showFields();
		// enz()->showFields();
		// pa()->showFields();
		Id info = buildInfo( enz, enzMap_, args );
		numEnz_++;
		return enz;
	}
}

Id ReadKkit::buildText( const vector< string >& args )
{
	Id text;
	numOthers_++;
	return text;
}

Id ReadKkit::buildInfo( Id parent, 
	map< string, int >& m, const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );
	Id info = shell_->doCreate( "Neutral", parent, "info", dim );
	assert( info != Id() );

	/*
	Id info = shell_->doCreate( "KkitInfo", parent, "info", dim );
	double x = atof( args[ m[ "x" ] ].c_str() );
	double y = atof( args[ m[ "y" ] ].c_str() );
	int color = atoi( args[ m[ "color" ] ].c_str() );

	Field< double >::set( info.eref(), "x", x );
	Field< double >::set( info.eref(), "y", y );
	Field< int >::set( info.eref(), "color", color );
	*/

	return info;
}

Id ReadKkit::buildGroup( const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );

	string head;
	string tail = pathTail( args[2], head );

	Id pa = shell_->doFind( head );
	assert( pa != Id() );
	Id group = shell_->doCreate( "Neutral", pa, tail, dim );
	assert( group != Id() );
	Id info = buildInfo( group, groupMap_, args );

	numOthers_++;
	return group;
}

Id ReadKkit::buildMol( const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );
	const double NA = 6.0e23; // Kkit uses 6e23 for NA.

	string head;
	string tail = pathTail( args[2], head );
	Id pa = shell_->doFind( head );
	assert( pa != Id() );

	double nInit = atof( args[ molMap_[ "nInit" ] ].c_str() );
	double vsf = atof( args[ molMap_[ "vol" ] ].c_str() ); 
	/**
	 * vsf is vol scale factor, which is what GENESIS stores in 'vol' field
	 * n = vsf * conc( uM )
	 * Also, n = ( conc (uM) / 1e6 ) * NA * vol
	 * so, vol = 1e6 * vsf / NA
	 */
	double vol = 1.0e3 * vsf / NA; // Converts volscale to actual vol in m^3
	int slaveEnable = atoi( args[ molMap_[ "slave_enable" ] ].c_str() );
	double diffConst = atof( args[ molMap_[ "DiffConst" ] ].c_str() );

	Id mol;
	if ( slaveEnable == 0 ) {
		mol = shell_->doCreate( "Mol", pa, tail, dim );
	} else if ( slaveEnable & 4 ) {
		mol = shell_->doCreate( "BufMol", pa, tail, dim );
	} else {
		mol = shell_->doCreate( "Mol", pa, tail, dim );
		/*
		cout << "ReadKkit::buildMol: Unknown slave_enable flag '" << 
			slaveEnable << "' on " << args[2] << "\n";
			*/
	}
	assert( mol != Id() );
	// skip the 10 chars of "/kinetics/"
	molIds_[ args[2].substr( 10 ) ] = mol; 

	Field< double >::set( mol.eref(), "nInit", nInit );
	Field< double >::set( mol.eref(), "diffConst", diffConst );
	SetGet1< double >::set( mol.eref(), "setSize", vol );

	separateVols( mol, vol );

	Id info = buildInfo( mol, molMap_, args );

	/*
	cout << setw( 20 ) << head << setw( 15 ) << tail << "	" << 
		setw( 12 ) << nInit << "	" << 
		vol << "	" << diffConst << "	" <<
		slaveEnable << endl;
		*/
	numMols_++;
	return mol;
}

void ReadKkit::buildSumTotal( const string& src, const string& dest )
{
	map< string, Id >::iterator i = molIds_.find( dest );
	assert( i != molIds_.end() );
	Id destId = i->second;
	
	// Don't bother on buffered mol.
	if ( destId()->cinfo()->isA( "BufMol" ) ) 
		return;

	Id sumId;
	// Check if the mol has not yet been converted to handle SumTots.
	if ( destId()->cinfo()->name() == "Mol" ) {
		vector< unsigned int > dim( 1, 1 );
		sumId = shell_->doCreate( "SumFunc", destId, "sumFunc", dim );
		const DataHandler* orig = destId()->dataHandler();
		DataHandler* dup = orig->copy();
	
		// Turn dest into a FuncMol.
		destId()->zombieSwap( FuncMol::initCinfo(), dup );
	} else {
		sumId = Neutral::child( destId.eref(), "sumFunc" );
	}

	if ( sumId == Id() ) {
		cout << "Error: ReadKkit::buildSumTotal: could not make SumFunc on '"
		<< dest << "'\n";
		return;
	}
	
	// Connect up messages
	i = molIds_.find( src );
	assert( i != molIds_.end() );
	Id srcId = i->second;

	bool ret = shell_->doAddMsg( "single", 
		FullId( srcId, 0 ), "nOut",
		FullId( sumId, 0 ), "input" ); 

	ret = shell_->doAddMsg( "single", 
		FullId( sumId, 0 ), "output",
		FullId( destId, 0 ), "input" ); 

	assert( ret );
}
	

Id ReadKkit::buildGeometry( const vector< string >& args )
{
	Id geometry;
	numOthers_++;
	return geometry;
}

Id ReadKkit::buildGraph( const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );

	string head;
	string tail = pathTail( args[2], head );

	Id pa = shell_->doFind( head );
	assert( pa != Id() );
	Id graph = shell_->doCreate( "Neutral", pa, tail, dim );
	assert( graph != Id() );
	numOthers_++;
	return graph;
}

Id ReadKkit::buildPlot( const vector< string >& args )
{
	static vector< unsigned int > dim( 1, 1 );

	string head;
	string tail = pathTail( args[2], head ); // Name of plot
	string temp;
	string graph = pathTail( head, temp ); // Name of graph

	Id pa = shell_->doFind( head );
	assert( pa != Id() );

	Id plot = shell_->doCreate( "Table", pa, tail, dim );
	assert( plot != Id() );

	temp = graph + "/" + tail;
	plotIds_[ temp ] = plot; 

	numPlot_++;
	return plot;
}

Id ReadKkit::buildTab( const vector< string >& args )
{
	Id tab;
	numOthers_++;
	return tab;
}

unsigned int ReadKkit::loadTab( const vector< string >& args )
{
	return 0;
}

void ReadKkit::innerAddMsg( 
	const string& src, const map< string, Id >& m1, const string& srcMsg,
	const string& dest, const map< string, Id >& m2, const string& destMsg )
{
	map< string, Id >::const_iterator i = m1.find( src );
	assert( i != m1.end() );
	Id srcId = i->second;

	i = m2.find( dest );
	assert( i != m2.end() );
	Id destId = i->second;

	// dest mol is substrate of src reac
	bool ret = shell_->doAddMsg( "single", 
		FullId( srcId, 0 ), srcMsg,
		FullId( destId, 0 ), destMsg ); 
	assert( ret != Msg::badMsg );
}


void ReadKkit::addmsg( const vector< string >& args)
{
	string src = args[1].substr( 10 );
	string dest = args[2].substr( 10 );
	
	if ( args[3] == "REAC" ) {
		if ( args[4] == "A" && args[5] == "B" ) {
			innerAddMsg( src, reacIds_, "sub", dest, molIds_, "reac" );
		} 
		else if ( args[4] == "B" && args[5] == "A" ) {
			// dest mol is product of src reac
			innerAddMsg( src, reacIds_, "prd", dest, molIds_, "reac" );
		}
		else if ( args[4] == "sA" && args[5] == "B" ) {
			// Msg from enzyme to substrate.
			if ( mmEnzIds_.find( src ) == mmEnzIds_.end() )
				innerAddMsg( src, enzIds_, "sub", dest, molIds_, "reac" );
			else
				innerAddMsg( src, mmEnzIds_, "sub", dest, molIds_, "reac" );
		}
	}
	else if ( args[3] == "ENZYME" ) { // Msg from enz mol to enz site
		if ( mmEnzIds_.find( dest ) == mmEnzIds_.end() )
			innerAddMsg( src, molIds_, "reac", dest, enzIds_, "enz" );
		else
			innerAddMsg( src, molIds_, "nOut", dest, mmEnzIds_, "enz" );
	}
	else if ( args[3] == "MM_PRD" ) { // Msg from enz to Prd mol
		if ( mmEnzIds_.find( src ) == mmEnzIds_.end() )
			innerAddMsg( src, enzIds_, "prd", dest, molIds_, "reac" );
		else
			innerAddMsg( src, mmEnzIds_, "prd", dest, molIds_, "reac" );
	}
	else if ( args[3] == "PLOT" ) { // Time-course output for molecule
		string head;
		string temp;
		dest = pathTail( args[2], head );
		string graph = pathTail( head, temp );
		temp = graph + "/" + dest;
		if ( args[4] == "Co" )
			innerAddMsg( temp, plotIds_, "requestData", src, molIds_, "get_conc" );
		else if ( args[4] == "n" )
		// innerAddMsg( src, molIds_, "nOut", dest, plotIds_, "input" );
			innerAddMsg( temp, plotIds_, "requestData", src, molIds_, "get_n" );
		else
			cout << "Unknown PLOT msg field '" << args[4] << "'\n";
	}
	else if ( args[3] == "SUMTOTAL" ) { // Summation function.
		buildSumTotal( src, dest );
	}
}
