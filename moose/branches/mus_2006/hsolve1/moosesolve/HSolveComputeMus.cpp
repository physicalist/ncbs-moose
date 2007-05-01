/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <vector>
#include <fstream>
#include <iomanip>
#include "header.h"
#include "../../biophysics/Compartment.h"
#include "../../biophysics/CompartmentWrapper.h"
#include "../HSolveCompute.h"
#include "HSolveModelMus.h"
#include "HSolveComputeMus.h"

Finfo* HSolveComputeMus::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ValueFinfo< int >(
		"NDiv", &HSolveComputeMus::getNDiv, 
		&HSolveComputeMus::setNDiv, "int" ),
	new ValueFinfo< double >(
		"VLo", &HSolveComputeMus::getVLo, 
		&HSolveComputeMus::setVLo, "double" ),
	new ValueFinfo< double >(
		"VHi", &HSolveComputeMus::getVHi, 
		&HSolveComputeMus::setVHi, "double" ),
///////////////////////////////////////////////////////
// EvalField definitions
///////////////////////////////////////////////////////
	new ValueFinfo< string >(
		"path", &HSolveComputeMus::getPath, 
		&HSolveComputeMus::setPath, "string" ),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	new Dest0Finfo(
		"reinitIn", &HSolveComputeMus::reinitFunc,
		&HSolveComputeMus::getProcessConn, "", 1 ),
	new Dest1Finfo< double >(
		"reinitializeIn", &HSolveComputeMus::reinitializeFunc,
		&HSolveComputeMus::getReinitializeInConn, "" ),
	new Dest1Finfo< ProcInfo >(
		"processIn", &HSolveComputeMus::processFunc,
		&HSolveComputeMus::getProcessConn, "", 1 ),
	new Dest1Finfo< string >(
		"writeConduitIn", &HSolveComputeMus::writeConduitFunc,
		&HSolveComputeMus::getWriteConduitInConn, "" ),
	new Dest1Finfo< string >(
		"recordCompartmentIn", &HSolveComputeMus::recordCompartmentFunc,
		&HSolveComputeMus::getRecordCompartmentInConn, "" ),
	new Dest2Finfo< string, int >(
		"printVoltageTraceIn", &HSolveComputeMus::printVoltageTraceFunc,
		&HSolveComputeMus::getPrintVoltageTraceInConn, "" ),
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
	new SharedFinfo(
		"process", &HSolveComputeMus::getProcessConn,
		"processIn, reinitIn" ),
};

const Cinfo HSolveComputeMus::cinfo_(
	"HSolve1",
	"Niraj Dudani, 2007, NCBS",
	"HSolve: Hines solver equipped to handle branched nerve with HH channels.",
	"Neutral",
	HSolveComputeMus::fieldArray_,
	sizeof(HSolveComputeMus::fieldArray_)/sizeof(Finfo *),
	&HSolveComputeMus::create
);

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// EvalField function definitions
///////////////////////////////////////////////////

string HSolveComputeMus::localGetPath() const
{
			return path_;
}
void HSolveComputeMus::localSetPath( string value ) {
			this->setPath( value );
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HSolveComputeMus::recordCompartmentFuncLocal( string path )
{
			vector< Element* > target;
			if ( Element::wildcardFind( path, target ) != 1 )
				return;
			for ( unsigned long ci = 0; ci < compartment_.size(); ++ci )
				if ( compartment_[ ci ] == target[ 0 ] ) {
					recordPath_.push_back( path );
					recordIndex_.push_back( ci );
					plotVolt_.resize( 1 + plotVolt_.size() );
					break;
				}
}
void HSolveComputeMus::printVoltageTraceFuncLocal( string fileName, int plotMode )
{
			ofstream fout( fileName.c_str(), plotMode == 1 ? ios::out : ios::app );
			fout << "#Time   ";
			for ( unsigned long i = 0; i < recordPath_.size(); ++i )
				fout << "\t" << recordPath_[ i ];
			for ( unsigned long i = 0; i < plotTime_.size(); ++i ) {
				fout << "\n" << fixed << setw( 8 ) << plotTime_[ i ];
				fout << scientific << setprecision( 6 );
				for ( unsigned long j = 0; j < plotVolt_.size(); ++j )
					fout << "\t" << plotVolt_[ j ][ i ];
			}
			fout << endl;
			fout.close();
}
///////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////
Element* processConnHSolveComputeMusLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HSolveComputeMus, processConn_ );
	return reinterpret_cast< HSolveComputeMus* >( ( unsigned long )c - OFFSET );
}

Element* reinitializeInConnHSolveComputeMusLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HSolveComputeMus, reinitializeInConn_ );
	return reinterpret_cast< HSolveComputeMus* >( ( unsigned long )c - OFFSET );
}

Element* writeConduitInConnHSolveComputeMusLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HSolveComputeMus, writeConduitInConn_ );
	return reinterpret_cast< HSolveComputeMus* >( ( unsigned long )c - OFFSET );
}

Element* recordCompartmentInConnHSolveComputeMusLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HSolveComputeMus, recordCompartmentInConn_ );
	return reinterpret_cast< HSolveComputeMus* >( ( unsigned long )c - OFFSET );
}

Element* printVoltageTraceInConnHSolveComputeMusLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HSolveComputeMus, printVoltageTraceInConn_ );
	return reinterpret_cast< HSolveComputeMus* >( ( unsigned long )c - OFFSET );
}

///////////////////////////////////////////////////
// Other function definitions
///////////////////////////////////////////////////
void HSolveComputeMus::setPath( const string& path ) {
	vector< Element* > seed;
	CompartmentWrapper dummy( "dummy" );
	int count = Element::wildcardFind( path, seed );
	if ( count == 0 )
		cerr << "Error: Invalid path set in \'" << this->path() << "\'.\n";
	else if ( count > 1 )
		cerr << "Error: Path in \'" << this->path() << "\' should point to exactly 1 element.\n";
	else if ( !seed[ 0 ]->cinfo()->isA( dummy.cinfo() ) )
		cerr << "Error: Path in \'" << this->path() << "\' should point to type \'Compartment\'.\n";
	else {
		seed_ = seed[ 0 ];
		path_ = path;
	}
}
void HSolveComputeMus::storeRecordings( double currTime ) {
	if ( recordIndex_.size() == 0 )
		return;
	plotTime_.push_back( currTime );
	for ( unsigned long i = 0; i < plotVolt_.size(); ++i )
		plotVolt_[ i ].push_back (
			V_[ recordIndex_[ i ] ]
		);
}
#define STREAM_OUT( STREAM, DATA ) \
	( ( STREAM ).write( reinterpret_cast< const char* >( &( DATA ) ), sizeof( DATA ) ) )
#define STREAM_OUT_VECTOR( STREAM, DATA ) \
	do { \
		unsigned long vSize = ( DATA ).size( ); \
		STREAM_OUT( ( STREAM ), vSize ); \
		if ( vSize ) \
			( STREAM ).write( \
				reinterpret_cast< const char* >( &( DATA )[ 0 ] ), \
				sizeof( DATA )[ 0 ] * vSize \
			); \
	} while( 0 )
void HSolveComputeMus::writeConduit( const string& fileName ) const {
	ofstream fout( fileName.c_str(), ios::binary );
	STREAM_OUT_VECTOR( fout, checkpoint_ );
	STREAM_OUT( fout, N_ );
	STREAM_OUT( fout, VLo_ );
	STREAM_OUT( fout, VHi_ );
	STREAM_OUT( fout, NDiv_ );
	STREAM_OUT( fout, dV_ );
	double Inject, Vm, Em, Cm, Rm, Ra;
	for ( unsigned long ci = 0; ci < N_; ++ci ) {
		model_.getFieldDouble( compartment_[ ci ], "Inject", Inject );
		model_.getFieldDouble( compartment_[ ci ], "Vm", Vm );
		model_.getFieldDouble( compartment_[ ci ], "Em", Em );
		model_.getFieldDouble( compartment_[ ci ], "Cm", Cm );
		model_.getFieldDouble( compartment_[ ci ], "Rm", Rm );
		model_.getFieldDouble( compartment_[ ci ], "Ra", Ra );
		STREAM_OUT( fout, Inject );
		STREAM_OUT( fout, Vm );
		STREAM_OUT( fout, Em );
		STREAM_OUT( fout, Cm );
		STREAM_OUT( fout, Rm );
		STREAM_OUT( fout, Ra );
	}
	STREAM_OUT_VECTOR( fout, channelCount_ );
	STREAM_OUT_VECTOR( fout, gateCount_ );
	STREAM_OUT_VECTOR( fout, gateCount1_ );
	STREAM_OUT_VECTOR( fout, Gbar_ );
	STREAM_OUT_VECTOR( fout, GbarEk_ );
	STREAM_OUT_VECTOR( fout, state_ );
	STREAM_OUT_VECTOR( fout, power_ );
	STREAM_OUT_VECTOR( fout, lookup_ );
	STREAM_OUT_VECTOR( fout, gateFamily_ );
	STREAM_OUT( fout, lookupBlocSize_ );
	STREAM_OUT_VECTOR( fout, recordIndex_ );
	fout.close( );
}
