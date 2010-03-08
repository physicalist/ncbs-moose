/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MSG_H
#define _MSG_H

/**
 * Manages data flow between two elements. Is always many-to-many, with
 * assorted variants.
 */

typedef unsigned int MsgId;

class Msg
{
	public:
		Msg( Element* e1, Element* e2 );
		virtual ~Msg();

		/**
		 * Deletes a message identified by its mid.
		 */
		static void deleteMsg( MsgId mid );

		/**
 		* Initialize the Null location in the Msg vector.
 		*/
		static void initNull();

		/**
		 * Call clearQ on e1. Note that it applies at
		 * the Element level, not the index level.
		 */
		// virtual void clearQ() const;

		/**
		 * Add an event to the queue on the target element.
		 * This is generally the same function, so the base Msg provides it.
		 * Will need to be specialized for multithread and
		 * multinode dispersal of messages too.
		virtual void addToQ( const Element* caller, Qinfo& q,
			const ProcInfo* p, const char* arg ) const;
		 */

		/**
		 * Calls Process on e1.
		 */
		virtual void process( const ProcInfo *p ) const;

		
		/**
		 * Execute func( arg ) on all relevant indices of target
		 */
		virtual void exec( const char* arg, const ProcInfo* p ) const = 0;

		// Something here to set up sync message buffers

		// Something here to print message info
		// virtual void print( string& output ) const;

		// Duplicate message on new Elements.
		// virtual Msg* dup( Element* e1, Element* e2 ) const;

		Element* e1() const {
			return e1_;
		}

		Element* e2() const {
			return e2_;
		}

		MsgId mid() const {
			return mid_;
		}

		/*
		MsgId mid1() const {
			return m1_;
		}

		MsgId mid2() const {
			return m2_;
		}
		*/

		/**
		 * Looks up the message on the global vector of Msgs.
		 */
		static const Msg* getMsg( MsgId m );

		/**
		 * The zero MsgId, used as the error value.
		 */
		static const MsgId Null;

	protected:
		Element* e1_;
		Element* e2_;
		MsgId mid_; // Index of this msg on the msg_ vector.

		/// Manages all Msgs in the system.
		static vector< Msg* > msg_;

		/**
		 * Manages garbage collection for deleted messages. Deleted
		 * MsgIds are kept here so that they can be reused.
		 */
		static vector< MsgId > garbageMsg_;
};

class SingleMsg: public Msg
{
	public:
		SingleMsg( Eref e1, Eref e2 );
		~SingleMsg() {;}

		void exec( const char* arg, const ProcInfo* p) const;

		static bool add( Eref e1, const string& srcField, 
			Eref e2, const string& destField );

	private:
		DataId i1_;
		DataId i2_;
};

class OneToOneMsg: public Msg
{
	public:
		OneToOneMsg( Element* e1, Element* e2 );
		~OneToOneMsg() {;}

		void exec( const char* arg, const ProcInfo* p) const;
	private:
};

class OneToAllMsg: public Msg
{
	public:
		OneToAllMsg( Eref e1, Element* e2 );
		~OneToAllMsg() {;}

		void exec( const char* arg, const ProcInfo* p ) const;

		static bool add( Eref e1, const string& srcField, 
			Element* e2, const string& destField );

	private:
		DataId i1_;
};


#endif // _MSG_H
