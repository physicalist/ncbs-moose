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
class ConnTainer;

/**
 * This definition is used to indicate that a conn or msg is a dummy one.
 */
#define MAXUINT (unsigned int)( ~0 )
#define MAXINT (int)( ( ~0 ) << 1 )

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
 * The nine functions the Conn must provide for RecvFunc use are: 
 * - targetElement()
 * - targetEindex()
 * - targetIndex()
 * - targetMsg()
 * - sourceElement()
 * - sourceEindex()
 * - sourceIndex()
 * - sourceMsg()
 * - data()
 * The last of these is the most used and specifies the exact chunk of 
 * memory for the recvFunc to operate on.
 *
 * In addition, the Conn must provide a way for the system to advance
 * through multiple possible targets of the Conn. This functionality
 * also permits the Conn to be skipped altogether, which is important
 * when we delete Conns but don't want to mess up iterators.
 * These functions are
 * void increment()
 * void nextElement()
 * bool good()
 *
 * Then, the Conn must provide for return messages, where the e1 and e2
 * are flipped.
 * Conn flip()
 *
 * And the Conn returns the ConnTainer it currently represents
 * const ConnTainer* connTainer() const;
 *
 * Here the Conn reports if it is going from Dest to Src.
 * bool isDest()
 */


class Conn
{
	public:
		virtual ~Conn()
		{;}

		/**
		 * Returns the target Element for this Conn
		 */
		virtual Element* targetElement() const = 0;
		/**
		 * Returns the index of the target Element for this Conn
		 */
		virtual unsigned int targetEindex() const = 0;
		/**
		 * Returns an index to uniquely identify the conn to the target
		 */
		virtual unsigned int targetIndex() const = 0;
		/**
		 * Returns an index to uniquely identify the Msg of the target
		 * Positive indices are onto Msg targets, negative are onto
		 * dest targets.
		 */
		virtual int targetMsg() const = 0;

		/**
		 * Returns the originating Element for this Conn
		 */
		virtual Element* sourceElement() const = 0;
		/**
		 * Returns the index of the source Element for this Conn
		 */
		virtual unsigned int sourceEindex() const = 0;
		/**
		 * Returns an index to uniquely identify the conn of the source
		 */
		virtual unsigned int sourceIndex() const = 0;
		/**
		 * Returns an index to uniquely identify the Msg of the source
		 * Positive indices are onto Msg targets, negative are onto
		 * dest targets.
		 */
		virtual int sourceMsg() const = 0;


		/**
		 * Gets the data pointer of the targetElement. 
		 * It is used very frequently in recvFuncs.
		 */
		virtual void* data() const = 0;

		/**
		 * increment() updates internal counter, used in iterating through
		 * targets.
		 */
		virtual void increment() = 0;

		/**
		 * nextElement() jumps from one
		 * Element and hence ConnTainer to the next. Used for iterating
		 * at the Element level.
		 */
		virtual void nextElement() = 0;

		/**
		 * True while the iteration should continue
		 */
		virtual bool good() const = 0;

		/**
		 * Returns a Conn with e1 and e2 flipped so that return messages
		 * traverse back with the correct args.
		 */
		virtual const Conn* flip() const = 0;

		/**
 		 * Returns the ConnTainer it currently represents
		 */
		virtual const ConnTainer* connTainer() const = 0;

		/**
		 * True if the Conn is going from Dest to Source
		 */
		virtual bool isDest() const = 0;

	private:
};

#endif // _CONN_H
