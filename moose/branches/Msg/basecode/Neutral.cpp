/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Neutral.h"
#include "Dinfo.h"

const Cinfo* Neutral::initCinfo()
{
	SrcFinfo0 child( "child", "Message to child Elements" );
	static DestFinfo parent( "parent", "Message from Parent Element(s)", 
			new EpFunc0< Neutral >( &Neutral::destroy ) );
	static ValueFinfo< Neutral, string > name( 
			"name",
			"Name of object", 
			&Neutral::setName, 
			&Neutral::getName );
	
	static Finfo* neutralFinfos[] = {
		&name,
		&child,
		&parent,
	};

	static Cinfo neutralCinfo (
		"Neutral",
		0, // No base class.
		neutralFinfos,
		sizeof( neutralFinfos ) / sizeof( Finfo* ),
		new Dinfo< Neutral >()
	);

	return &neutralCinfo;
}

static const Cinfo* neutralCinfo = Neutral::initCinfo();


Neutral::Neutral()
	: name_( "" )
{
	;
}

void Neutral::process( const ProcInfo* p, const Eref& e )
{
	;
}

void Neutral::setName( string name )
{
	name_ = name;
}

string Neutral::getName() const
{
	return name_;
}

void Neutral::destroy( Eref e, const Qinfo* q )
{
	// cout << "in Neutral::destroy()[ " << e.index() << "]\n";
	;
}
