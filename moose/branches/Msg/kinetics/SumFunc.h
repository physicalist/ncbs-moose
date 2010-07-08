/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * SumFunc.
 * This is the MOOSE class to sum together assorted inputs.
 */
class SumFunc {
	public:
		SumFunc();
		void process( const Eref& e, ProcPtr info);
		void reinit( const Eref& e, ProcPtr info );
		void input( double d );
		double getResult() const;

		static const Cinfo* initCinfo();
	private:
		double result_;
};

