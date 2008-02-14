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

class SimpleConn: public Conn
{
	public:
		SimpleConn( SimpleConnTainer* s )
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
			return s_->eI1;
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
		 * increment() updates internal counter, used in iterating through
		 * targets.
		 */
		void operator++() {
			s_ = 0;
		}
		bool good() const {
			return s_ != 0;
		}

		/**
		 * Returns a Conn with e1 and e2 flipped so that return messages
		 * traverse back with the correct args.
		 */
		const Conn* flip() const {
			return new ReverseSimpleConn( s_ );
		}

	private:
		SimpleConnTainer* s_;
};

class ReverseSimpleConn: public Conn
{
	public:
		ReverseSimpleConn( SimpleConnTainer* s )
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
		void increment() {
			s_ = 0;
		}
		bool good() {
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
		SimpleContainer* a_;
};

class SimpleConnTainer: public ConnTainer
{
	public:
		SimpleConnTainer( Element* e1, Element* e2, 
			unsigned int msg1, unsigned int msg2,
			unsigned int eI1 = 0, unsigned int eI2 = 0,
			unsigned int i1 = 0, unsigned int i2 = 0 )
			: 
			ConnTainer( e1, e2, msg1, msg2 ),
			eI1_( eI1 ), eI2_( eI2 ),
			i1_( i1 ), i2_( i2 )
		{;}

		Conn* conn( unsigned int eIndex, bool isReverse ) {
			numIter_++; // For reference counting. Do we need it?
			if ( isReverse )
				return new ReverseSimpleConn( this, eIndex );
			else
				return new SimpleConn( this, eIndex );
		}

		bool add( Element* e1, Element* e2 ) {
			SimpleConnTainer ce( e1, e2 );
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

		bool copy( Element* e1, Element* e2 ) {
			Msg::add( e1, e2, msg1(), msg2() );
		}
		
	private:
		unsigned int eI1_;
		unsigned int eI2_;
		unsigned int i1_;
		unsigned int i2_;
};

#endif // _SIMPLE_CONN_H
