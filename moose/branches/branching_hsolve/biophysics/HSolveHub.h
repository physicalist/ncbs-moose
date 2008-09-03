/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_HUB_H
#define _HSOLVE_HUB_H

/**
 * Biophysical elements in a neuronal model hand over control (computation,
 * fields, messages) to the solver. HSolveHub handles the fields and messages--
 * it can do so because the solver's data structures are shared between the
 * integrator and the hub.
 */
class HSolveHub
{
public:
	HSolveHub();
	
	///////////////////////////////////////////////////
	// Field functions
	///////////////////////////////////////////////////
	static unsigned int getNcompt( Eref e );
	
	///////////////////////////////////////////////////
	// Dest functions
	///////////////////////////////////////////////////
	static void compartmentFunc(
		const Conn* c,
		vector< double >* V,
		vector< Element* >* elist );
	static void channelFunc( const Conn* c, vector< Element* >* elist );
	static void spikegenFunc( const Conn* c, vector< Element* >* elist );
	static void synchanFunc( const Conn* c, vector< Element* >* elist );
	static void destroy( const Conn* c );
	static void childFunc( const Conn* c, int stage );
	
	///////////////////////////////////////////////////
	// Field functions (Biophysics)
	///////////////////////////////////////////////////
	static void setCompartmentVm( const Conn* c, double value );
	static double getCompartmentVm( Eref e );
	
	static void setCompartmentEm( const Conn* c, double value );
	static double getCompartmentEm( Eref e );
	
	// Im is read-only
	static double getCompartmentIm( Eref e );
	
	static void setCompartmentInject( const Conn* c, double value );
	static double getCompartmentInject( Eref e );
	
	static void setChannelGbar( const Conn* c, double value );
	static double getChannelGbar( Eref e );
	
	static void setSynChanGbar( const Conn* c, double value );
	static double getSynChanGbar( Eref e );
	
	///////////////////////////////////////////////////
	// Dest functions (Biophysics)
	///////////////////////////////////////////////////
	static void comptInjectMsgFunc( const Conn* c, double I );
	
private:
	void innerCompartmentFunc(
		Eref e,
		vector< double >* V,
		vector< Element* >* elist );
	void innerChannelFunc( Eref e, vector< Element* >* elist );
	void innerSpikegenFunc( Eref e, vector< Element* >* elist );
	void innerSynchanFunc( Eref e, vector< Element* >* elist );
	
	static void zombify( 
		Eref hub, Eref e,
		const Finfo* hubFinfo, Finfo* solveFinfo );
	static void unzombify( Element* e );
	static void clearFunc( Eref e );
	static void clearMsgsFromFinfo( Eref e, const Finfo * f );
	static void redirectDestMessages(
		Eref hub, Eref e,
		const Finfo* hubFinfo, const Finfo* eFinfo,
		unsigned int eIndex, vector< unsigned int >& map,
		vector< Element *>* elist, bool retain );
	static void redirectDynamicMessages( Element* e );
	static HSolveHub* getHubFromZombie( Eref e, unsigned int& index );
	
	vector< double >* V_;
	vector< double >* state_;
};

#endif // _HSOLVE_HUB_H
