/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _ELEMENT_VALUE_FINFO_H
#define _ELEMENT_VALUE_FINFO_H

/**
 * This variant of ValueFinfo provides facilities to set and get
 * values of fields that require information about the managing
 * Element and possibly the Qinfo as well.
 */
template < class T, class F > class ElementValueFinfo: public Finfo
{
	public:
		~ElementValueFinfo() {
			delete set_;
			delete get_;
		}

		ElementValueFinfo( const string& name, const string& doc, 
			void ( T::*setFunc )( const Eref&, const Qinfo*, F ),
			F ( T::*getFunc )( const Eref&, const Qinfo* ) const )
			: Finfo( name, doc )
		{
				string setname = "set_" + name;
				set_ = new DestFinfo(
					setname,
					"Assigns field value.",
					new EpFunc1< T, F >( setFunc ) );

				string getname = "get_" + name;
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetEpFunc< T, F >( getFunc ) );
		}


		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( set_ );
			c->registerFinfo( get_ );
		}

		bool strSet( const Eref& tgt, const string& field, 
			const string& arg ) const {
			Field< F > sg( tgt );
			return sg.innerStrSet( tgt, field, arg );
		}

		bool strGet( const Eref& tgt, const string& field, 
			string& returnValue ) const {
			Field< F > sg( tgt );
			return sg.innerStrGet( tgt, field, returnValue );
		}

	private:
		DestFinfo* set_;
		DestFinfo* get_;
};

template < class T, class F > class ReadOnlyElementValueFinfo: public Finfo
{
	public:
		~ReadOnlyElementValueFinfo() {
			delete get_;
		}

		ReadOnlyElementValueFinfo( const string& name, const string& doc, 
			F ( T::*getFunc )( const Eref& e, const Qinfo* q ) const )
			: Finfo( name, doc )
		{
				string getname = "get_" + name;
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetEpFunc< T, F >( getFunc ) );
		}


		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( get_ );
		}

		bool strSet( const Eref& tgt, const string& field, 
			const string& arg ) const {
			return 0;
		}

		bool strGet( const Eref& tgt, const string& field, 
			string& returnValue ) const {
			Field< F > sg( tgt );
			return sg.innerStrGet( tgt, field, returnValue );
		}

	private:
		DestFinfo* get_;
};

#endif // _ELEMENT_VALUE_FINFO_H
