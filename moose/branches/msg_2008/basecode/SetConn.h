/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SET_CONN_H
#define _SET_CONN_H

/**
 * This Conn is used as a handle when doing assignments, ie., the set
 * command. All it really needs to know are the target Element and eIndex.
 */
class SetConn: public Conn
{
	public:
		SetConn( Element* e, unsigned int eIndex )
			: e_( e ), eIndex_( eIndex )
		{;}

		~SetConn()
		{;}

		Element* targetElement() const {
			return e_;
		}
		unsigned int targetEindex() const {
			return eIndex_;
		}
		unsigned int targetIndex() const {
			return 0;
		}
		unsigned int targetMsg() const {
			return 0;
		}
		Element* sourceElement() const {
			return e_;
		}
		unsigned int sourceEindex() const {
			return 0;
		}
		unsigned int sourceIndex() const {
			return 0;
		}
		unsigned int sourceMsg() const {
			return 0;
		}
		void* data() const {
			return e_->data( eIndex_ );
		}

		/**
		 * increment() updates internal counter, used in iterating through
		 * targets.
		 */
		void operator++() {
			;
		}
		bool good() const {
			return 0;
		}

		/**
		 * Returns a Conn with e1 and e2 flipped so that return messages
		 * traverse back with the correct args.
		 */
		const Conn* flip() const {
			return new SetConn( *this );
		}

	private:
		Element* e_;
		unsigned int eIndex_;
};

#endif // _SET_CONN_H
