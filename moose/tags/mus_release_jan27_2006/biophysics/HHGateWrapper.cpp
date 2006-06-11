#include <math.h>
#include "header.h"
#include "../builtins/Interpol.h"
#include "../builtins/InterpolWrapper.h"
#include "HHGate.h"
#include "HHGateWrapper.h"


Finfo* HHGateWrapper::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ValueFinfo< double >(
		"power", &HHGateWrapper::getPower, 
		&HHGateWrapper::setPower, "double" ),
	new ValueFinfo< double >(
		"state", &HHGateWrapper::getState, 
		&HHGateWrapper::setState, "double" ),
	new ObjFinfo< Interpol >(
		"A", &HHGateWrapper::getA,
		&HHGateWrapper::setA, &HHGateWrapper::lookupA, "Interpol"),
	new ObjFinfo< Interpol >(
		"B", &HHGateWrapper::getB,
		&HHGateWrapper::setB, &HHGateWrapper::lookupB, "Interpol"),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	new SingleSrc2Finfo< double, double >(
		"gateOut", &HHGateWrapper::getGateSrc, 
		"gateIn", 1 ),
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	new Dest3Finfo< double, double, double >(
		"gateIn", &HHGateWrapper::gateFunc,
		&HHGateWrapper::getGateConn, "gateOut", 1 ),
	new Dest1Finfo< double >(
		"reinitIn", &HHGateWrapper::reinitFunc,
		&HHGateWrapper::getGateConn, "", 1 ),
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
	new SharedFinfo(
		"gate", &HHGateWrapper::getGateConn,
		"gateIn, gateOut, reinitIn" ),
};

const Cinfo HHGateWrapper::cinfo_(
	"HHGate",
	"Upinder S. Bhalla, 2005, NCBS",
	"HHGate: Gate for Hodkgin-Huxley type channels, equivalent to the\nm and h terms on the Na squid channel and the n term on K.\nThis takes the voltage and state variable from the channel,\ncomputes the new value of the state variable and a scaling,\ndepending on gate power, for the conductance. These two\nterms are sent right back in a message to the channel.",
	"Neutral",
	HHGateWrapper::fieldArray_,
	sizeof(HHGateWrapper::fieldArray_)/sizeof(Finfo *),
	&HHGateWrapper::create
);

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HHGateWrapper::gateFuncLocal( double v, double state, double dt )
{
			double y = B_.doLookup( v );
			double x = exp( -y * dt );
			state = state * x + ( A_.doLookup( v ) / y ) * ( 1 - x );
			gateSrc_.send( state, takePower_( state ) );
}
void HHGateWrapper::reinitFuncLocal( double Vm )
{
			if ( power_ == 0.0 )
				takePower_ = power0;
			else if ( power_ == 1.0 )
				takePower_ = power1;
			else if ( power_ == 2.0 )
				takePower_ = power2;
			else if ( power_ == 3.0 )
				takePower_ = power3;
			else if ( power_ == 4.0 )
				takePower_ = power4;
	double x = A_.doLookup( Vm );
	double y = B_.doLookup( Vm );
	double z = x / y;
	gateSrc_.send( z, takePower_( z ) );
//	test();
}
///////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////
Element* gateConnHHGateLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHGateWrapper, gateConn_ );
	return reinterpret_cast< HHGateWrapper* >( ( unsigned long )c - OFFSET );
}

//////////////////////////////////////////////////////////////////
// Testing
//////////////////////////////////////////////////////////////////

/*
void HHGateWrapper::test()
{
	double dx = A_.localGetDx();
	double xmin = A_.localGetXmin();
	double xmax = A_.localGetXmax();
	double x = xmin - ( xmax - xmin ) / 3.0;
	int i;

	//double finalX = xmax + (xmax - xmin ) / 3.0;
	dx *= 1.6;
	cout << "/newplot\n";
	cout << "/plotname " << name() << ".A\n";
	for (i = 0; i <= A_.localGetXdivs(); i++) {
		cout << x << "	" << A_.doLookup( x ) << "\n";
		x += dx;
	}

	cout << "/newplot\n";
	cout << "/plotname " << name() << ".B\n";
	x = xmin - ( xmax - xmin ) / 3.0;
	for (i = 0; i <= B_.localGetXdivs(); i++) {
		cout << x << "	" << B_.doLookup( x ) << "\n";
		x += dx;
	}
}
*/
