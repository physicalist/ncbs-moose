/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _FIELD_DATA_HANDLER_H
#define _FIELD_DATA_HANDLER_H

/**
 * This class manages access to array fields X in an array of objects Y.
 * Examples are synapses and clock ticks.
 * Replaces FieldElement.h
 * It is templated by the field type, the parent type and a lookup function * that extracts the field from the parent.  */

template< class Parent, class Field > class FieldDataHandler: public FieldDataHandlerBase
{
	public:
		FieldDataHandler( const DinfoBase* dinfo,
			const DataHandler* parentDataHandler,
			Field* ( Parent::*lookupField )( unsigned int ),
			unsigned int ( Parent::*getNumField )() const,
			void ( Parent::*setNumField )( unsigned int num ) )
			: 
				FieldDataHandlerBase( dinfo , parentDataHandler ),
				lookupField_( lookupField ),
				getNumField_( getNumField ),
				setNumField_( setNumField )
		{;}

		~FieldDataHandler()
		{;} // Don't delete data because the parent Element should do so.


		/**
		 * Makes a copy of the FieldDataHandler. Doesn't care what the
		 * parent dimesion was, as it just refers everything to the parent
		 * anyway.
		 * Needs post-processing to substitute in the new parent.
		 */
		DataHandler* copy( bool toGlobal ) const
		{
			FieldDataHandler< Parent, Field >* ret =
				new FieldDataHandler< Parent, Field >( *this );
			return ret;
		}

		/**
		 * The FieldDataHandler doesn't care what the parent dim was, as
		 * it always refers back to the parent for the actual data access
		 * functions. So expanding dimensions doesn't need anything other
		 * than a copy of the original FieldDataHandler.
		 * Needs post-processing to substitute in the new parent.
		 */
		DataHandler* copyToNewDim( unsigned int newDim, bool toGlobal ) const
		{
			FieldDataHandler< Parent, Field >* ret =
				new FieldDataHandler< Parent, Field >( *this );
			return ret;
		}

		/**
		 * I'm dubious about this one too, because I don't see how the
		 * original lookup and set/get functions could work on a different
		 * dinfo
		 */
		DataHandler* copyUsingNewDinfo( const DinfoBase* dinfo ) const
		{
			FieldDataHandler< Parent, Field >* ret =
				new FieldDataHandler< Parent, Field >(
					dinfo,
					parentDataHandler(),
					lookupField_,
					getNumField_,
					setNumField_
				);
			return ret;
		}

		/**
		 * Returns the pointer to the field entry at fieldIndex, on the
		 * parent data entry at data.
		 */
		char* lookupField( char* data, unsigned int fieldIndex ) const
		{
			if ( data ) {
				Parent* pa = reinterpret_cast< Parent* >( data );
				Field* s = ( pa->*lookupField_ )( fieldIndex );
				return reinterpret_cast< char* >( s );
			}
			return 0;
		}

		/**
		 * Assigns the number of field entries on parent data entry.
		 */
		void setNumField( char* data, unsigned int size )
		{
			if ( data ) {
				Parent* pa = reinterpret_cast< Parent* >( data );
				( pa->*setNumField_ )( size );
				if ( size > getFieldDimension() )
					setFieldDimension( size );
			}
		}

		/**
		 * Returns the number of field entries on parent data entry.
		 */
		unsigned int getNumField( const char* data ) const
		{
			if ( data ) {
				const Parent* pa = reinterpret_cast< const Parent* >( data );
				return ( pa->*getNumField_ )();
			}
			return 0;
		}


		/////////////////////////////////////////////////////////////////
		// setDataBlock stuff. Defer implementation for now.
		/////////////////////////////////////////////////////////////////

			/*
		bool setDataBlock( const char* data, unsigned int numData,
			DataId startIndex ) const 
		{
			if ( parentDataHandler_->isDataHere( startIndex.data() ) ) {
				char* temp = parentDataHandler_->data( startIndex.data() );
				assert( temp );
				Parent* pa = reinterpret_cast< Parent* >( temp );

				unsigned int numField = ( pa->*getNumField_ )();
				unsigned int max = numData;
				if ( did.field() + numData > numField  )
					max = numField - did.field();
				for ( unsigned int i = 0; i < max; ++i ) {
					Field* f = ( pa->*lookupField_ )( did.field() + i );
					*f = *reinterpret_cast< const Field* >( 
						data + i * dinfo()->size() );
				}
			}
			return 1;
			return 0;
		}
			*/

		/**
		 * Assigns a block of data at the specified location.
		 * Returns true if all OK. No allocation.
		 */
		/*
		bool setDataBlock( const char* data, unsigned int numData,
			const vector< unsigned int >& startIndex ) const
		{
			if ( startIndex.size() == 0 )
				return setDataBlock( data, numData, 0 );
			unsigned int fieldIndex = startIndex[0];
			if ( startIndex.size() == 1 )
				return setDataBlock( data, numData, DataId( 0, fieldIndex ) );
			vector< unsigned int > temp = startIndex;
			temp.pop_back(); // Get rid of fieldIndex.
			DataDimensions dd( parentDataHandler_->dims() );
			unsigned int paIndex = dd.linearIndex( temp );
			return setDataBlock( data, numData, DataId( paIndex, fieldIndex ) );
			return 0;
		}
			*/

	private:
		Field* ( Parent::*lookupField_ )( unsigned int );
		unsigned int ( Parent::*getNumField_ )() const;
		void ( Parent::*setNumField_ )( unsigned int num );
};

#endif	// _FIELD_DATA_HANDLER_H

