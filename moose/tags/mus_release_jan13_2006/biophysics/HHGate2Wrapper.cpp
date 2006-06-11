#include "header.h"
#include "../builtins/Interpol.h"
#include "../builtins/InterpolWrapper.h"
#include "HHGate2.h"
#include "HHGate2Wrapper.h"


Finfo* HHGate2Wrapper::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ValueFinfo< double >(
		"power", &HHGate2Wrapper::getPower, 
		&HHGate2Wrapper::setPower, "double" ),
	new ValueFinfo< double >(
		"state", &HHGate2Wrapper::getState, 
		&HHGate2Wrapper::setState, "double" ),
	new ObjFinfo< Interpol >(
		"A", &HHGate2Wrapper::getA,
		&HHGate2Wrapper::setA, &HHGate2Wrapper::lookupA, "Interpol"),
	new ObjFinfo< Interpol >(
		"B", &HHGate2Wrapper::getB,
		&HHGate2Wrapper::setB, &HHGate2Wrapper::lookupB, "Interpol"),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	new Return2Finfo< double, double >(
		"gateOut", &HHGate2Wrapper::getGateMultiReturnConn, 
		"gateIn, reinitIn", 1 ),
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	new Dest3Finfo< double, double, double >(
		"gateIn", &HHGate2Wrapper::gateFunc,
		&HHGate2Wrapper::getGateConn, "gateOut", 1 ),
	new Dest1Finfo< double >(
		"reinitIn", &HHGate2Wrapper::reinitFunc,
		&HHGate2Wrapper::getGateConn, "gateOut", 1 ),
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
	new SharedFinfo(
		"gate", &HHGate2Wrapper::getGateConn,
		"gateIn, gateOut, reinitIn" ),
};

const Cinfo HHGate2Wrapper::cinfo_(
	"HHGate2",
	"Upinder S. Bhalla, 2005, NCBS",
	"HHGate2: Gate for Hodkgin-Huxley type channels, equivalent to the\nm and h terms on the Na squid channel and the n term on K.\nThis takes the voltage and state variable from the channel,\ncomputes the new value of the state variable and a scaling,\ndepending on gate power, for the conductance. These two\nterms are sent right back in a message to the channel.",
	"Neutral",
	HHGate2Wrapper::fieldArray_,
	sizeof(HHGate2Wrapper::fieldArray_)/sizeof(Finfo *),
	&HHGate2Wrapper::create
);

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HHGate2Wrapper::gateFuncLocal( Conn* c, 
	double v, double state, double dt )
{
	ReturnConn* rc = static_cast< ReturnConn* >( c );

	double y = B_.doLookup( v );
	double x = exp( -y * dt );
	state = state * x + ( A_.doLookup( v ) / y ) * ( 1 - x );
	// This ugly construction returns the info back to sender.
	reinterpret_cast< void ( * )( Conn*, double, double ) >(
		rc->recvFunc() )
		( rc->rawTarget(), state, takePower_( state ) );
}

void HHGate2Wrapper::reinitFuncLocal( Conn* c, double Vm )
{
	ReturnConn* rc = static_cast< ReturnConn* >( c );
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
	//gateSrc_.send( z, takePower_( z ) );
	reinterpret_cast< void ( * )( Conn*, double, double ) >(
		rc->recvFunc() )
		( rc->rawTarget(), z, takePower_( z ) );
}
///////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////
