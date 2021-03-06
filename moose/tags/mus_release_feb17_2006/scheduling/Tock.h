/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _Tock_h
#define _Tock_h
class Tock
{
	friend class TockWrapper;
	public:
		Tock()
		{
		}
		Tock( const string& label )
			: label_ (label)
		{
			;
		}

	private:
		string label_;
};
#endif // _Tock_h
