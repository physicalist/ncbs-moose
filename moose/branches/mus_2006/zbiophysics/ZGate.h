/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZGate_h
#define _ZGate_h
class ZGate
{
	friend class ZGateWrapper;
	public:
		ZGate()
		{
		}
		double alpha( double V );
		double beta( double V );

	private:
		double power_;
		double state_;
		int alphaForm_;
		double alpha_A_;
		double alpha_B_;
		double alpha_V0_;
		int betaForm_;
		double beta_A_;
		double beta_B_;
		double beta_V0_;
};
#endif // _ZGate_h
