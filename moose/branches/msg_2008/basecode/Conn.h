/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _CONN_H
#define _CONN_H
class Element;

/**
 * This definition is used to indicate that a conn is a dummy one.
 */
#define MAXUINT (unsigned int)(~0)

/**
 * This class handles connections. Connections are the underlying
 * linkages upon which messages run: they are like wires, bidirectional.
 * Mutiple messages can run on any given wire.
 * An array of Conns is present in each SimpleElement, 
 * to enable bidirectional traversal of messages.
 *
 * The Conn structure has two design requirements: 
 * First, it must provide complete traversability in either direction.
 * This is achieved because we can identify the remote Conn with the
 * combination of Element*, target index, and index of MsgSrc/MsgDest
 * provided by the ConnInfo.
 * Second, it must provide complete information for the RecvFunc
 * that handles message operations on the destination object. 
 * Most RecvFuncs operate only on the Element*, but some need to know
 * their own index (which is the target index). Some need to go back
 * to the originating object, which involves looking up the matching
 * Conn on the target Element.
 *
 * \todo We need to provide a range of functions to handle changing
 * of target Elements, or reindexing following changes in the target
 * array of Conns.
 */
class Conn
{
		public:
			Conn()
					: e_( 0 ), eIndex_( 0 ), index_( 0 ),
						info_( getSimpleConnInfo( 0 ) )
			{;}

			Conn( ConnInfo* ci )
					: e_( 0 ), eIndex_( 0 ), index_( 0 ), info_( ci )
			{;}

			///\todo Need to extend to specify msgsrc/dest
			Conn( Element* e, unsigned int targetConnIndex )
					: e_( e ), index_( targetConnIndex ), 
					info_( getSimpleConnInfo( 0 ) )
			{;}

			~Conn()
			{
				// Calls the virtual function that decides if the ConnInfo
				// should really be deallocated.
				if ( info_ != 0 )
					info_->freeMe();
			}

			
			/**
			 * Returns the originating Element for this Conn
			 * Used infrequently, involves multiple lookups.
			 * Refers to ConnInfo because the conn may be composite.
			 */
			Element* sourceElement() const;

			/**
			 * Returns the target Element for this Conn. Fast.
			 * Used in every message call, just returns local field.
			 */
			Element* targetElement() const {
					return e_;
			}

			/**
			 * Returns the index of the originating Element for this Conn
			 * Used infrequently, involves multiple lookups.
			 * Refers to ConnInfo.
			 */
			unsigned int sourceEindex() const;

			/**
			 * Returns the index of the target Element for this Conn. Fast.
			 */
			unsigned int targetEindex() const {
					return eIndex_;
			}

			/**
			 * Returns the index of this Conn on the originating
			 * Element, using internal information only.
			 * Used infrequently, involves multiple lookups.
			 * Use this call for preference as it is safer.
			 */
			unsigned int sourceIndex( ) const;

			/**
			 * Returns the index of the target Conn. Fast.
			 * Used in every message call, just returns local field
			 */
			unsigned int targetIndex() const {
					return index_;
			}

			/**
			 * Returns index of source slot. The first few slots are
			 * the MsgSrcs, and the rest are Dests.
			 * As usual, needs to look up the matching Conn to get this 
			 * info, so it is slow.
			 */
			unsigned int sourceSlotIndex() const;
			
			/**
			 * Returns index of target slot. The first few slots are
			 * the MsgSrcs, and the rest are Dests.
			 * Finds this by looking up the index in the ConnInfo.
			 * Generic messages just have a templated instance of the
			 * basic ConnInfo that returns the index as specified by 
			 * templating. Such instances are shared.
			 * Composite messages are independently allocated, easy to
			 * set up index.
			 */
			unsigned int targetSlotIndex() const;

			/**
			 * This function tells the target conn that the 
			 * index of the source has changed to j.
			 */
			void updateIndex( unsigned int j );
			
			// Deprecated
			// void set( Element* e, unsigned int index );

			/**
			 * This utility function gets the data pointer from the
			 * targetElement. It is used very frequently in recvFuncs.
			 */
			void* data() const;

			/**
			 * Utility function used only by Copy.cpp as far as I know
			 */
			void replaceElement( Element* e ) {
				e_ = e;
			}

		private:
			
			/// e_ points to the target element.
			Element* e_;

			/// Index of target entry for array elements.
			unsigned int eIndex_;


			/**
			 * index_ is the index of the return Conn on the MsgSrc or Dest.
			 * No absolute index any more. This is more useful too,
			 * for things like synapse lookup, which use relative index.
			 * Minor issue crops up with shared messages when we have
			 * inputs from multiple source FuncLists, in which case the
			 * lookup index has to be cumulated. Ugh.
			 */
			unsigned int index_;	

			/**
			 * The ConnInfo points to extra information about this 
			 * connection. It is actually instantiated as an element
			 * somewhere, but I haven't worked out where as yet.
			 * ConnInfo can specify full connection matrix.
			 * Can also work through on-the-fly calculations for 
			 * source and dest without any specific storage.
			 * In principle, could set up connections dynamically,
			 * say all targets that meet some criterion. 
			 * This would only work if the targets point to the same
			 * ConnInfo.
			 */
			ConnInfo* info_;
};

#endif // _CONN_H
