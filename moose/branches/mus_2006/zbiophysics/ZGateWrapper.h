/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZGateWrapper_h
#define _ZGateWrapper_h
class ZGateWrapper: public ZGate, public Neutral
{
	friend Element* gateInConnZGateLookup( const Conn* );
    public:
		ZGateWrapper(const string& n)
		:
			Neutral( n )
//			gateConn_( this )
		{
			;
		}
		void test();
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static void setPower( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->power_ = value;
		}
		static double getPower( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->power_;
		}
		static void setState( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->state_ = value;
		}
		static double getState( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->state_;
		}
		static void setAlphaForm( Conn* c, int value ) {
			static_cast< ZGateWrapper* >( c->parent() )->alphaForm_ = value;
		}
		static int getAlphaForm( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->alphaForm_;
		}
		static void setAlpha_A( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->alpha_A_ = value;
		}
		static double getAlpha_A( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->alpha_A_;
		}
		static void setAlpha_B( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->alpha_B_ = value;
		}
		static double getAlpha_B( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->alpha_B_;
		}
		static void setAlpha_V0( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->alpha_V0_ = value;
		}
		static double getAlpha_V0( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->alpha_V0_;
		}
		static void setBetaForm( Conn* c, int value ) {
			static_cast< ZGateWrapper* >( c->parent() )->betaForm_ = value;
		}
		static int getBetaForm( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->betaForm_;
		}
		static void setBeta_A( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->beta_A_ = value;
		}
		static double getBeta_A( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->beta_A_;
		}
		static void setBeta_B( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->beta_B_ = value;
		}
		static double getBeta_B( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->beta_B_;
		}
		static void setBeta_V0( Conn* c, double value ) {
			static_cast< ZGateWrapper* >( c->parent() )->beta_V0_ = value;
		}
		static double getBeta_V0( const Element* e ) {
			return static_cast< const ZGateWrapper* >( e )->beta_V0_;
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		void gateFuncLocal( ) { ; }
		static void gateFunc( Conn* c ) {
			static_cast< ZGateWrapper* >( c->parent() )->
				gateFuncLocal(  );
		}
/*		void gateFuncLocal( Conn* c, double v, double state, double dt);

		static void gateFunc( Conn* c, double v, double state, double dt ) {
			static_cast< ZGateWrapper* >( c->parent() )->
				gateFuncLocal( c, v, state, dt );
		}

		void reinitFuncLocal( Conn* c, double Vm, double power,
			int instant );
		static void reinitFunc( Conn* c, double Vm, double power,
			int instant ) {
			static_cast< ZGateWrapper* >( c->parent() )->
				reinitFuncLocal( c, Vm, power, instant );
		}*/
///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getGateInConn( Element* e ) {
			return &( static_cast< ZGateWrapper* >( e )->gateInConn_ );
		}
/*		static Conn* getGateConn( Element* e ) {
			return &( static_cast< ZGateWrapper* >( e )->gateConn_ );
		}
		static MultiReturnConn* getGateMultiReturnConn( Element* e ) {
			return &( static_cast< ZGateWrapper* >( e )->gateConn_ );
		}*/

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const ZGate* p = dynamic_cast<const ZGate *>(proto);
			// if (p)... and so on. 
			return new ZGateWrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
//		MultiReturnConn gateConn_;
		UniConn< gateInConnZGateLookup > gateInConn_;


///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _ZGateWrapper_h
