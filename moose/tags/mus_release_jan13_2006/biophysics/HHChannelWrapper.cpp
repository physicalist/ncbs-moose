#include "header.h"
#include "HHChannel.h"
#include "HHChannelWrapper.h"

const double HHChannel::EPSILON = 1.0e-10;
const int HHChannel::INSTANT_X = 1;
const int HHChannel::INSTANT_Y = 2;
const int HHChannel::INSTANT_Z = 4;

Finfo* HHChannelWrapper::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ValueFinfo< double >(
		"Gbar", &HHChannelWrapper::getGbar, 
		&HHChannelWrapper::setGbar, "double" ),
	new ValueFinfo< double >(
		"Ek", &HHChannelWrapper::getEk, 
		&HHChannelWrapper::setEk, "double" ),
	new ValueFinfo< double >(
		"Xpower", &HHChannelWrapper::getXpower, 
		&HHChannelWrapper::setXpower, "double" ),
	new ValueFinfo< double >(
		"Ypower", &HHChannelWrapper::getYpower, 
		&HHChannelWrapper::setYpower, "double" ),
	new ValueFinfo< double >(
		"Zpower", &HHChannelWrapper::getZpower, 
		&HHChannelWrapper::setZpower, "double" ),
	new ValueFinfo< double >(
		"surface", &HHChannelWrapper::getSurface, 
		&HHChannelWrapper::setSurface, "double" ),
	new ValueFinfo< int >(
		"instant", &HHChannelWrapper::getInstant, 
		&HHChannelWrapper::setInstant, "int" ),
	new ValueFinfo< double >(
		"Gk", &HHChannelWrapper::getGk, 
		&HHChannelWrapper::setGk, "double" ),
	new ValueFinfo< double >(
		"Ik", &HHChannelWrapper::getIk, 
		&HHChannelWrapper::setIk, "double" ),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	new SingleSrc2Finfo< double, double >(
		"channelOut", &HHChannelWrapper::getChannelSrc, 
		"channelIn", 1 ),
	new SingleSrc1Finfo< double >(
		"IkOut", &HHChannelWrapper::getIkSrc, 
		"" ),
	new SingleSrc3Finfo< double, double, double >(
		"xGateOut", &HHChannelWrapper::getXGateSrc, 
		"channelIn", 1 ),
	new SingleSrc3Finfo< double, double, double >(
		"yGateOut", &HHChannelWrapper::getYGateSrc, 
		"channelIn", 1 ),
	new SingleSrc3Finfo< double, double, double >(
		"zGateOut", &HHChannelWrapper::getZGateSrc, 
		"channelIn", 1 ),
	new SingleSrc1Finfo< double >(
		"xGateReinitOut", &HHChannelWrapper::getXGateReinitSrc, 
		"reinitIn", 1 ),
	new SingleSrc1Finfo< double >(
		"yGateReinitOut", &HHChannelWrapper::getYGateReinitSrc, 
		"reinitIn", 1 ),
	new SingleSrc1Finfo< double >(
		"zGateReinitOut", &HHChannelWrapper::getZGateReinitSrc, 
		"reinitIn", 1 ),
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	new Dest2Finfo< double, ProcInfo >(
		"channelIn", &HHChannelWrapper::channelFunc,
		&HHChannelWrapper::getChannelConn, "xGateOut, yGateOut, zGateOut,channelOut", 1 ),
	new Dest1Finfo< double >(
		"EkIn", &HHChannelWrapper::EkFunc,
		&HHChannelWrapper::getEkInConn, "" ),
	new Dest1Finfo< double >(
		"concenIn", &HHChannelWrapper::concenFunc,
		&HHChannelWrapper::getConcenInConn, "" ),
	new Dest1Finfo< double >(
		"addGbarIn", &HHChannelWrapper::addGbarFunc,
		&HHChannelWrapper::getAddGbarInConn, "" ),
	new Dest1Finfo< double >(
		"reinitIn", &HHChannelWrapper::reinitFunc,
		&HHChannelWrapper::getReinitInConn, "xGateReinitOut, yGateReinitOut, zGateReinitOut" ),
	new Dest2Finfo< double, double >(
		"xGateIn", &HHChannelWrapper::xGateFunc,
		&HHChannelWrapper::getXGateConn, "", 1 ),
	new Dest2Finfo< double, double >(
		"yGateIn", &HHChannelWrapper::yGateFunc,
		&HHChannelWrapper::getYGateConn, "", 1 ),
	new Dest2Finfo< double, double >(
		"zGateIn", &HHChannelWrapper::zGateFunc,
		&HHChannelWrapper::getZGateConn, "", 1 ),
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
	new SharedFinfo(
		"channel", &HHChannelWrapper::getChannelConn,
		"channelOut, channelIn, reinitIn" ),
	new SharedFinfo(
		"xGate", &HHChannelWrapper::getXGateConn,
		"xGateOut, xGateIn, xGateReinitOut" ),
	new SharedFinfo(
		"yGate", &HHChannelWrapper::getYGateConn,
		"yGateOut, yGateIn, yGateReinitOut" ),
	new SharedFinfo(
		"zGate", &HHChannelWrapper::getZGateConn,
		"zGateOut, zGateIn, zGateReinitOut" ),
};

const Cinfo HHChannelWrapper::cinfo_(
	"HHChannel",
	"Upinder S. Bhalla, 2005, NCBS",
	"HHChannel: Hodgkin-Huxley type voltage-gated Ion channel. Something\nlike the old tabchannel from GENESIS, but also presents\na similar interface as hhchan from GENESIS. ",
	"Neutral",
	HHChannelWrapper::fieldArray_,
	sizeof(HHChannelWrapper::fieldArray_)/sizeof(Finfo *),
	&HHChannelWrapper::create
);

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void HHChannelWrapper::channelFuncLocal( double Vm, ProcInfo info )
{
	g_ += Gbar_;
	xGateSrc_.send( Vm, X_, info->dt_ );
	yGateSrc_.send( Vm, Y_, info->dt_ );
	if ( useConcentration_ )
		zGateSrc_.send( conc_, Z_, info->dt_ );
	else
		zGateSrc_.send( Vm, Z_, info->dt_ );
	// the state variables and conductance terms come back
	// from each gate during the above 'send' calls.
	Gk_ = g_;
	channelSrc_.send( Gk_, Ek_ );
	Ik_ = ( Ek_ - Vm ) * g_;
	g_ = 0.0;
}
void HHChannelWrapper::reinitFuncLocal( double Vm )
{
	g_ = Gbar_;
	xGateReinitSrc_.send( Vm );
	yGateReinitSrc_.send( Vm );
	useConcentration_ = concenInConn_.nTargets();
	if ( useConcentration_ )
		zGateReinitSrc_.send( conc_ );
	else
		zGateReinitSrc_.send( Vm );
	Gk_ = g_;
	channelSrc_.send( Gk_, Ek_ );
	Ik_ = ( Ek_ - Vm ) * g_;
	g_ = 0.0;
}
///////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////
Element* channelConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, channelConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* xGateConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, xGateConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* yGateConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, yGateConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* zGateConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, zGateConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* IkOutConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, IkOutConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* EkInConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, EkInConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* concenInConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, concenInConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* addGbarInConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, addGbarInConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* reinitInConnHHChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( HHChannelWrapper, reinitInConn_ );
	return reinterpret_cast< HHChannelWrapper* >( ( unsigned long )c - OFFSET );
}

