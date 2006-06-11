#ifndef _HHChannel_h
#define _HHChannel_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
*********************************************************************
*/
class HHChannel
{
	friend class HHChannelWrapper;
	public:
		HHChannel()
		{
		}

	private:
		double Gbar_;
		double Ek_;
		double Xpower_;
		double Ypower_;
		double Zpower_;
		double surface_;
		int instant_;
		double Gk_;
		double Ik_;
		double X_;
		double Y_;
		double Z_;
		double g_;	
		double conc_;
		bool useConcentration_;	
		static const double EPSILON;
		static const int INSTANT_X;
		static const int INSTANT_Y;
		static const int INSTANT_Z;
};
#endif // _HHChannel_h
