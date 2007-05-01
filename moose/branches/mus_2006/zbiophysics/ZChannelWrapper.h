/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZChannelWrapper_h
#define _ZChannelWrapper_h
class ZChannelWrapper: 
	public ZChannel, public Neutral
{
	friend Element* channelConnZChannelLookup( const Conn* );
	friend Element* xGateOutConnZChannelLookup( const Conn* );
	friend Element* yGateOutConnZChannelLookup( const Conn* );
	friend Element* zGateOutConnZChannelLookup( const Conn* );
/*	friend Element* xGateConnZChannelLookup( const Conn* );
	friend Element* yGateConnZChannelLookup( const Conn* );
	friend Element* zGateConnZChannelLookup( const Conn* );*/
	friend Element* reinitInConnZChannelLookup( const Conn* );
    public:
		ZChannelWrapper(const string& n)
		:
			Neutral( n ),
			channelSrc_( &channelConn_ ),
			xGateSrc_( &xGateOutConn_ ),
			yGateSrc_( &yGateOutConn_ ),
			zGateSrc_( &zGateOutConn_ )
/*			xGateSrc_( &xGateConn_ ),
			yGateSrc_( &yGateConn_ ),
			zGateSrc_( &zGateConn_ ),
			xGateReinitSrc_( &xGateConn_ ),
			yGateReinitSrc_( &yGateConn_ ),
			zGateReinitSrc_( &zGateConn_ )*/
		{
			;
		}
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static void setGbar( Conn* c, double value ) {
			static_cast< ZChannelWrapper* >( c->parent() )->Gbar_ = value;
		}
		static double getGbar( const Element* e ) {
			return static_cast< const ZChannelWrapper* >( e )->Gbar_;
		}
		static void setEk( Conn* c, double value ) {
			static_cast< ZChannelWrapper* >( c->parent() )->Ek_ = value;
		}
		static double getEk( const Element* e ) {
			return static_cast< const ZChannelWrapper* >( e )->Ek_;
		}
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
		static SingleMsgSrc* getChannelSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->channelSrc_ );
		}

		static SingleMsgSrc* getXGateSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->xGateSrc_ );
		}

		static SingleMsgSrc* getYGateSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->yGateSrc_ );
		}

		static SingleMsgSrc* getZGateSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->zGateSrc_ );
		}

/*		static SingleMsgSrc* getXGateReinitSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->xGateReinitSrc_ );
		}

		static SingleMsgSrc* getYGateReinitSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->yGateReinitSrc_ );
		}

		static SingleMsgSrc* getZGateReinitSrc( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->zGateReinitSrc_ );
		}*/

///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////
		void channelFuncLocal( double Vm, ProcInfo info );
		static void channelFunc( Conn* c, double Vm, ProcInfo info ) {
			static_cast< ZChannelWrapper* >( c->parent() )->
				channelFuncLocal( Vm, info );
		}

		void reinitFuncLocal( double Vm );
		static void reinitFunc( Conn* c, double Vm ) {
			static_cast< ZChannelWrapper* >( c->parent() )->
				reinitFuncLocal( Vm );
		}

/*		void xGateFuncLocal( double X, double gScale ) {
		}
		static void xGateFunc( Conn* c, double X, double gScale ) {
			static_cast< ZChannelWrapper* >( c->parent() )->
				xGateFuncLocal( X, gScale );
		}

		void yGateFuncLocal( double Y, double gScale ) {
		}
		static void yGateFunc( Conn* c, double Y, double gScale ) {
			static_cast< ZChannelWrapper* >( c->parent() )->
				yGateFuncLocal( Y, gScale );
		}

		void zGateFuncLocal( double Z, double gScale ) {
		}
		static void zGateFunc( Conn* c, double Z, double gScale ) {
			static_cast< ZChannelWrapper* >( c->parent() )->
				zGateFuncLocal( Z, gScale );
		}*/


///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////
		static Conn* getChannelConn( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->channelConn_ );
		}
		static Conn* getXGateOutConn( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->xGateOutConn_ );
		}
		static Conn* getYGateOutConn( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->yGateOutConn_ );
		}
		static Conn* getZGateOutConn( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->zGateOutConn_ );
		}
		static Conn* getReinitInConn( Element* e ) {
			return &( static_cast< ZChannelWrapper* >( e )->reinitInConn_ );
		}

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto );

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////
		SingleMsgSrc2< double, double > channelSrc_;
		SingleMsgSrc0 xGateSrc_;
		SingleMsgSrc0 yGateSrc_;
		SingleMsgSrc0 zGateSrc_;
		UniConn< xGateOutConnZChannelLookup > xGateOutConn_;
		UniConn< yGateOutConnZChannelLookup > yGateOutConn_;
		UniConn< zGateOutConnZChannelLookup > zGateOutConn_;
/*		SingleMsgSrc3< double, double, double > xGateSrc_;
		SingleMsgSrc3< double, double, double > yGateSrc_;
		SingleMsgSrc3< double, double, double > zGateSrc_;
		SingleMsgSrc3< double, double, int > xGateReinitSrc_;
		SingleMsgSrc3< double, double, int > yGateReinitSrc_;
		SingleMsgSrc3< double, double, int > zGateReinitSrc_;*/
		UniConn< channelConnZChannelLookup > channelConn_;
/*		UniConn< xGateConnZChannelLookup > xGateConn_;
		UniConn< yGateConnZChannelLookup > yGateConn_;
		UniConn< zGateConnZChannelLookup > zGateConn_;*/
		UniConn< reinitInConnZChannelLookup > reinitInConn_;

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _ZChannelWrapper_h
