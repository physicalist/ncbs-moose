/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MSG_H
#define _MSG_H

/**
 * The MsgSrc class specifies the ranges of conns to be used for
 * messaging, and specifies which functions to be used within the
 * range. In case there are additional ranges with different funcs
 * but the same src, we use the next_ index to point to a location
 * further up in the array, past the predefined src range.
 * Used for the source of simple messages, and for either end of 
 * bidirectional shared messages.
 */
class Msg
{
	public:
		Msg();
		/**
		 * Destructor needs to clean up all connections
		 */
		~Msg();

		/**
		 * Begin const_iterator for the ConnTainers on this Msg.
		 * Used in most iterations through the ConnTainers.
		 */
		vector< ConnTainer* >::const_iterator begin() const {
			return c_.begin();
		}
		/**
		 * End const_iterator for the ConnTainers on this Msg.
		 * Used in most iterations through the ConnTainers.
		 */
		vector< ConnTainer* >::const_iterator end() const {
			return c_.end();
		}

		/**
		 * Begin iterator for the ConnTainers on this Msg. This
		 * is used for those rare cases where the user wants to mess
		 * with the ConnTainers.
		 */
		vector< ConnTainer* >::iterator varBegin() {
			return c_.begin();
		}
		/**
		 * End iterator for the ConnTainers on this Msg. This
		 * is used for those rare cases where the user wants to mess
		 * with the ConnTainers.
		 */
		vector< ConnTainer* >::iterator varEnd() {
			return c_.end();
		}

		/**
		 * Size of the ConnTainer vector
		 * \todo: Should we include the 'next' size here too?
		 */
		unsigned int size() const;

		/**
		 * Iterator to the conn selected by the 'tgt' index.
		 */
		Conn* findConn( unsigned int eIndex, unsigned int tgt ) const;

		/**
		 * Add a new message from e1 (source ) to e2 (dest).
		 * The m1 and m2 indicate source and dest msg indices.
		 * The funcId1 is the source funcId, which is going to be used
		 * at the dest, but is optional so it may be zero.
		 * The funcId2 is the dest funcId, which must be nonzero and will
		 * be used when the source calls the dest.
		 * Later I may relax the directional restrictions.
		 *
		 * Returns the newly created ConnTainer pointer. Note that the
		 * ConnTainer type depends on the type of the elements and
		 * possibly also on additional info, yet to be determined.
		 * For example, a message between SimpleElements will use a
		 * SimpleConnTainer.
		 * A message between ArrayElements may use SparseConnTainer or
		 * One2OneConnTainer, or various others.
		 */
		static ConnTainer* add( Element* e1, Element* e2, 
			unsigned int m1, unsigned int m2,
			unsigned int funcId1, unsigned int funcId2 );

		// This is invoked by the remote Msg.
		// It is also used in Copy.
		bool drop( const ConnTainer* doomed );

		/**
		 * This variant of drop initiates the removal of a specific 
		 * local ConnTainer, identified by index
		 */
		bool drop( unsigned int doomed );

		/**
		 * Drops all messages emanating from this Msg. Typically a prelude
		 * to deleting self.
		 */
		void dropAll();

		/**
		 * Utility function for putting a new ConnTainer onto a msg as
		 * specified by the funcId. If necessary it traverses through the
		 * 'next' message looking for a funcId match,  and allocates a
		 * new 'next' message for the FuncId if there are no matches.
		 */
		void assignMsgByFuncId( 
			Element* e, unsigned int funcId, ConnTainer* ct );

		/**
		 * Returns the function identified by funcNum.
		 */
		RecvFunc func( unsigned int funcNum ) const {
			return fv_->func( funcNum );
		}


		/**
		 * True if this is the destination of a message.
		 * Undefined if the message is empty: Check for size first.
		 * The definition of message source and dest is done at Finfo
		 * setup time. For simple messages no problem. For Shared Finfos,
		 * the one that has the first 'source' entry is the source.
		 */
		bool isDest() const;

		/**
		 * Returns the ptr to the next Msg in the list.
		 */
		const Msg* next( const Element* e ) const;

		/**
		 * Returns the Id of the FuncVec
		 */
		unsigned int funcId() const {
			return fv_->id();
		}

		/**
		 * Lists out all the target Elements with their indices.
		 * Shouldn't this be a vector of ids?
		 * Clears out the list first.
		 */
		unsigned int targets(
			vector< pair< Element*, unsigned int> >& list,
			unsigned int myEindex = 0
		) const;

		/**
		 * Counts the number of targets, including going through the
		 * 'next_' msg if any. May be much faster than listing.
		 */
		unsigned int numTargets( const Element* e ) const;

		/**
		 * Makes a duplicate of the current message specified by c,
		 * to now go between e1 and e2.
		 */
		bool copy( const ConnTainer* c, Element* e1, Element* e2 ) const;

	private:
		/**
		 * This manages the ConnTainers.
		 * Each ConnTainer handles a virtual vector of Conns. The ConnTainer provides a Conn as an iterator.
		 */
		vector< ConnTainer* > c_; 

		/**
		 * Points to a global FuncList.
		 */
		const FuncVec *fv_; 

		/**
		 * Could use index or ptr here. need to sort it out.
		 */
		unsigned int next_; 
};

#endif // _MSG_H
