/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../builtins/Interpol.h"
#include "../builtins/InterpolWrapper.h"
#include "ZGate.h"
#include "ZGateWrapper.h"

Finfo* ZGateWrapper::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ValueFinfo< double >(
		"power", &ZGateWrapper::getPower, 
		&ZGateWrapper::setPower, "double" ),
	new ValueFinfo< double >(
		"state", &ZGateWrapper::getState, 
		&ZGateWrapper::setState, "double" ),
	new ValueFinfo< int >(
		"alphaForm", &ZGateWrapper::getAlphaForm, 
		&ZGateWrapper::setAlphaForm, "int" ),
	new ValueFinfo< double >(
		"alpha_A", &ZGateWrapper::getAlpha_A, 
		&ZGateWrapper::setAlpha_A, "double" ),
	new ValueFinfo< double >(
		"alpha_B", &ZGateWrapper::getAlpha_B, 
		&ZGateWrapper::setAlpha_B, "double" ),
	new ValueFinfo< double >(
		"alpha_V0", &ZGateWrapper::getAlpha_V0, 
		&ZGateWrapper::setAlpha_V0, "double" ),
	new ValueFinfo< int >(
		"betaForm", &ZGateWrapper::getBetaForm, 
		&ZGateWrapper::setBetaForm, "int" ),
	new ValueFinfo< double >(
		"beta_A", &ZGateWrapper::getBeta_A, 
		&ZGateWrapper::setBeta_A, "double" ),
	new ValueFinfo< double >(
		"beta_B", &ZGateWrapper::getBeta_B, 
		&ZGateWrapper::setBeta_B, "double" ),
	new ValueFinfo< double >(
		"beta_V0", &ZGateWrapper::getBeta_V0, 
		&ZGateWrapper::setBeta_V0, "double" ),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
/*	new Return2Finfo< double, double >(
		"gateOut", &ZGateWrapper::getGateMultiReturnConn, 
		"gateIn, reinitIn", 1 ),*/
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
	new Dest0Finfo(
		"gateIn", &ZGateWrapper::gateFunc,
		&ZGateWrapper::getGateInConn, "" ),
/*	new Dest3Finfo< double, double, double >(
		"gateIn", &ZGateWrapper::gateFunc,
		&ZGateWrapper::getGateConn, "gateOut", 1 ),
	new Dest3Finfo< double, double, int >(
		"reinitIn", &ZGateWrapper::reinitFunc,
		&ZGateWrapper::getGateConn, "gateOut", 1 ),*/
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
/*	new SharedFinfo(
		"gate", &ZGateWrapper::getGateConn,
		"gateIn, gateOut, reinitIn" ),*/
};

const Cinfo ZGateWrapper::cinfo_(
	"ZGate",
	"Niraj Dudani, 2007, NCBS; Upinder S. Bhalla, 2005, NCBS",
	"ZGate: Zombie/dummy gate. Calculates rate constants alpha and beta.\nHas messaging interface similar to HHGate.\nMeant for use by the new HSolve. Only temporary.",
	"Neutral",
	ZGateWrapper::fieldArray_,
	sizeof(ZGateWrapper::fieldArray_)/sizeof(Finfo *),
	&ZGateWrapper::create
);

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////
/*
void ZGateWrapper::gateFuncLocal( Conn* c, 
	double v, double state, double dt )
{
}

void ZGateWrapper::reinitFuncLocal( Conn* c, double Vm, double power,
	int instant )
{
}*/
///////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////
Element* gateInConnZGateLookup( const Conn* c )
{
	static const unsigned long OFFSET =
		FIELD_OFFSET ( ZGateWrapper, gateInConn_ );
	return reinterpret_cast< ZGateWrapper* >( ( unsigned long )c - OFFSET );
}
