/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZChannel_h
#define _ZChannel_h

class ZChannel
{
	friend class ZChannelWrapper;
	public:
		ZChannel()
			: Gbar_( 0.0 ), Ek_( 0.0 )
		{
			;
		}

	private:
		double Gbar_;
		double Ek_;
};
#endif // _ZChannel_h
