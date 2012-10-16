/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "TableEntry.h"
#include "TableBase.h"
#include "Table.h"
// #include "UpFunc.h"

const Cinfo* moose::TableEntry::initCinfo()
{
		static ValueFinfo< moose::TableEntry, double > value(
			"value",
			"Data value in this entry",
			&moose::TableEntry::setValue,
			&moose::TableEntry::getValue
		);

	static Finfo* tableEntryFinfos[] = {
		&value,	// Field
	};

	static Cinfo tableEntryCinfo (
		"TableEntry",
		Neutral::initCinfo(),
		tableEntryFinfos,
		sizeof( tableEntryFinfos ) / sizeof ( Finfo* ),
		new Dinfo< moose::TableEntry >()
	);

	return &tableEntryCinfo;
}

static const Cinfo* tableEntryCinfo = moose::TableEntry::initCinfo();

moose::TableEntry::TableEntry()
	: value_( 1.0 )
{
	;
}

moose::TableEntry::TableEntry( double v )
	: value_( v )
{
	;
}

void moose::TableEntry::setValue( const double v )
{
	value_ = v;
}

double moose::TableEntry::getValue() const
{
	return value_;
}
