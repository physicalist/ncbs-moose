/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ClockTick_h
#define _ClockTick_h
class ClockTick
{
	friend class ClockTickWrapper;
	public:
		ClockTick()
		{
			stage_ = 0.0;
			nextt_ = 0.0;
		}

	private:
		double stage_;
		double nextt_;
		double epsnextt_;
		double max_clocks_;
		double nclocks_;
		double dt_;
		bool terminate_;
};
#endif // _ClockTick_h
