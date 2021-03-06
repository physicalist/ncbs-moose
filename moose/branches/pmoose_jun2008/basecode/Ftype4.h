/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _FTYPE4_H
#define _FTYPE4_H

template < class T1, class T2, class T3, class T4 > 
	void send4( Eref e, Slot src, T1 v1, T2 v2, T3 v3, T4 v4 );

/**
 * The Ftype4 handles 4-argument functions.
 */
template < class T1, class T2, class T3, class T4 > 
	class Ftype4: public Ftype
{
		public:
			Ftype4()
				: Ftype( "ftype4" )
			{
				addSyncFunc( RFCAST( 
					&( Ftype4< T1, T2, T3, T4 >::syncFunc ) ) );
				addAsyncFunc( RFCAST( 
					&( Ftype4< T1, T2, T3, T4 >::asyncFunc ) ) );
				addProxyFunc( RFCAST( 
					&( Ftype4< T1, T2, T3, T4 >::proxyFunc ) ) );
			}

			unsigned int nValues() const {
				return 4;
			}
			
			bool isSameType( const Ftype* other ) const {
				return ( dynamic_cast< const Ftype4< T1, T2, T3, T4 >* >( other ) != 0 );
			}
			
			static bool isA( const Ftype* other ) {
				return ( dynamic_cast< const Ftype4< T1, T2, T3, T4 >* >( other ) != 0 );
			}

			size_t size() const
			{
				return ( sizeof( T1 ) + sizeof( T2 ) + 
					sizeof( T3 ) + sizeof( T4 ) );
			}

			/**
			 * This variant works for most kinds of Finfo,
			 * so we make a default version.
			 * In a few cases we need to specialize it because
			 * we need extra info from the Finfo.
			 * This function makes the strong assumption that
			 * we will NOT look up any dynamic finfo, and will
			 * NOT use the conn index in any way. This would
			 * fail because the conn index given is a dummy one.
			 * We do a few assert tests in downstream functions
			 * for any such attempts
			 * to search for a Finfo based on the index.
			 */
			virtual bool set(
			Eref e, const Finfo* f, T1 v1, T2 v2, T3 v3, T4 v4) const
			{

				void (*set)( const Conn*, T1 v1, T2 v2, T3 v3, T4 v4 ) =
					reinterpret_cast< 
						void (*)( const Conn*, T1 v1, T2 v2, T3 v3, T4 v4 )
					>(
									f->recvFunc()
					);
				SetConn c( e );
				set( &c, v1, v2, v3, v4 );
				return 1;
			}

			static const Ftype* global() {
				static Ftype* ret = new Ftype4< T1, T2, T3, T4 >();
				return ret;
			}

			RecvFunc recvFunc() const {
				return 0;
			}

			RecvFunc trigFunc() const {
				return 0;
			}
virtual std::string getTemplateParameters() const
    {
        static std::string s = 
			Ftype::full_type(std::string(typeid(T1).name())) +"," +
			Ftype::full_type(std::string(typeid(T2).name()))+"," +
			Ftype::full_type(std::string (typeid(T3).name())) + "," +
			Ftype::full_type( std::string (typeid( T4 ).name () ) );
        cout << "Ftype4::getTemplateParameters() - " << s << endl;        
        return s;
    }
			///////////////////////////////////////////////////////
			// Here we define the functions for handling 
			// messages of this type for parallel messaging.
			///////////////////////////////////////////////////////
			/**
			 * This function extracts the value for this field from
			 * the data, and executes the function call for its
			 * target Conn. It returns the data pointer set to the
			 * next field.
			 */
			static void proxyFunc(
				const Conn* c, const void* data, Slot slot )
			{
				T1 v1;
				T2 v2;
				T3 v3;
				T4 v4;
				data = Serializer< T1 >::unserialize( v1, data );
				data = Serializer< T2 >::unserialize( v2, data );
				data = Serializer< T3 >::unserialize( v3, data );
				data = Serializer< T4 >::unserialize( v4, data );
				send4< T1, T2, T3, T4 >( c->target(), slot, v1, v2, v3, v4);
			}

			/**
			 * This function inserts data into the outgoing buffer.
			 * This variant is used when the data is synchronous: sent
			 * every clock step, so that the sequence is fixed.
			 */
			static void syncFunc( const Conn* c, T1 v1, T2 v2, T3 v3,
				T4 v4 ) {
				unsigned int size1 = Serializer< T1 >::serialSize( v1 );
				unsigned int size2 = Serializer< T2 >::serialSize( v2 );
				unsigned int size3 = Serializer< T3 >::serialSize( v3 );
				unsigned int size4 = Serializer< T4 >::serialSize( v4 );
				void* data = getParBuf( c, size1 + size2 + size3 + size4 ); 
				data = Serializer< T1 >::serialize( data, v1 );
				data = Serializer< T2 >::serialize( data, v2 );
				data = Serializer< T3 >::serialize( data, v3 );
				Serializer< T4 >::serialize( data, v4 );
			}

			/**
			 * This variant is used for asynchronous data, where data
			 * is sent in at unpredictable stages of the simulation. It
			 * therefore adds additional data to identify the message
			 * source
			 */
			static void asyncFunc( const Conn* c, T1 v1, T2 v2, T3 v3, T4 v4 ){
				unsigned int size1 = Serializer< T1 >::serialSize( v1 );
				unsigned int size2 = Serializer< T2 >::serialSize( v2 );
				unsigned int size3 = Serializer< T3 >::serialSize( v3 );
				unsigned int size4 = Serializer< T4 >::serialSize( v4 );
				void* data = getAsyncParBuf( c, size1 + size2 + size3 + size4 ); 
				data = Serializer< T1 >::serialize( data, v1 );
				data = Serializer< T2 >::serialize( data, v2 );
				data = Serializer< T3 >::serialize( data, v3 );
				Serializer< T4 >::serialize( data, v4 );
			}
};

#endif // _FTYPE4_H
