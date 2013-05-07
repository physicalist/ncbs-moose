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
	// Value Field access function definitions.
	virtual void   setVm( const Eref& e, const Qinfo* q, double value );
	virtual double getVm( const Eref& e, const Qinfo* q ) const;
	virtual void   setEm( const Eref& e, const Qinfo* q, double value );
	virtual double getEm( const Eref& e, const Qinfo* q ) const;
	virtual void   setCm( const Eref& e, const Qinfo* q, double value );
	virtual double getCm( const Eref& e, const Qinfo* q ) const;
	virtual void   setRm( const Eref& e, const Qinfo* q, double value );
	virtual double getRm( const Eref& e, const Qinfo* q ) const;
	virtual void   setRa( const Eref& e, const Qinfo* q, double value );
	virtual double getRa( const Eref& e, const Qinfo* q ) const;
	// Im is read-only.
	virtual double getIm( const Eref& e, const Qinfo* q ) const;
	virtual void   setInject( const Eref& e, const Qinfo* q, double value );
	virtual double getInject( const Eref& e, const Qinfo* q ) const;
	virtual void   setInitVm( const Eref& e, const Qinfo* q, double value );
	virtual double getInitVm( const Eref& e, const Qinfo* q ) const;
	virtual void   setDiameter(
		const Eref& e, const Qinfo* q, double value );
	virtual double getDiameter( const Eref& e, const Qinfo* q ) const;
	virtual void   setLength( const Eref& e, const Qinfo* q, double value );
	virtual double getLength( const Eref& e, const Qinfo* q ) const;
	virtual void   setX0( const Eref& e, const Qinfo* q, double value );
	virtual double getX0( const Eref& e, const Qinfo* q ) const;
	virtual void   setY0( const Eref& e, const Qinfo* q, double value );
	virtual double getY0( const Eref& e, const Qinfo* q ) const;
	virtual void   setZ0( const Eref& e, const Qinfo* q, double value );
	virtual double getZ0( const Eref& e, const Qinfo* q ) const;
	virtual void   setX( const Eref& e, const Qinfo* q, double value );
	virtual double getX( const Eref& e, const Qinfo* q ) const;
	virtual void   setY( const Eref& e, const Qinfo* q, double value );
	virtual double getY( const Eref& e, const Qinfo* q ) const;
	virtual void   setZ( const Eref& e, const Qinfo* q, double value );
	virtual double getZ( const Eref& e, const Qinfo* q ) const;
	
	// Dest function definitions.
	/**
	 * The process function does the object updating and sends out
	 * messages to channels, nernsts, and so on.
	 */
	virtual void process( const Eref& e, ProcPtr p );
	
	/**
	 * The reinit function reinitializes all fields.
	 */
	virtual void reinit( const Eref& e, ProcPtr p );
	
	/**
	 * The initProc function is for a second phase of 'process'
	 * operations. It sends the axial and raxial messages
	 * to other compartments. It has to be executed out of phase
	 * with the main process so that all compartments are 
	 * equivalent and there is no calling order dependence in 
	 * the results.
	 */
	virtual void initProc( const Eref& e, ProcPtr p );
	
	/**
	 * Empty function to do another reinit step out of phase
	 * with the main one. Nothing needs doing there.
	 */
	virtual void initReinit( const Eref& e, ProcPtr p );
	
	/**
	 * handleChannel handles information coming from the channel
	 * to the compartment
	 */
	virtual void handleChannel(
		const Eref& e, const Qinfo* q, double Gk, double Ek );
	
	/**
	 * handleRaxial handles incoming raxial message data.
	 */
	virtual void handleRaxial(
		const Eref& e, const Qinfo* q, double Ra, double Vm );
	
	/**
	 * handleAxial handles incoming axial message data.
	 */
	virtual void handleAxial(
		const Eref& e, const Qinfo* q, double Vm );
	
	/**
	 * Injects a constantly updated current into the compartment.
	 * Unlike the 'inject' field, this injected current is
	 * applicable only for a single timestep. So this is meant to
	 * be used as the destination of a message rather than as a
	 * one-time assignment.
	 */
	virtual void injectMsg(
		const Eref& e, const Qinfo* q, double current );
	
	/**
	 * Injects a constantly updated current into the
	 * compartment, with a probability prob. Note that it isn't
	 * the current amplitude that is random, it is the presence
	 * or absence of the current that is probabilistic.
	 */
	virtual void randInject(
		const Eref& e, const Qinfo* q, double prob, double current );
	
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
