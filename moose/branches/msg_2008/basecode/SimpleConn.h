/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SIMPLE_CONN_H
#define _SIMPLE_CONN_H


class SimpleConnTainer: public ConnTainer
{
	public:
		SimpleConnTainer( Element* e1, Element* e2, 
			unsigned int msg1, unsigned int msg2,
			unsigned int eI1 = 0, unsigned int eI2 = 0,
			unsigned int i1 = 0, unsigned int i2 = 0 );

		Conn* conn( unsigned int eIndex, bool isReverse ) const;
		Conn* conn( unsigned int eIndex, bool isReverse,
			unsigned int connIndex ) const;

		bool add( Element* e1, Element* e2 );

		unsigned int size() const {
			return 1;
		}

		unsigned int eI1() const {
			return eI1_;
		}

		unsigned int eI2() const {
			return eI2_;
		}

		unsigned int i1() const {
			return i1_;
		}

		unsigned int i2() const {
			return i2_;
		}

		bool copy( Element* e1, Element* e2 ) const;
		
	private:
		unsigned int eI1_;
		unsigned int eI2_;
		unsigned int i1_;
		unsigned int i2_;
};

class SimpleConn: public Conn
{
	public:
		SimpleConn( const SimpleConnTainer* s )
			: s_( s )
		{;}

		~SimpleConn()
		{;}

		Element* targetElement() const {
			return s_->e2();
		}
		unsigned int targetEindex() const {
			return s_->eI2();
		}
		unsigned int targetIndex() const {
			return s_->i2();
		}
		unsigned int targetMsg() const {
			return s_->msg2();
		}
		Element* sourceElement() const {
			return s_->e1();
		}
		unsigned int sourceEindex() const {
			return s_->eI1();
		}
		unsigned int sourceIndex() const {
			return s_->i1();
		}
		unsigned int sourceMsg() const {
			return s_->msg1();
		}
		void* data() const {
			return s_->e2()->data( s_->eI2() );
		}

		/**
		 * operator++() updates internal counter, used in iterating through
		 * targets. Since we have a single entry in the SimpleConn, all
		 * this has to do is to invalidate further good() calls.
		 */
		void operator++() {
			s_ = 0;
		}
		bool good() const {
			return ( s_ != 0 );
		}

		/**
		 * Returns a Conn with e1 and e2 flipped so that return messages
		 * traverse back with the correct args.
		 */
		const Conn* flip() const;

	private:
		const SimpleConnTainer* s_;
};

class ReverseSimpleConn: public Conn
{
	public:
		ReverseSimpleConn( const SimpleConnTainer* s )
			: s_( s ) 
		{;}

		~ReverseSimpleConn()
		{;}

		Element* targetElement() const {
			return s_->e1();
		}
		unsigned int targetEindex() const {
			return s_->eI1();
		}
		unsigned int targetIndex() const {
			return s_->i1();
		}
		unsigned int targetMsg() const {
			return s_->msg1();
		}
		Element* sourceElement() const {
			return s_->e2();
		}
		unsigned int sourceEindex() const {
			return s_->eI2();
		}
		unsigned int sourceIndex() const {
			return s_->i2();
		}
		unsigned int sourceMsg() const {
			return s_->msg2();
		}
		void* data() const {
			return s_->e1()->data( s_->eI1() );
		}

		/**
		 * increment() updates internal counter, used in iterating through
		 * targets.
		 */
		void operator++() {
			s_ = 0;
		}

		bool good() const {
			return ( s_ != 0 );
		}

		/**
		 * Returns a Conn with e1 and e2 flipped so that return messages
		 * traverse back with the correct args.
		 */
		const Conn* flip() const {
			return new SimpleConn( s_ );
		}

	private:
		const SimpleConnTainer* s_;
};

#endif // _SIMPLE_CONN_H
