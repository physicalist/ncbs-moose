#ifndef _HHGateWrapper_h
#define _HHGateWrapper_h
class HHGate2Wrapper: public HHGate2, public Neutral
{
    public:
		HHGate2Wrapper(const string& n)
		:
			Neutral( n ),
			gateConn_( this )
		{
			;
		}
		void test();
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static void setPower( Conn* c, double value ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->power_ = value;
		}
		static double getPower( const Element* e ) {
			return static_cast< const HHGate2Wrapper* >( e )->power_;
		}
		static void setState( Conn* c, double value ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->state_ = value;
		}
		static double getState( const Element* e ) {
			return static_cast< const HHGate2Wrapper* >( e )->state_;
		}
		static void setA( Conn* c, Interpol value ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->A_ = value;
		}
		static Interpol getA( const Element* e ) {
			return static_cast< const HHGate2Wrapper* >( e )->A_;
		}
		/*
		static void setA( Conn* c, Interpol* value ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->A_ = *value;
		}
		static const Interpol* getA( const Element* e ) {
			return &( static_cast< const HHGate2Wrapper* >( e )->A_ );
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
				( &static_cast< const HHGate2Wrapper* >( e )->A_ ) -
				OFFSET
				);
		}

		static void setB( Conn* c, Interpol value ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->B_ = value;
		}
		static Interpol getB( const Element* e ) {
			return static_cast< const HHGate2Wrapper* >( e )->B_;
		}
		static Element* lookupB( Element* e, unsigned long index )
		{
			static InterpolWrapper iw("temp");
			static Interpol* ip = &iw;
			static const unsigned long OFFSET = 
				( unsigned long )( ip ) - ( unsigned long )(&iw);
			return reinterpret_cast< InterpolWrapper* >( 
				( unsigned long )
				( &static_cast< const HHGate2Wrapper* >( e )->B_ ) -
				OFFSET
				);
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		void gateFuncLocal( Conn* c, double v, double state, double dt);

		static void gateFunc( Conn* c, double v, double state, double dt ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->
				gateFuncLocal( c, v, state, dt );
		}

		void reinitFuncLocal( Conn* c, double Vm );
		static void reinitFunc( Conn* c, double Vm ) {
			static_cast< HHGate2Wrapper* >( c->parent() )->
				reinitFuncLocal( c, Vm );
		}


///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getGateConn( Element* e ) {
			return &( static_cast< HHGate2Wrapper* >( e )->gateConn_ );
		}
		static MultiReturnConn* getGateMultiReturnConn( Element* e ) {
			return &( static_cast< HHGate2Wrapper* >( e )->gateConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const HHGate2* p = dynamic_cast<const HHGate2 *>(proto);
			// if (p)... and so on. 
			return new HHGate2Wrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		MultiReturnConn gateConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _HHGate2Wrapper_h
