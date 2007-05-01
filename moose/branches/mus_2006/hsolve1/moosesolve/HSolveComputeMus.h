/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSolveComputeMus_h
#define _HSolveComputeMus_h

class HSolveComputeMus:
	public HSolveCompute< Element* >, public Neutral
{
	friend Element* processConnHSolveComputeMusLookup( const Conn* );
	friend Element* reinitializeInConnHSolveComputeMusLookup( const Conn* );
	friend Element* writeConduitInConnHSolveComputeMusLookup( const Conn* );
	friend Element* recordCompartmentInConnHSolveComputeMusLookup( const Conn* );
	friend Element* printVoltageTraceInConnHSolveComputeMusLookup( const Conn* );

    public:
		HSolveComputeMus(const string& n)
		:
			Neutral( n )
			// processConn uses a templated lookup function,
			// reinitializeInConn uses a templated lookup function,
			// writeConduitInConn uses a templated lookup function,
			// recordCompartmentInConn uses a templated lookup function,
			// printVoltageTraceInConn uses a templated lookup function
		{
			;
		}
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static void setNDiv( Conn* c, int value ) {
			static_cast< HSolveComputeMus* >( c->parent() )->NDiv_ = value;
		}
		static int getNDiv( const Element* e ) {
			return static_cast< const HSolveComputeMus* >( e )->NDiv_;
		}
		static void setVLo( Conn* c, double value ) {
			static_cast< HSolveComputeMus* >( c->parent() )->VLo_ = value;
		}
		static double getVLo( const Element* e ) {
			return static_cast< const HSolveComputeMus* >( e )->VLo_;
		}
		static void setVHi( Conn* c, double value ) {
			static_cast< HSolveComputeMus* >( c->parent() )->VHi_ = value;
		}
		static double getVHi( const Element* e ) {
			return static_cast< const HSolveComputeMus* >( e )->VHi_;
		}
///////////////////////////////////////////////////////
//    EvalField header definitions.                  //
///////////////////////////////////////////////////////
		string localGetPath() const;
		static string getPath( const Element* e ) {
			return static_cast< const HSolveComputeMus* >( e )->
			localGetPath();
		}
		void localSetPath( string value );
		static void setPath( Conn* c, string value ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
			localSetPath( value );
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		void reinitFuncLocal(  ) {
			;
		}
		static void reinitFunc( Conn* c ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
				reinitFuncLocal(  );
		}
		void reinitializeFuncLocal( double dt ) {
			initialize( &model_, seed_, dt,
			            NDiv_, VLo_, VHi_ );
		}
		static void reinitializeFunc( Conn* c, double dt ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
				reinitializeFuncLocal( dt );
		}
		void processFuncLocal( ProcInfo info ) {
			storeRecordings( info->currTime_ );
			step( );
		}
		static void processFunc( Conn* c, ProcInfo info ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
				processFuncLocal( info );
		}
		void writeConduitFuncLocal( string fileName ) {
			this->writeConduit( fileName );
		}
		static void writeConduitFunc( Conn* c, string fileName ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
				writeConduitFuncLocal( fileName );
		}
		void recordCompartmentFuncLocal( string path );
		static void recordCompartmentFunc( Conn* c, string path ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
				recordCompartmentFuncLocal( path );
		}
		void printVoltageTraceFuncLocal( string fileName, int plotMode );
		static void printVoltageTraceFunc( Conn* c, string fileName, int plotMode ) {
			static_cast< HSolveComputeMus* >( c->parent() )->
				printVoltageTraceFuncLocal( fileName, plotMode );
		}


///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getProcessConn( Element* e ) {
			return &( static_cast< HSolveComputeMus* >( e )->processConn_ );
		}
		static Conn* getReinitializeInConn( Element* e ) {
			return &( static_cast< HSolveComputeMus* >( e )->reinitializeInConn_ );
		}
		static Conn* getWriteConduitInConn( Element* e ) {
			return &( static_cast< HSolveComputeMus* >( e )->writeConduitInConn_ );
		}
		static Conn* getRecordCompartmentInConn( Element* e ) {
			return &( static_cast< HSolveComputeMus* >( e )->recordCompartmentInConn_ );
		}
		static Conn* getPrintVoltageTraceInConn( Element* e ) {
			return &( static_cast< HSolveComputeMus* >( e )->printVoltageTraceInConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const HSolveComputeMus* p = dynamic_cast<const HSolveComputeMus *>(proto);
			// if (p)... and so on. 
			return new HSolveComputeMus(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		UniConn< processConnHSolveComputeMusLookup > processConn_;
		UniConn< reinitializeInConnHSolveComputeMusLookup > reinitializeInConn_;
		UniConn< writeConduitInConnHSolveComputeMusLookup > writeConduitInConn_;
		UniConn< recordCompartmentInConnHSolveComputeMusLookup > recordCompartmentInConn_;
		UniConn< printVoltageTraceInConnHSolveComputeMusLookup > printVoltageTraceInConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Private functions and fields for the Wrapper class//
///////////////////////////////////////////////////////
		HSolveModelMus           model_;
		Element*                 seed_;
		string                   path_;
		int                      NDiv_;
		double                   VLo_;
		double                   VHi_;
		void                     setPath( const string& path );
		void                     writeConduit( const string& fileName ) const;
		void                     storeRecordings( double currTime );
		vector< string >         recordPath_;
		vector< unsigned long >  recordIndex_;
		vector< double >         plotTime_;
		vector< vector< double > > plotVolt_;

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _HSolveComputeMus_h
