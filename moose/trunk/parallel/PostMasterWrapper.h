/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/*
struct PostBuffer {
	public: 
		unsigned int schedule;
		unsigned long size;
		vector< unsigned int > offset_;
	private:
		char* buffer;
};
*/


// Dummy functions
void isend( char* buf, int size, char* name, int dest );
void irecv( char* buf, int size, char* name, int src );

#ifndef _PostMasterWrapper_h
#define _PostMasterWrapper_h
class PostMasterWrapper: 
	public PostMaster, public Neutral
{
	friend Element* processConnPostMasterLookup( const Conn* );
    public:
		PostMasterWrapper(const string& n);
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////

		void localSetRemoteNode( int node );
		static void setRemoteNode( Conn* c, int value ) {
			static_cast< PostMasterWrapper* >( c->parent() )->
					localSetRemoteNode( value );
		}

		static int getRemoteNode( const Element* e ) {
			return static_cast< const PostMasterWrapper* >( e )->remoteNode_;
		}

		static int getLocalNode( const Element* e ) {
			return static_cast< const PostMasterWrapper* >( e )->localNode_;
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
		static NMsgSrc* getSrcSrc( Element* e ) {
			return &( static_cast< PostMasterWrapper* >( e )->srcSrc_ );
		}

///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		void destFuncLocal( long index ) {
		}
		static void destFunc( Conn* c ) {
			static_cast< PostMasterWrapper* >( c->parent() )->
				destFuncLocal(
				static_cast< SolverConn* >( c )->index() );
		}

		void processFuncLocal( ProcInfo info );
		static void processFunc( Conn* c, ProcInfo info ) {
			static_cast< PostMasterWrapper* >( c->parent() )->
				processFuncLocal( info );
		}

		void reinitFuncLocal(  );
		static void reinitFunc( Conn* c ) {
			static_cast< PostMasterWrapper* >( c->parent() )->
				reinitFuncLocal(  );
		}

		void postFuncLocal( ProcInfo info );
		static void postFunc( Conn* c, ProcInfo info ) {
			static_cast< PostMasterWrapper* >( c->parent() )->
				postFuncLocal( info );
		}

		// Dummy func, equivalent of reinitFunc, does nothing.
		static void postInitFunc( Conn* c ) {
			;
		}


///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getPostConn( Element* e ) {
			return &( static_cast< PostMasterWrapper* >( e )->postConn_ );
		}
		static Conn* getProcessConn( Element* e ) {
			return &( static_cast< PostMasterWrapper* >( e )->processConn_ );
		}
		static Conn* getSrcOutConn( Element* e ) {
			return &( static_cast< PostMasterWrapper* >( e )->srcOutConn_ );
		}
		static Conn* getDestInConn( Element* e ) {
			return &( static_cast< PostMasterWrapper* >( e )->destInConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const PostMaster* p = dynamic_cast<const PostMaster *>(proto);
			// if (p)... and so on. 
			return new PostMasterWrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}

		char* getPostPtr( unsigned long index );
///////////////////////////////////////////////////////
// Schedule and setup functions.                     //
///////////////////////////////////////////////////////

		void assignSchedule();
		void informTargetNode();
		void connectTick( Element* tick );
		bool callsMe( Element* tickElm );
		bool connect( const string& target );
		void checkPendingRequests();

		void assignIncomingSizes();
		void assignIncomingSchedule();

///////////////////////////////////////////////////////
// testing functions
///////////////////////////////////////////////////////
		void isend( char* buf, int size, char* name, int tag, int dest);
		void irecv( char* buf, int size, char* name, int tag, int src );

    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		ParallelMsgSrc srcSrc_;
		MultiConn postConn_;
		UniConn< processConnPostMasterLookup > processConn_;
		MultiConn srcOutConn_;
		SolveMultiConn destInConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Private functions and fields for the Wrapper class//
///////////////////////////////////////////////////////
// here are some for testing
		char* remoteTransferBuffer_;
		char localTransferBuffer_[1000];

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _PostMasterWrapper_h
