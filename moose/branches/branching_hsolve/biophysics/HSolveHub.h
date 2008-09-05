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
 * fields, messages) to the solver. The integrator (HSolve) takes care of the
 * computation, while the hub (HSolveHub) handles field requests and incoming
 * messages. It can do so because it has access to HSolve's data through an
 * interface.
 */
class HSolveHub
{
public:
	HSolveHub();
	
	///////////////////////////////////////////////////
	// Field functions
	///////////////////////////////////////////////////
	
	///////////////////////////////////////////////////
	// Dest functions
	///////////////////////////////////////////////////
	static void hubFunc( const Conn* c, HSolveActive* integ );
	static void destroy( const Conn* c );
	static void childFunc( const Conn* c, int stage );
	
	///////////////////////////////////////////////////
	// Field functions (Biophysics)
	///////////////////////////////////////////////////
	static void setVm( const Conn* c, double value );
	static double getVm( Eref e );
	
	// Im is read-only
	static double getIm( Eref e );
	
	static void setInject( const Conn* c, double value );
	static double getInject( Eref e );
	
	static void setChannelGbar( const Conn* c, double value );
	static double getChannelGbar( Eref e );
	
	static void setSynChanGbar( const Conn* c, double value );
	static double getSynChanGbar( Eref e );
	
	///////////////////////////////////////////////////
	// Dest functions (Biophysics)
	///////////////////////////////////////////////////
	static void comptInjectMsgFunc( const Conn* c, double value );
	
private:
	void innerHubFunc( Eref hub, HSolveActive* integ );
	void manageCompartments( );
	
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
		vector< Element *>*  elist, bool retain );
	static void redirectDynamicMessages( Element* e );
	static HSolveHub* getHubFromZombie( Eref e, unsigned int& index );
	
	Eref hub_;
	HSolveActive* integ_;
	vector< unsigned int > comptInjectMap_;
};

#endif // _HSOLVE_HUB_H
