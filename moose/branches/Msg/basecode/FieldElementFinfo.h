/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This class sets up fields that should be accessed as independent
 * Elements. These fields are typically complex ones with several sub-
 * fields, and are also typically arranged in an array.
 * Examples are the Ticks for Clock objects, and Synapses on an
 * IntFire Neuron.
 *
 * The creation of this one Finfo sets up three things: 
 * First, the information for the FieldElement that will handle the class.
 * Second, the set and get functions for the size of the array of Fields.
 * Third, an automatic creation of the FieldElement whenever an Element
 * of the Parent class is made. This defaults to a child, but can be moved.
 */

#ifndef _FIELD_ELEMENT_FINFO_H
#define _FIELD_ELEMENT_FINFO_H

template < class T, class F > class FieldElementFinfo: public Finfo
{
	public:

		FieldElementFinfo( 
			const string& name, 
			const string& doc, 
			const Cinfo* fieldCinfo,
			F* ( T::*lookupField )( unsigned int ),
			void( T::*setNumField )( unsigned int num ),
			unsigned int ( T::*getNumField )() const
		)
			: 	Finfo( name, doc), 
				fieldCinfo_( fieldCinfo ),
				lookupField_( lookupField ),
				setNumField_( setNumField ),
				getNumField_( getNumField )
		{
				string setname = "set_num_" + name;
				setNum_ = new DestFinfo(
					setname,
					"Assigns number of field entries in field array.",
					new OpFunc1< T, unsigned int >( setNumField ) );

				string getname = "get_num_" + name;
				getNum_ = new DestFinfo(
					getname,
					"Requests number of field entries in field array."
					"The requesting Element must "
					"provide a handler for the returned value.",
					new GetOpFunc< T, unsigned int >( getNumField ) );
		}

		~FieldElementFinfo() {
			delete setNum_;
			delete getNum_;
		}

		/**
		 * Virtual function. Later will need to add stuff to tie the
		 * child element to the parent.
		 */
		void postCreationFunc( Id parent, Element* parentElm ) const
		{
			// static Dinfo< F > fieldDinfo;
			Id kid = Id::nextId();
			new Element(
				kid, fieldCinfo_, name(), 
				new FieldDataHandler< T, F >(
					fieldCinfo_->dinfo(),
					// &fieldDinfo,
					parentElm->dataHandler(),
					lookupField_, getNumField_, setNumField_ )
			);
		}

		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( setNum_ );
			c->registerFinfo( getNum_ );
			c->registerPostCreationFinfo( this );
		}

	private:
		DestFinfo* setNum_;
		DestFinfo* getNum_;
		const Cinfo* fieldCinfo_;
		F* ( T::*lookupField_ )( unsigned int );
		void( T::*setNumField_ )( unsigned int num );
		unsigned int ( T::*getNumField_ )() const;
		
	//	OpFunc1< T, F >* setOpFunc_;
	//	GetOpFunc< T, F >* getOpFunc_;
};


#endif // _FIELD_ELEMENT_FINFO_H
