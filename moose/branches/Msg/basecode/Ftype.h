/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class Ftype
{
	public:
		virtual ~Ftype()
		{;}
		virtual bool checkSlot( const Slot* s) const = 0;
		virtual void op( Eref e, const char* buf ) const = 0;
};

template< class T, class A > class Ftype1: public Ftype
{
	public:
		Ftype1( void ( T::*func )( A ) )
			: func_( func )
			{;}

		bool checkSlot( const Slot* s ) const {
			return dynamic_cast< const Slot1< A >* >( s );
		}

		void op( Eref e, const char* buf ) const {
			(static_cast< T* >( e.data() )->*func_)( 
				*reinterpret_cast< const A* >( buf )
			);
		}

	private:
		void ( T::*func_ )( A ); 
};

template< class T, class A1, class A2 > class Ftype2: public Ftype
{
	public:
		Ftype2( void ( T::*func )( A1, A2 ) )
			: func_( func )
			{;}

		bool checkSlot( const Slot* s ) const {
			return dynamic_cast< const Slot2< A1, A2 >* >( s );
		}

		void op( Eref e, const char* buf ) const {
			const char* buf2 = buf + sizeof( A1 );
			(static_cast< T* >( e.data() )->*func_)( 
				*reinterpret_cast< const A1* >( buf ),
				*reinterpret_cast< const A2* >( buf2 )
			);
		}

	private:
		void ( T::*func_ )( A1, A2 ); 
};

template< class T, class A1, class A2, class A3 > class Ftype3: 
	public Ftype
{
	public:
		Ftype3( void ( T::*func )( A1, A2, A3 ) )
			: func_( func )
			{;}

		bool checkSlot( const Slot* s ) const {
			return dynamic_cast< const Slot3< A1, A2, A3 >* >( s );
		}

		void op( Eref e, const char* buf ) const {
			const char* buf2 = buf + sizeof( A1 );
			const char* buf3 = buf2 + sizeof( A2 );
			(static_cast< T* >( e.data() )->*func_)( 
				*reinterpret_cast< const A1* >( buf ),
				*reinterpret_cast< const A2* >( buf2 ),
				*reinterpret_cast< const A3* >( buf3 )
			);
		}

	private:
		void ( T::*func_ )( A1, A2, A3 ); 
};
