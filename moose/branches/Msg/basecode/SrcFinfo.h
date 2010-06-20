/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _SRC_FINFO_H
#define _SRC_FINFO_H

/**
 * This set of classes define Message Sources. Their main job is to supply 
 * a type-safe send operation, and to provide typechecking for it.
 */

class SrcFinfo: public Finfo
{
	public:
		SrcFinfo( const string& name, const string& doc ); 

		~SrcFinfo() {;}

		void registerFinfo( Cinfo* c );

		bool strSet( const Eref& tgt, const string& field, 
			const string& arg ) const {
			return 0; // always fails
		}

		bool strGet( const Eref& tgt, const string& field, 
			string& returnValue ) const {
			return 0; // always fails
		}

		BindIndex getBindIndex() const {
			return bindIndex_;
		}

		/**
		 * Checks that the target will work for this Msg.
		 */
		bool checkTarget( const Finfo* target ) const;

		/**
		 * First checks that the target will work, then adds the Msg.
		 */
		bool addMsg( const Finfo* target, MsgId mid, Element* src ) const;

	private:
		/**
		 * Index into the msgBinding_ vector.
		 */
		unsigned short bindIndex_;
};

/**
 * SrcFinfo0 sets up calls without any arguments.
 */
class SrcFinfo0: public SrcFinfo
{
	public:

		SrcFinfo0( const string& name, const string& doc );
		~SrcFinfo0() {;}
		
		// Will need to specialize for strings etc.
		void send( Eref e, const ProcInfo* p ) const;
		void sendTo( Eref e, const ProcInfo* p, const FullId& target) const;

	private:
};


