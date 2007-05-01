/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ZChannel.h"
#include "ZChannelWrapper.h"

Finfo* ZChannelWrapper::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ValueFinfo< double >(
		"Gbar", &ZChannelWrapper::getGbar, 
		&ZChannelWrapper::setGbar, "double" ),
	new ValueFinfo< double >(
		"Ek", &ZChannelWrapper::getEk, 
		&ZChannelWrapper::setEk, "double" ),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
	new SingleSrc2Finfo< double, double >(
		"channelOut", &ZChannelWrapper::getChannelSrc, 
		"channelIn", 1 ),
	new SingleSrc0Finfo(
		"xGateOut", &ZChannelWrapper::getXGateSrc, 
		"" ),
	new SingleSrc0Finfo(
		"yGateOut", &ZChannelWrapper::getYGateSrc, 
		"" ),
	new SingleSrc0Finfo(
		"zGateOut", &ZChannelWrapper::getZGateSrc, 
		"" ),
/*	new SingleSrc3Finfo< double, double, double >(
		"xGateOut", &ZChannelWrapper::getXGateSrc, 
		"channelIn", 1 ),
	new SingleSrc3Finfo< double, double, double >(
		"yGateOut", &ZChannelWrapper::getYGateSrc, 
		"channelIn", 1 ),
	new SingleSrc3Finfo< double, double, double >(
		"zGateOut", &ZChannelWrapper::getZGateSrc, 
		"channelIn", 1 ),
	new SingleSrc3Finfo< double, double, int >(
		"xGateReinitOut", &ZChannelWrapper::getXGateReinitSrc, 
		"reinitIn", 1 ),
	new SingleSrc3Finfo< double, double, int >(
		"yGateReinitOut", &ZChannelWrapper::getYGateReinitSrc, 
		"reinitIn", 1 ),
	new SingleSrc3Finfo< double, double, int >(
		"zGateReinitOut", &ZChannelWrapper::getZGateReinitSrc, 
		"reinitIn", 1 ),*/
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	new Dest2Finfo< double, ProcInfo >(
		"channelIn", &ZChannelWrapper::channelFunc,
		&ZChannelWrapper::getChannelConn, "xGateOut, yGateOut, zGateOut,channelOut", 1 ),
	new Dest1Finfo< double >(
		"reinitIn", &ZChannelWrapper::reinitFunc,
		&ZChannelWrapper::getReinitInConn, "" ),
/*	new Dest1Finfo< double >(
		"reinitIn", &ZChannelWrapper::reinitFunc,
		&ZChannelWrapper::getReinitInConn, "xGateReinitOut, yGateReinitOut, zGateReinitOut" ),
	new Dest2Finfo< double, double >(
		"xGateIn", &ZChannelWrapper::xGateFunc,
		&ZChannelWrapper::getXGateConn, "", 1 ),
	new Dest2Finfo< double, double >(
		"yGateIn", &ZChannelWrapper::yGateFunc,
		&ZChannelWrapper::getYGateConn, "", 1 ),
	new Dest2Finfo< double, double >(
		"zGateIn", &ZChannelWrapper::zGateFunc,
		&ZChannelWrapper::getZGateConn, "", 1 ),*/
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
	new SharedFinfo(
		"channel", &ZChannelWrapper::getChannelConn,
		"channelOut, channelIn, reinitIn" ),
/*	new SharedFinfo(
		"xGate", &ZChannelWrapper::getXGateConn,
		"xGateOut, xGateIn, xGateReinitOut" ),
	new SharedFinfo(
		"yGate", &ZChannelWrapper::getYGateConn,
		"yGateOut, yGateIn, yGateReinitOut" ),
	new SharedFinfo(
		"zGate", &ZChannelWrapper::getZGateConn,
		"zGateOut, zGateIn, zGateReinitOut" ),*/
};

const Cinfo ZChannelWrapper::cinfo_(
	"ZChannel",
	"Niraj Dudani, 2007, NCBS; Upinder S. Bhalla, 2005, NCBS",
	"ZChannel: ",
	"Neutral",
	ZChannelWrapper::fieldArray_,
	sizeof(ZChannelWrapper::fieldArray_)/sizeof(Finfo *),
	&ZChannelWrapper::create
);

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////
void ZChannelWrapper::channelFuncLocal( double Vm, ProcInfo info ) { ; }
void ZChannelWrapper::reinitFuncLocal( double Vm ) { ; }

///////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////
Element* channelConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, channelConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* xGateOutConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, xGateOutConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* yGateOutConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, yGateOutConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* zGateOutConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, zGateOutConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

/*
Element* xGateConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, xGateConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* yGateConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, yGateConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

Element* zGateConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, zGateConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}*/

Element* reinitInConnZChannelLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZChannelWrapper, reinitInConn_ );
	return reinterpret_cast< ZChannelWrapper* >( ( unsigned long )c - OFFSET );
}

///////////////////////////////////////////////////
// Create function.
///////////////////////////////////////////////////

void addZGate( Element* chan, Conn* tgt, const string& name )
{
	if ( tgt ) {
		Element* gate = tgt->parent();
		if ( gate ) {
			Field temp( gate, "gate" );
			chan->field( name ).add( temp );
		}
	}
}

Element* ZChannelWrapper::create(
	const string& name, Element* pa, const Element* proto )
{
	const ZChannelWrapper* p = 
		dynamic_cast<const ZChannelWrapper *>(proto);

	ZChannelWrapper *ret = new ZChannelWrapper( name );

	if ( p ) {
		ret->Gbar_ = p->Gbar_;
		ret->Ek_ = p->Ek_;
/*
		addZGate( ret, p->xGateConn_.target( 0 ), "xGate" );
		addZGate( ret, p->yGateConn_.target( 0 ), "yGate" );
		addZGate( ret, p->zGateConn_.target( 0 ), "zGate" );*/
	}
	return ret;
}
