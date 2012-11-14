/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef MOOSE_DUMMY_FINFO_H
#define MOOSE_DUMMY_FINFO_H


/*
*/
class DummyFinfo : public Finfo
{
	public:
		~DummyFinfo();
		DummyFinfo( const string& name, const string& doc);

		void registerFinfo( Cinfo* c );
		bool strSet( const Eref& tgt, const string& field, 
			const string& arg ) const;
		bool strGet( const Eref& tgt, const string& field, 
			string& returnValue ) const;
		
};

#endif // MOOSE_DUMMY_FINFO_H
