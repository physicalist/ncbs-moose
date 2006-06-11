#ifndef _HHGateWrapper_h
#define _HHGateWrapper_h
class HHGateWrapper: 
	public HHGate, public Neutral
{
	friend Element* gateConnHHGateLookup( const Conn* );
    public:
		HHGateWrapper(const string& n)
		:
			Neutral( n ),
			gateSrc_( &gateConn_ )
			// gateConn uses a templated lookup function
		{
			;
		}
		void test();
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static void setPower( Conn* c, double value ) {
			static_cast< HHGateWrapper* >( c->parent() )->power_ = value;
		}
		static double getPower( const Element* e ) {
			return static_cast< const HHGateWrapper* >( e )->power_;
		}
		static void setState( Conn* c, double value ) {
			static_cast< HHGateWrapper* >( c->parent() )->state_ = value;
		}
		static double getState( const Element* e ) {
			return static_cast< const HHGateWrapper* >( e )->state_;
		}
		static void setA( Conn* c, Interpol value ) {
			static_cast< HHGateWrapper* >( c->parent() )->A_ = value;
		}
		static Interpol getA( const Element* e ) {
			return static_cast< const HHGateWrapper* >( e )->A_;
		}
		/*
		static void setA( Conn* c, Interpol* value ) {
			static_cast< HHGateWrapper* >( c->parent() )->A_ = *value;
		}
		static const Interpol* getA( const Element* e ) {
			return &( static_cast< const HHGateWrapper* >( e )->A_ );
		}
		*/
		static Element* lookupA( Element* e, unsigned long index )
		{
			static InterpolWrapper iw("temp");
			static Interpol* ip = &iw;
			static const unsigned long OFFSET = 
				( unsigned long )( ip ) - ( unsigned long )(&iw);
				// FIELD_OFFSET( InterpolWrapper, Interpol );
			// cerr << "in lookupA (the interpol ): OFFSET = " << OFFSET << "\n";
			return reinterpret_cast< InterpolWrapper* >( 
				( unsigned long )
				( &static_cast< const HHGateWrapper* >( e )->A_ ) -
				OFFSET
				);
		}

		static void setB( Conn* c, Interpol value ) {
			static_cast< HHGateWrapper* >( c->parent() )->B_ = value;
		}
		static Interpol getB( const Element* e ) {
			return static_cast< const HHGateWrapper* >( e )->B_;
		}
		static Element* lookupB( Element* e, unsigned long index )
		{
			static InterpolWrapper iw("temp");
			static Interpol* ip = &iw;
			static const unsigned long OFFSET = 
				( unsigned long )( ip ) - ( unsigned long )(&iw);
			return reinterpret_cast< InterpolWrapper* >( 
				( unsigned long )
				( &static_cast< const HHGateWrapper* >( e )->B_ ) -
				OFFSET
				);
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
		static SingleMsgSrc* getGateSrc( Element* e ) {
			return &( static_cast< HHGateWrapper* >( e )->gateSrc_ );
		}

///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		void gateFuncLocal( double v, double state, double dt );
		static void gateFunc( Conn* c, double v, double state, double dt ) {
			static_cast< HHGateWrapper* >( c->parent() )->
				gateFuncLocal( v, state, dt );
		}

		void reinitFuncLocal( double Vm );
		static void reinitFunc( Conn* c, double Vm ) {
			static_cast< HHGateWrapper* >( c->parent() )->
				reinitFuncLocal( Vm );
		}


///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getGateConn( Element* e ) {
			return &( static_cast< HHGateWrapper* >( e )->gateConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const HHGate* p = dynamic_cast<const HHGate *>(proto);
			// if (p)... and so on. 
			return new HHGateWrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		SingleMsgSrc2< double, double > gateSrc_;
		UniConn< gateConnHHGateLookup > gateConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _HHGateWrapper_h
