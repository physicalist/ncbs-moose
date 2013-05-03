/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file copyleft for the full notice.
**********************************************************************/

#ifndef _COMPARTMENT_WRAPPER_H
#define _COMPARTMENT_WRAPPER_H

/**
 * This class is a thin wrapper around a CompartmentInterface object, and
 * registers it as a MOOSE class.
 * 
 * It holds a pointer to a CompartmentInterface object, and forwards all
 * function calls to it. The specialized CompartmentInterface object can be
 * either a regular Compartment, or a ZCompartment, which in turn forwards all
 * function calls to the Hines' solver.
 */
class CompartmentWrapper: public CompartmentInterface
{
public:
	/**
	 * Initializes the class info.
	 */
	static const Cinfo* initCinfo();
	
	/*
	 * This Finfo is used to send out Vm to channels, spikegens, etc.
	 * 
	 * It is exposed here so that HSolve can also use it to send out
	 * the Vm to the recipients.
	 */
	static SrcFinfo1< double >* VmOut();
	
private:
	CompartmentInterface* compartment_;
};

#endif
