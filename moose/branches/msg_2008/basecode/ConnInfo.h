/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla, and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _CONN_INFO_H
#define _CONN_INFO_H


/**
 * ConnInfo is an abstract base class for handling composite connections,
 * and providing other useful information to the Conn. 
 *
 * Provides
 * iterators for finding next conn, and lookups or generation of conns 
 * to be used in either direction.
 * In typical use, one arrayElement connects to another through a 
 * Conn using one of the sparseMatrix ConnInfos. The only permanently
 * defined Conns here are the source and dest ones acting as holders for
 * a common ConnInfo.
 *
 */
class ConnInfo
{
	public:
		ConnInfo()
		{;}

		virtual ~ConnInfo()
		{;}

		/**
		 * The slotIndex identifies which MsgSrc or Dest this
		 * Conn is attached to. Needed for traversal of messages.
		 */
		virtual unsigned int slotIndex() const = 0;

		/**
		 * Ugly hack used to handle memory when the Conn is deleted.
		 * The issue is that some ConnInfos are provided to the Conn
		 * as statics, and others as
		 * dynamically allocated objects. This function does the 
		 * object deletion of the ConnInfo only if it is not a static.
		 * Should only be called from the Conn when it is being destroyed.
		 */
		virtual void freeMe() = 0;

	private:
};

/**
 * The SimpleConnInfo is used by ordinary connections which don't worry
 * about array targets. It is normally supplied as a global static ptr
 * to the ordinary connections so they don't have to allocate their own.
 */
class SimpleConnInfo: public ConnInfo
{
	public:
		SimpleConnInfo( )
		: slotIndex_( 0 )
		{;}

		SimpleConnInfo( unsigned int slotIndex ) 
		: slotIndex_( slotIndex )
		{;}

		~SimpleConnInfo()
		{;}

		unsigned int slotIndex() const {
			return slotIndex_;
		}

		// Empty function because SimpleConnInfos are not meant to be 
		// deleted by the Conn.
		void freeMe() 
		{;}
	private:
		unsigned int slotIndex_;
};

/**
 * This utility function returns globally defined slotIndex entries.
 * Don't try to destroy them!
 */
extern SimpleConnInfo* getSimpleConnInfo( unsigned int slotIndex );

#endif // _CONN_INFO_H
