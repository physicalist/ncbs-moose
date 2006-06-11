/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <math.h>

// Read a cell model into MOOSE.
// Comments are as in C++
//
// Special command lines start with a '*'
// Commands include:
// Coordinate system:
// 		*cartesian	*polar	*cylindrical
// Measurement reference:
//		*relative *absolute
// Compartment type:
//		*symmetric *asymmetric
//
// Value assignment:
//		*set_global <name> <value>
//
// Main data lines are of form
// name  parent  x       y       z       d       ch      dens ...
// name is name of new compartment
// arent = name of preceding compartment, or . 
// 		if it is the same as the last one
// x y z are the coordinates, in whatever system is chosen
// 

enum CoordMode { CARTESIAN, POLAR, CYLINDRICAL };
static const double PI = 3.1415926535;

class ReadCell
{
	public:
		ReadCell( const string& fileName, const string& cellName,
			const string& options );
		void calculateCoords( Element* compt, Element* parent,
			double x, double y, double z );
		bool readComptLine( const string& line );
		static void chopLine( 
			const string& line, vector< string >& args );
		void makeChannel( 
			Element* compt, const string& name, double density );

	private:
		Element* cell_;
		Element* lastCompt_;
		Element* library_;
		CoordMode coordMode_;
		bool isRelative_;
		bool isSymmetric_;
		unsigned long lineNum_;
};

ReadCell::ReadCell( const string& fileName, const string& cellName,
	const string& options )
	: cell_( 0 ), lastCompt_( 0 ), coordMode_( CARTESIAN ),
		isRelative_( 0 ), isSymmetric_( 0 ), lineNum_( 0 )
{
	cell_ = Element::root()->relativeFind( cellName );
	if ( !cell_ ) {
		cerr << "Error:ReadCell::ReadCell: parent cell not found: '" <<
			cellName << "'\n";
		return;
	}
	library_ = Element::root()->relativeFind( "library" );
	if ( !library_ ) {
		cerr << "Error:ReadCell::ReadCell: /library not found \n";
		return;
	}
	ifstream f( fileName.c_str() );
	string line;
	lineNum = 0;
	f.getline( line );
	while ( f.good() ) {
		lineNum_++;
		if ( checkComment( line ) )
			continue;

		if line[ 0 ] == '*' {
			if ( line == "*cartesian" )
				coordMode = CARTESIAN;
			else if ( line == "*polar" )
				coordMode = POLAR;
			else if ( line == "*cylindrical" )
				coordMode = CYLINDRICAL;
			else if ( line == "*relative" )
				isRelative = 1;
			else if ( line == "*absolute" )
				isRelative = 0;
			else if ( line == "*symmetric" )
				isSymmetric = 1;
			else if ( line == "*asymmetric" )
				isSymmetric = 0;
			else if ( line.substr( 0, 11 ) == "*set_global" )
				setGlobal( line.substr( 12 ) );
			else
				cerr << "Error:ReadCell::ReadCell: Unknown option '" <<
					line << "'\n";
		} else {
			readComptLine( line );
		}
		f.getline( line );
	}
}

// Returns 1 if line is read, 0 otherwise
void ReadCell::chopLine( const string& line, vector< string >& args )
{
	unsigned long p = line.find_first_not_of( " \t" );
	if ( p == string::npos )
		return;
	string s = line;
	while ( p != string::npos ) {
		s = s.substr( p );
		p = s.find_first_of( " \t" );
		temp = s.substr( 0, p );
		args.push_back( temp );
		if ( p == string::npos )
			break;
		s = s.substr( p );
		p = s.find_first_not_of( " \t" );
	}
}


bool ReadCell::readComptLine( const string& line )
{
	static const Cinfo* comptCi = Cinfo::find( "Compartment" );
	static const Cinfo* symComptCi = Cinfo::find( "SymCompartment" );
	vector< string > args;
	chopLine( line, args );
	if ( ( args.size() < 6 || args.size() ) % 2 != 0 ) {
		cerr << "Error: ReadCell::readComptLine: line " << lineNum <<
			": Wrong number of args in '" << line << "'\n";
		return 0;
	}

	Element* compt;	
	unsigned int numChans = 0;
	if ( isSymmetric )
		compt = symComptci->create( args[0], cell );
	else
		compt = comptci->create( args[0], cell );
	
	if ( args[1] == "." ) // Parent compt
		parent = lastCompt;
	else 
		parent = cell->RelativeFind( args[1] );

	if ( !parent ) {
		cerr << "Error: ReadCell::readComptLine: line " << lineNum <<
			": parent compt not found: '" << args[1] << "'\n";
		return 0;
	}
	
	parent->field( "axial" ).add( compt->field( "raxial" ) );
	
	calculateCoords( compt, parent,
		atof( args[ 2 ].c_str() ), 
		atof( args[ 3 ].c_str() ), 
		atof( args[ 4 ].c_str() ) ); 

	Ftype1< double >::set( compt, "diameter", 
		atof( args[ 5 ].c_str() ) );

	for ( i = 6 ; i < args.size(); i += 2 )
		makeChannel( compt, args[ i ], atof( args[ i + 1 ].c_str() ) );

	lastCompt = compt;
	return 1;
}

void ReadCell::calculateCoords( Element* compt, element* parent,
	double x, double y, double z )
{
	double paX, paY, paZ;
	double myX, myY, myZ;
	double length = 0.0;
	switch ( coordMode ) {
		case CARTESIAN :
			if ( isRelative ) {
				length = sqrt( x * x + y * y + z * z );
				myX = paX + x;
				myY = paY + y;
				myZ = paZ + z;
			} else {
				length = sqrt( x * x + y * y + z * z );
				myX = x;
				myY = y;
				myZ = z;
			}
			break;
		case POLAR :
			break
		case CYLINDRICAL :
			break
	}
	Ftype1< double >::set( compt, "length", length );
	/*
	Ftype1< double >::set( compt, "x", mX );
	Ftype1< double >::set( compt, "y", mY );
	Ftype1< double >::set( compt, "z", mZ );
	*/
}

void ReadCell::makeChannel( 
			Element* compt, const string& name, double density )
{
	Element* chan = library_->RelativeFind( name );
	if ( !chan ) {
		cerr << "Error: ReadCell::makeChannel: line " << lineNum <<
			": channel not found: '" << name << "'\n";
	}

	compt->field( "channel" ).add( chan->field( "channel" ) );
	if ( density > 0 ) {
		double diameter;
		double length;
		Ftype1< double >::get( compt, "diameter", diameter );
		Ftype1< double >::get( compt, "length", length );
		density *= diameter * length * PI;

		Ftype1< double >::set( chan, "Gbar", density );
	} else {
		Ftype1< double >::set( chan, "Gbar", -density );
	}
}
