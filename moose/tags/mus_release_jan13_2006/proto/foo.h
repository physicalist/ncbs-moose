#ifndef _ShellWrapper_h
#define _ShellWrapper_h
class ShellWrapper: 
	public Shell, public Neutral
{
	friend Element* addInConnLookup( const Conn* );
	friend Element* dropInConnLookup( const Conn* );
	friend Element* setInConnLookup( const Conn* );
	friend Element* createInConnLookup( const Conn* );
	friend Element* deleteInConnLookup( const Conn* );
	friend Element* moveInConnLookup( const Conn* );
	friend Element* copyInConnLookup( const Conn* );
	friend Element* copyShallowInConnLookup( const Conn* );
	friend Element* copyHaloInConnLookup( const Conn* );
	friend Element* ceInConnLookup( const Conn* );
	friend Element* pusheInConnLookup( const Conn* );
	friend Element* popeInConnLookup( const Conn* );
	friend Element* aliasInConnLookup( const Conn* );
	friend Element* quitInConnLookup( const Conn* );
	friend Element* startInConnLookup( const Conn* );
	friend Element* stopInConnLookup( const Conn* );
	friend Element* resetInConnLookup( const Conn* );
	friend Element* stepInConnLookup( const Conn* );
	friend Element* callInConnLookup( const Conn* );
	friend Element* getInConnLookup( const Conn* );
	friend Element* getmsgInConnLookup( const Conn* );
	friend Element* isaInConnLookup( const Conn* );
	friend Element* showInConnLookup( const Conn* );
	friend Element* showmsgInConnLookup( const Conn* );
	friend Element* showobjectInConnLookup( const Conn* );
	friend Element* pweInConnLookup( const Conn* );
	friend Element* leInConnLookup( const Conn* );
	friend Element* listcommandsInConnLookup( const Conn* );
	friend Element* listobjectsInConnLookup( const Conn* );
    public:
		ShellWrapper(const string& n)
		:
			Neutral( n ),
			// addInConn uses a templated lookup function,
			// dropInConn uses a templated lookup function,
			// setInConn uses a templated lookup function,
			// createInConn uses a templated lookup function,
			// deleteInConn uses a templated lookup function,
			// moveInConn uses a templated lookup function,
			// copyInConn uses a templated lookup function,
			// copyShallowInConn uses a templated lookup function,
			// copyHaloInConn uses a templated lookup function,
			// ceInConn uses a templated lookup function,
			// pusheInConn uses a templated lookup function,
			// popeInConn uses a templated lookup function,
			// aliasInConn uses a templated lookup function,
			// quitInConn uses a templated lookup function,
			// startInConn uses a templated lookup function,
			// stopInConn uses a templated lookup function,
			// resetInConn uses a templated lookup function,
			// stepInConn uses a templated lookup function,
			// callInConn uses a templated lookup function,
			// getInConn uses a templated lookup function,
			// getmsgInConn uses a templated lookup function,
			// isaInConn uses a templated lookup function,
			// showInConn uses a templated lookup function,
			// showmsgInConn uses a templated lookup function,
			// showobjectInConn uses a templated lookup function,
			// pweInConn uses a templated lookup function,
			// leInConn uses a templated lookup function,
			// listcommandsInConn uses a templated lookup function,
			// listobjectsInConn uses a templated lookup function
		{
			;
		}
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		static void addFunc( Conn* c, string src, string dest ) {
			static_cast< ShellWrapper* >( c->parent() )->
				addFuncLocal( src, dest );
		}
		void addFuncLocal( string src, string dest ) {
		}
		static void dropFunc( Conn* c, string src, string dest ) {
			static_cast< ShellWrapper* >( c->parent() )->
				dropFuncLocal( src, dest );
		}
		void dropFuncLocal( string src, string dest ) {
		}
		static void setFunc( Conn* c, string field, string value ) {
			static_cast< ShellWrapper* >( c->parent() )->
				setFuncLocal( field, value );
		}
		void setFuncLocal( string field, string value ) {
			/*
			vector< Element* >f;
			fillElist( f, field ); 
			*/
		}
		static void createFunc( Conn* c, string type, string path ) {
			static_cast< ShellWrapper* >( c->parent() )->
				createFuncLocal( type, path );
		}
		void createFuncLocal( string type, string path ) {
		}
		static void deleteFunc( Conn* c, string path ) {
			static_cast< ShellWrapper* >( c->parent() )->
				deleteFuncLocal( path );
		}
		void deleteFuncLocal( string path ) {
		}
		static void moveFunc( Conn* c, string src, string dest ) {
			static_cast< ShellWrapper* >( c->parent() )->
				moveFuncLocal( src, dest );
		}
		void moveFuncLocal( string src, string dest ) {
		}
		static void copyFunc( Conn* c, string src, string dest ) {
			static_cast< ShellWrapper* >( c->parent() )->
				copyFuncLocal( src, dest );
		}
		void copyFuncLocal( string src, string dest ) {
		}
		static void copyShallowFunc( Conn* c, string src, string dest ) {
			static_cast< ShellWrapper* >( c->parent() )->
				copyShallowFuncLocal( src, dest );
		}
		void copyShallowFuncLocal( string src, string dest ) {
		}
		static void copyHaloFunc( Conn* c, string src, string dest ) {
			static_cast< ShellWrapper* >( c->parent() )->
				copyHaloFuncLocal( src, dest );
		}
		void copyHaloFuncLocal( string src, string dest ) {
		}
		static void ceFunc( Conn* c, string newpath ) {
			static_cast< ShellWrapper* >( c->parent() )->
				ceFuncLocal( newpath );
		}
		void ceFuncLocal( string newpath ) {
		}
		static void pusheFunc( Conn* c, string newpath ) {
			static_cast< ShellWrapper* >( c->parent() )->
				pusheFuncLocal( newpath );
		}
		void pusheFuncLocal( string newpath ) {
		}
		static void popeFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				popeFuncLocal(  );
		}
		void popeFuncLocal(  ) {
		}
		static void aliasFunc( Conn* c, string orig, string new ) {
			static_cast< ShellWrapper* >( c->parent() )->
				aliasFuncLocal( orig, new );
		}
		void aliasFuncLocal( string orig, string new ) {
		}
		static void quitFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				quitFuncLocal(  );
		}
		void quitFuncLocal(  ) {
		}
		static void startFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				startFuncLocal(  );
		}
		void startFuncLocal(  ) {
		}
		static void stopFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				stopFuncLocal(  );
		}
		void stopFuncLocal(  ) {
		}
		static void resetFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				resetFuncLocal(  );
		}
		void resetFuncLocal(  ) {
		}
		static void stepFunc( Conn* c, string steptime ) {
			static_cast< ShellWrapper* >( c->parent() )->
				stepFuncLocal( steptime );
		}
		void stepFuncLocal( string steptime ) {
		}
		static void callFunc( Conn* c, string args ) {
			static_cast< ShellWrapper* >( c->parent() )->
				callFuncLocal( args );
		}
		void callFuncLocal( string args ) {
		}
		static void getFunc( Conn* c, string field ) {
			static_cast< ShellWrapper* >( c->parent() )->
				getFuncLocal( field );
		}
		void getFuncLocal( string field ) {
		}
		static void getmsgFunc( Conn* c, string field ) {
			static_cast< ShellWrapper* >( c->parent() )->
				getmsgFuncLocal( field );
		}
		void getmsgFuncLocal( string field ) {
		}
		static void isaFunc( Conn* c, string type, string field ) {
			static_cast< ShellWrapper* >( c->parent() )->
				isaFuncLocal( type, field );
		}
		void isaFuncLocal( string type, string field ) {
		}
		static void showFunc( Conn* c, string field ) {
			static_cast< ShellWrapper* >( c->parent() )->
				showFuncLocal( field );
		}
		void showFuncLocal( string field ) {
		}
		static void showmsgFunc( Conn* c, string field ) {
			static_cast< ShellWrapper* >( c->parent() )->
				showmsgFuncLocal( field );
		}
		void showmsgFuncLocal( string field ) {
		}
		static void showobjectFunc( Conn* c, string classname ) {
			static_cast< ShellWrapper* >( c->parent() )->
				showobjectFuncLocal( classname );
		}
		void showobjectFuncLocal( string classname ) {
		}
		static void pweFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				pweFuncLocal(  );
		}
		void pweFuncLocal(  ) {
			cout << workingElement_->name();
		}
		static void leFunc( Conn* c ) {
			static_cast< ShellWrapper* >( c->parent() )->
				leFuncLocal(  );
		}
		void leFuncLocal(  ) {
		}
		static void listcommandsFunc( Conn* c, string classname ) {
			static_cast< ShellWrapper* >( c->parent() )->
				listcommandsFuncLocal( classname );
		}
		void listcommandsFuncLocal( string classname ) {
		}
		static void listobjectsFunc( Conn* c, string classname ) {
			static_cast< ShellWrapper* >( c->parent() )->
				listobjectsFuncLocal( classname );
		}
		void listobjectsFuncLocal( string classname ) {
		}

