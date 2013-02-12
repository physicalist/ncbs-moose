/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _FUNC_POOL_H
#define _FUNC_POOL_H

/**
 * This class acts as a relay for values generated by other objects.
 * It is typically used for SumTotals for legacy kkit models, and
 * for arbitrary functions for things loaded up by SBML.
 * When acting as a SumTotal it has a child SumTot object that does
 * the arithmetic and sends it the total.
 */
class FuncPool: public Pool
{
	public: 
		FuncPool();
		~FuncPool();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );
		void input( const Eref& e, const Qinfo* q, double v );

		static const Cinfo* initCinfo();
	private:
};

#endif	// _FUNC_POOL_H
