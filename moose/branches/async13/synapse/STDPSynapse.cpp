/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SynHandlerBase.h"
#include "Synapse.h"
#include "STDPSynapse.h"

const Cinfo* STDPSynapse::initCinfo()
{

	static string doc[] = 
	{
		"Name", "STDPSynapse",
		"Author", "Aditya Gilra",
		"Description", "Subclass of Synapse including variables for Spike Timing Dependent Plasticity (STDP).",
	};

    static ValueFinfo< STDPSynapse, double > aPlus(
        "aPlus", 
        "aPlus is a pre-synaptic variable that keeps a decaying 'history' of previous pre-spike(s)"
        "and is used to update the synaptic weight when a post-synaptic spike appears."
        "It determines the t_pre < t_post (pre before post) part of the STDP window.",
		&STDPSynapse::setAPlus,
		&STDPSynapse::getAPlus
    );

    static ValueFinfo< STDPSynapse, double > aPlus0(
        "aPlus0", 
        "aPlus0 is added to aPlus on every pre-spike",
		&STDPSynapse::setAPlus0,
		&STDPSynapse::getAPlus0
    );

    static ValueFinfo< STDPSynapse, double > tauPlus(
        "tauPlus", 
        "aPlus decays with tauPlus time constant",
		&STDPSynapse::setTauPlus,
		&STDPSynapse::getTauPlus
    );

	static Finfo* synapseFinfos[] = {
		&aPlus,		// Field
		&aPlus0,	// Field
		&tauPlus,	// Field
	};

	static Dinfo< STDPSynapse > dinfo;
	static Cinfo STDPSynapseCinfo (
		"STDPSynapse",
		Synapse::initCinfo(),
		synapseFinfos,
		sizeof( synapseFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string ),
		true // This is a FieldElement.
	);

	return &STDPSynapseCinfo;
}

static const Cinfo* STDPSynapseCinfo = STDPSynapse::initCinfo();

STDPSynapse::STDPSynapse() : handler_ (0)
{
    aPlus_ = 0.0;
    tauPlus_ = 0.0;
    aPlus0_ = 0.0;
}

void STDPSynapse::setHandler( SynHandlerBase* h )
{
	handler_ = h;
    Synapse::setHandler( h );
}

void STDPSynapse::setAPlus0( const double v )
{
	aPlus0_ = v;
}

double STDPSynapse::getAPlus0() const
{
	return aPlus0_;
}

void STDPSynapse::setAPlus( const double v )
{
	aPlus_ = v;
}

double STDPSynapse::getAPlus() const
{
	return aPlus_;
}

void STDPSynapse::setTauPlus( const double v )
{
	tauPlus_ = v;
}

double STDPSynapse::getTauPlus() const
{
	return tauPlus_;
}