///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getAddInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->addInConn_ );
		}
		static Conn* getDropInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->dropInConn_ );
		}
		static Conn* getSetInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->setInConn_ );
		}
		static Conn* getCreateInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->createInConn_ );
		}
		static Conn* getDeleteInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->deleteInConn_ );
		}
		static Conn* getMoveInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->moveInConn_ );
		}
		static Conn* getCopyInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->copyInConn_ );
		}
		static Conn* getCopyShallowInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->copyShallowInConn_ );
		}
		static Conn* getCopyHaloInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->copyHaloInConn_ );
		}
		static Conn* getCeInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->ceInConn_ );
		}
		static Conn* getPusheInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->pusheInConn_ );
		}
		static Conn* getPopeInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->popeInConn_ );
		}
		static Conn* getAliasInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->aliasInConn_ );
		}
		static Conn* getQuitInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->quitInConn_ );
		}
		static Conn* getStartInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->startInConn_ );
		}
		static Conn* getStopInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->stopInConn_ );
		}
		static Conn* getResetInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->resetInConn_ );
		}
		static Conn* getStepInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->stepInConn_ );
		}
		static Conn* getCallInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->callInConn_ );
		}
		static Conn* getGetInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->getInConn_ );
		}
		static Conn* getGetmsgInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->getmsgInConn_ );
		}
		static Conn* getIsaInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->isaInConn_ );
		}
		static Conn* getShowInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->showInConn_ );
		}
		static Conn* getShowmsgInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->showmsgInConn_ );
		}
		static Conn* getShowobjectInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->showobjectInConn_ );
		}
		static Conn* getPweInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->pweInConn_ );
		}
		static Conn* getLeInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->leInConn_ );
		}
		static Conn* getListcommandsInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->listcommandsInConn_ );
		}
		static Conn* getListobjectsInConn( Element* e ) {
			return &( static_cast< ShellWrapper* >( e )->listobjectsInConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const Shell* p = dynamic_cast<const Shell *>(proto);
			// if (p)... and so on. 
			return new ShellWrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		UniConn< addInConnLookup > addInConn_;
		UniConn< dropInConnLookup > dropInConn_;
		UniConn< setInConnLookup > setInConn_;
		UniConn< createInConnLookup > createInConn_;
		UniConn< deleteInConnLookup > deleteInConn_;
		UniConn< moveInConnLookup > moveInConn_;
		UniConn< copyInConnLookup > copyInConn_;
		UniConn< copyShallowInConnLookup > copyShallowInConn_;
		UniConn< copyHaloInConnLookup > copyHaloInConn_;
		UniConn< ceInConnLookup > ceInConn_;
		UniConn< pusheInConnLookup > pusheInConn_;
		UniConn< popeInConnLookup > popeInConn_;
		UniConn< aliasInConnLookup > aliasInConn_;
		UniConn< quitInConnLookup > quitInConn_;
		UniConn< startInConnLookup > startInConn_;
		UniConn< stopInConnLookup > stopInConn_;
		UniConn< resetInConnLookup > resetInConn_;
		UniConn< stepInConnLookup > stepInConn_;
		UniConn< callInConnLookup > callInConn_;
		UniConn< getInConnLookup > getInConn_;
		UniConn< getmsgInConnLookup > getmsgInConn_;
		UniConn< isaInConnLookup > isaInConn_;
		UniConn< showInConnLookup > showInConn_;
		UniConn< showmsgInConnLookup > showmsgInConn_;
		UniConn< showobjectInConnLookup > showobjectInConn_;
		UniConn< pweInConnLookup > pweInConn_;
		UniConn< leInConnLookup > leInConn_;
		UniConn< listcommandsInConnLookup > listcommandsInConn_;
		UniConn< listobjectsInConnLookup > listobjectsInConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _ShellWrapper_h
