/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _INT_FIRE_H
#define _INT_FIRE_H


class IntFire
{
	friend void testStandaloneIntFire();
	friend void testSynapse();
	public: 
		IntFire();
		IntFire( double thresh, double tau );

		/**
 		 * Inserts an event into the pendingEvents queue for spikes.
 		 */
		void addSpike( DataId synIndex, const double time );
		
		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////
		
		void setVm( double v );
		double getVm() const;
		void setTau( double v );
		double getTau() const;
		void setThresh( double v );
		double getThresh() const;
		void setRefractoryPeriod( double v );
		double getRefractoryPeriod() const;
		void setNumSynapses( unsigned int v );
		unsigned int getNumSynapses() const;

		Synapse* getSynapse( unsigned int i );

		////////////////////////////////////////////////////////////////
		// Dest Func
		////////////////////////////////////////////////////////////////
		void process( Eref e, const Qinfo* q, ProcPtr p );
		void reinit( Eref e, const Qinfo* q, ProcPtr p );

		static const Cinfo* initCinfo();
	private:
		double Vm_; // State variable: Membrane potential. Resting pot is 0.
		double thresh_;	// Firing threshold
		double tau_; // Time course of membrane settling.
		double refractoryPeriod_; // Minimum time between successive spikes
		double lastSpike_; // Time of last action potential.
		vector< Synapse > synapses_;
		priority_queue< Synapse > pendingEvents_;
};

#endif // _INT_FIRE_H
