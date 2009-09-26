/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef ACKPICKDATA_H
#define ACKPICKDATA_H

// An instance of this structure is sent back by the glcellclient process to the
// MOOSE GLcell element after every color-data packet reception during sync-mode
// and on every pick (of an object in the glcellclient viewer window by the
// user with the mouse) during non-sync mode.
// During sync-mode, if there has been no picking event, the first value stores
// false and the second stores zero; if there has been a picking event, the first
// stores true and the second stores the (unsigned int) id of the compartment
// that was picked. During non-sync mode, the first value is always true and
// the second always stores a valid id.

struct AckPickData
{
	bool wasSomethingPicked;
	unsigned int idPicked;

	template< typename Archive >
	void serialize( Archive& ar, const unsigned int version)
	{
		ar & wasSomethingPicked;
		ar & idPicked;
	}
};

#endif // ACKPICKDATA_H
