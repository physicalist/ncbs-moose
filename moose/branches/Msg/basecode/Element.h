/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class SrcFinfo;
class Qinfo;
/**
 * Have a single Element class, that uses an IndexRange. There can
 * be multiple such objects representing a complete array. So the Id
 * must know how to access them all. Or they each refer to each other.
 * Or the user only sees the master (but may be many if distrib over nodes)
 */
class Element
{
	friend void testSync();
	friend void testAsync();
	friend void testStandaloneIntFire();
	friend void testSynapse();
	friend void testSyncArray( unsigned int, unsigned int, unsigned int );
	friend void testSparseMsg();
	public:
		/**
		 * This constructor is used when making zombies. We want to have a
		 * temporary Element for field access but nothing else, and it
		 * should not mess with messages.
		 */
		Element( Id id, const Cinfo* c, DataHandler* d );

		/// Regular constructor
		Element( Id id, const Cinfo* c, const string& name,
			const vector< unsigned int >& dimensions, 
			bool isGlobal = 0 );

		/// Regular constructor
		Element( Id id, const Cinfo* c, const string& name,
			DataHandler* dataHandler );

		/**
		 * This constructor copies over the original n times.
		 */
		Element( Id id, const Element* orig, unsigned int n );

		/**
		 * Destructor
		 */
		~Element();

		/**
		 * Returns name of Element
		 */
		const string& getName() const;
		
		/**
		 * Changes name of Element
		 */
		void setName( const string& val );

		/**
		 * Examine process queue, data that is expected every timestep.
		 * This function is done for all the local data entries in order.
		 */
		void process( const ProcInfo* p, FuncId fid );


		/**
		 * Returns the DataHandler, which actually manages the data.
		 */
		DataHandler* dataHandler() const;

		/**
		 * Asynchronous send command. Adds Qinfo and data onto msg specified
		 * by bindIndex, and queue specified in the ProcInfo.
		 */
		void asend( Qinfo& q, BindIndex bindIndex, 
			const ProcInfo *p, const char* arg ) const;

		/**
		 * Asynchronous send command, going to specific target Element/Data.
		 * Adds Qinfo and data onto msg specified
		 * by bindIndex, and queue specified in the ProcInfo.
		 */
		void tsend( Qinfo& q, BindIndex bindIndex, const ProcInfo *p, 
			const char* arg, const FullId& target ) const;

		/** 
		 * Pushes the Msg mid onto the list.
		 * The position on the list does not matter.
		 * 
		 */
		void addMsg( MsgId mid );

		/**
		 * Removes the specified msg from the list.
		 */
		void dropMsg( MsgId mid );
		
		/**
		 * Clears out all Msgs on specified BindIndex. Used in Shell::set
		 */
		void clearBinding( BindIndex b );

		/**
		 * Pushes back the specified Msg and Func pair into the properly
		 * indexed place on the msgBinding_ vector.
		 */
		void addMsgAndFunc( MsgId mid, FuncId fid, BindIndex bindIndex );

		/**
		 * gets the Msg/Func binding information for specified bindIndex.
		 * This is a vector.
		 * Returns 0 on failure.
		 */
		const vector< MsgFuncBinding >* getMsgAndFunc( BindIndex b ) const;

		/**
		 * Utility function for printing out all fields and their values
		 */
		void showFields() const;

		/**
		 * Utility function for traversing and displaying all messages
		 */
		void showMsg() const;

		/**
		 * Gets the class information for this Element
		 */
		const Cinfo* cinfo() const;

		/**
		 * Destroys all Elements in tree, being efficient about not
		 * trying to traverse through clearing messages to doomed Elements.
		 * Assumes tree includes all child elements.
		 * Typically the Neutral::destroy function builds up this tree
		 * and then calls this function.
		 */
		static void destroyElementTree( const vector< Id >& tree );

		/**
		 * Returns the Id on this Elm
		 */
		Id id() const;


	/////////////////////////////////////////////////////////////////////
	// Utility functions for message traversal
	/////////////////////////////////////////////////////////////////////
		/**
		 * Returns the binding index of the specified entry.
		 * Returns ~0 on failure.
		 */
		 unsigned int findBinding( MsgFuncBinding b ) const;

		/**
		 * Returns the first Msg that calls the specified Fid, 
		 * on current Element.
		 * Returns 0 on failure.
		 */
		 MsgId findCaller( FuncId fid ) const;

		/** 
		 * More general function. Fills up vector of MsgIds that call the
		 * specified Fid on current Element. Returns # found
		 */
		unsigned int getInputMsgs( vector< MsgId >& caller, FuncId fid)
		 	const;

		/**
		 * Fills in vector of Ids receiving messages from this SrcFinfo. 
		 * Returns # found
		 */
		unsigned int getOutputs( vector< Id >& ret, const SrcFinfo* finfo )
			const;

		/**
		 * Fills in vector of Ids sending messeges to this DestFinfo on
		 * this Element. Returns # found
		 */
		unsigned int getInputs( vector< Id >& ret, const DestFinfo* finfo )
			const;

	/////////////////////////////////////////////////////////////////////
		/**
		 * zombieSwap: replaces the Cinfo and DataHandler of the zombie.
		 * Deletes old DataHandler first.
		 */
		void zombieSwap( const Cinfo* newCinfo, DataHandler* newDataHandler     );

	private:
		string name_;

		Id id_; // The current way of accessing Id.

		/**
		 * This object stores and manages the actual data for the Element.
		 */
		DataHandler* dataHandler_;

		/**
		 * Class information
		 */
		const Cinfo* cinfo_;

		/**
		 * Message vector. This is the low-level messaging information.
		 * Contains info about incoming msgs? But this lacks binding info.
		 */
		vector< MsgId > m_;

		/**
		 * Binds an outgoing message to its function.
		 * SrcFinfo keeps track of the BindIndex to look things up.
		 * Note that a single BindIndex may refer to multiple Msg/Func
		 * pairs.
		 */
		vector< vector < MsgFuncBinding > > msgBinding_;
};