template < class T > class SrcFinfo1: public SrcFinfo
{
	public:
		~SrcFinfo1() {;}

		SrcFinfo1( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( Eref e, const ProcInfo* p, const T& arg ) const 
		{
			// Qinfo( eindex, size, useSendTo );
			Qinfo q( e.index(), sizeof( T ), 0 );
			e.element()->asend( q, getBindIndex(), p, 
				reinterpret_cast< const char* >( &arg ) );
		}

		/**
		 * We know the data index but we also need to know the target 
		 * Element or Msg, since there could be multiple ones. 
		 */
		void sendTo( Eref e, const ProcInfo* p,
			const T& arg, const FullId& target ) const
		{
			// Qinfo( eindex, size, useSendTo );
			Qinfo q( e.index(), sizeof( T ), 1 );
			e.element()->tsend( q, getBindIndex(), p, 
				reinterpret_cast< const char* >( &arg ), target );
		}

	private:
};

template <> class SrcFinfo1< string >: public SrcFinfo
{
	public:
		~SrcFinfo1() {;}

		SrcFinfo1( const string& name, const string& doc ) 
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( Eref e, const ProcInfo* p, const string& arg ) const
		{
			Conv< string > s( arg );

			// Qinfo( eindex, size, useSendTo );
			Qinfo q( e.index(), s.size(), 0 );
			char* buf = new char[ s.size() ];
			s.val2buf( buf );

			e.element()->asend( q, getBindIndex(), p, buf );
			delete[] buf;
		}

		void sendTo( Eref e, const ProcInfo* p, 
			const string& arg, const FullId& target ) const
		{
			Conv< string > s( arg );
			Qinfo q( e.index(), s.size(), 1 );
			char* buf = new char[ s.size() ];
			s.val2buf( buf );

			e.element()->tsend( q, getBindIndex(), p, buf, target );
			delete[] buf;
		}

	private:
};

template < class T1, class T2 > class SrcFinfo2: public SrcFinfo
{
	public:
		~SrcFinfo2() {;}

		SrcFinfo2( const string& name, const string& doc ) 
			: SrcFinfo( name, doc )
			{ ; }

		// This version is general but inefficient as it uses an extra
		// memcpy in val2buf.
		void send( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2 ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Qinfo q( e.index(), a1.size() + a2.size(), 0 );
			char* temp = new char[ a1.size() + a2.size() ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			e.element()->asend( q, getBindIndex(), p, temp );
			delete[] temp;
		}

		void sendTo( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2, const FullId& target ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Qinfo q( e.index(), a1.size() + a2.size(), 1 );
			char* temp = new char[ a1.size() + a2.size() ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			e.element()->tsend( q, getBindIndex(), p, temp, target );
			delete[] temp;
		}

	private:
};


template < class T1, class T2, class T3 > class SrcFinfo3: public SrcFinfo
{
	public:
		~SrcFinfo3() {;}

		SrcFinfo3( const string& name, const string& doc ) 
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2, const T3& arg3 ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Conv< T3 > a3( arg3 );
			Qinfo q( e.index(), a1.size() + a2.size() + a3.size(), 0 );
			char* temp = new char[ a1.size() + a2.size() + a3.size() ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			a3.val2buf( temp + a1.size() + a2.size() );
			e.element()->asend( q, getBindIndex(), p, temp );
			delete[] temp;
		}

		void sendTo( Eref e, DataId target, const ProcInfo* p,
			const T1& arg1, const T2& arg2, const T3& arg3 ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Conv< T3 > a3( arg3 );
			Qinfo q( e.index(), a1.size() + a2.size() + a3.size(), 1 );
			char* temp = new char[ a1.size() + a2.size() + a3.size() ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			a3.val2buf( temp + a1.size() + a2.size() );
			e.element()->tsend( q, getBindIndex(), p, temp, target );
			delete[] temp;
		}

	private:
};

template < class T1, class T2, class T3, class T4 > class SrcFinfo4: public SrcFinfo
{
	public:
		~SrcFinfo4() {;}

		SrcFinfo4( const string& name, const string& doc ) 
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2, 
			const T3& arg3, const T4& arg4 ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Conv< T3 > a3( arg3 );
			Conv< T4 > a4( arg4 );
			Qinfo q( e.index(), 
				a1.size() + a2.size() + a3.size() + a4.size(), 0 );
			char* temp = new char[ a1.size() + a2.size() + a3.size() + a4.size() ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			a3.val2buf( temp + a1.size() + a2.size() );
			a4.val2buf( temp + a1.size() + a2.size() + a3.size() );
			e.element()->asend( q, getBindIndex(), p, temp );
			delete[] temp;
		}

		void sendTo( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
			const FullId& target ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Conv< T3 > a3( arg3 );
			Conv< T4 > a4( arg4 );
			Qinfo q( e.index(), 
				a1.size() + a2.size() + a3.size() + a4.size(), 1 );
			char* temp = new char[ a1.size() + a2.size() + a3.size() + a4.size() ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			a3.val2buf( temp + a1.size() + a2.size() );
			a4.val2buf( temp + a1.size() + a2.size() + a3.size() );
			e.element()->tsend( q, getBindIndex(), p, temp, target );
			delete[] temp;
		}

	private:
};

template < class T1, class T2, class T3, class T4, class T5 > class SrcFinfo5: public SrcFinfo
{
	public:
		~SrcFinfo5() {;}

		SrcFinfo5( const string& name, const string& doc ) 
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
			const T5& arg5 ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Conv< T3 > a3( arg3 );
			Conv< T4 > a4( arg4 );
			Conv< T5 > a5( arg5 );
			unsigned int totSize = 
				a1.size() + a2.size() + a3.size() + a4.size() + a5.size();
			Qinfo q( e.index(), totSize, 0 );
			char* temp = new char[ totSize ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			a3.val2buf( temp + a1.size() + a2.size() );
			a4.val2buf( temp + a1.size() + a2.size() + a3.size() );
			a5.val2buf( temp + a1.size() + a2.size() + a3.size() + a4.size() );
			e.element()->asend( q, getBindIndex(), p, temp );
			delete[] temp;
		}

		void sendTo( Eref e, const ProcInfo* p,
			const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
			const T5& arg5,
			const FullId& target ) const
		{
			Conv< T1 > a1( arg1 );
			Conv< T2 > a2( arg2 );
			Conv< T3 > a3( arg3 );
			Conv< T4 > a4( arg4 );
			Conv< T5 > a5( arg5 );
			unsigned int totSize = 
				a1.size() + a2.size() + a3.size() + a4.size() + a5.size();
			Qinfo q( e.index(), totSize, 1 );
			char* temp = new char[ totSize ];
			a1.val2buf( temp );
			a2.val2buf( temp + a1.size() );
			a3.val2buf( temp + a1.size() + a2.size() );
			a4.val2buf( temp + a1.size() + a2.size() + a3.size() );
			a5.val2buf( temp + a1.size() + a2.size() + a3.size() + a4.size() );
			e.element()->tsend( q, getBindIndex(), p, temp, target );
			delete[] temp;
		}

	private:
};

#endif // _SRC_FINFO_H
