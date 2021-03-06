#ifndef _ReactionWrapper_h
#define _ReactionWrapper_h
class ReactionWrapper: 
	public Reaction, public Neutral
{
	friend Element* processConnReactionLookup( const Conn* );
    public:
		ReactionWrapper(const string& n)
		:
			Neutral( n ),
			subSrc_( &subConn_ ),
			prdSrc_( &prdConn_ ),
			// processConn uses a templated lookup function,
			subConn_( this ),
			prdConn_( this )
		{
			;
		}
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static void setKf( Conn* c, double value ) {
			static_cast< ReactionWrapper* >( c->parent() )->kf_ = value;
		}
		static double getKf( const Element* e ) {
			return static_cast< const ReactionWrapper* >( e )->kf_;
		}
		static void setKb( Conn* c, double value ) {
			static_cast< ReactionWrapper* >( c->parent() )->kb_ = value;
		}
		static double getKb( const Element* e ) {
			return static_cast< const ReactionWrapper* >( e )->kb_;
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
		static NMsgSrc* getSubSrc( Element* e ) {
			return &( static_cast< ReactionWrapper* >( e )->subSrc_ );
		}

		static NMsgSrc* getPrdSrc( Element* e ) {
			return &( static_cast< ReactionWrapper* >( e )->prdSrc_ );
		}

///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		static void reinitFunc( Conn* c ) {
			static_cast< ReactionWrapper* >( c->parent() )->
				reinitFuncLocal(  );
		}
		void reinitFuncLocal(  ) {
			A_ = B_ = 0;
		}
		static void processFunc( Conn* c, ProcInfo info ) {
			static_cast< ReactionWrapper* >( c->parent() )->
				processFuncLocal( info );
		}
		void processFuncLocal( ProcInfo info );
		static void subFunc( Conn* c, double n ) {
			static_cast< ReactionWrapper* >( c->parent() )->
				subFuncLocal( n );
		}
		void subFuncLocal( double n ) {
			A_ *= n;
		}
		static void prdFunc( Conn* c, double n ) {
			static_cast< ReactionWrapper* >( c->parent() )->
				prdFuncLocal( n );
		}
		void prdFuncLocal( double n ) {
			B_ *= n;
		}

///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getProcessConn( Element* e ) {
			return &( static_cast< ReactionWrapper* >( e )->processConn_ );
		}
		static Conn* getSubConn( Element* e ) {
			return &( static_cast< ReactionWrapper* >( e )->subConn_ );
		}
		static Conn* getPrdConn( Element* e ) {
			return &( static_cast< ReactionWrapper* >( e )->prdConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const Reaction* p = dynamic_cast<const Reaction *>(proto);
			// if (p)... and so on. 
			return new ReactionWrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		NMsgSrc2< double, double > subSrc_;
		NMsgSrc2< double, double > prdSrc_;
		UniConn< processConnReactionLookup > processConn_;
		MultiConn subConn_;
		MultiConn prdConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _ReactionWrapper_h
