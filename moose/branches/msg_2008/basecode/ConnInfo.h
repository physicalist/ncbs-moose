/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla, and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _CONN_INFO_H
#define _CONN_INFO_H

class ConnInfo
{
	public:
		ConnInfo()
		{;}

		virtual ~ConnInfo()
		{;}

		virtual unsigned int slotIndex() const = 0;

	private:
};

#endif // _CONN_INFO_H
