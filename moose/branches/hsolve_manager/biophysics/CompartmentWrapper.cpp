/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
//~ #include "../randnum/randnum.h"
#include "CompartmentInterface.h"
#include "CompartmentWrapper.h"

/*
 * This Finfo is used to send out Vm to channels, spikegens, etc.
 * 
 * It is exposed here so that HSolve can also use it to send out
 * the Vm to the recipients.
 */
// Static function.
SrcFinfo1< double >* CompartmentWrapper::VmOut() {
	static SrcFinfo1< double > VmOut( "VmOut", 
		"Sends out Vm value of compartment on each timestep" );
	return &VmOut;
}

// Here we send out Vm, but to a different subset of targets. So we
// have to define a new SrcFinfo even though the contents of the msg
// are still only Vm.

static SrcFinfo1< double >* axialOut() {
	static SrcFinfo1< double > axialOut( "axialOut", 
		"Sends out Vm value of compartment to adjacent compartments,"
		"on each timestep" );
	return &axialOut;
}

static SrcFinfo2< double, double >* raxialOut()
{
	static SrcFinfo2< double, double > raxialOut( "raxialOut", 
		"Sends out Raxial information on each timestep, "
		"fields are Ra and Vm" );
	return &raxialOut;
}

/**
 * The initCinfo() function sets up the CompartmentWrapper class.
 * This function uses the common trick of having an internal
 * static value which is created the first time the function is called.
 * There are several static arrays set up here. The ones which
 * use SharedFinfos are for shared messages where multiple kinds
 * of information go along the same connection.
 */
const Cinfo* CompartmentWrapper::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Shared messages
	///////////////////////////////////////////////////////////////////
	static DestFinfo process( "process", 
		"Handles 'process' call",
		new ProcOpFunc< CompartmentWrapper >( &CompartmentWrapper::process ) );
	
	static DestFinfo reinit( "reinit", 
		"Handles 'reinit' call",
		new ProcOpFunc< CompartmentWrapper >( &CompartmentWrapper::reinit ) );
	
	static Finfo* processShared[] =
	{
		&process, &reinit
	};
	
	static SharedFinfo proc( "proc",
		"This is a shared message to receive Process messages "
		"from the scheduler objects. The Process should be called "
		"_second_ in each clock tick, after the Init message."
		"The first entry in the shared msg is a MsgDest "
		"for the Process operation. It has a single argument, "
		"ProcInfo, which holds lots of information about current "
		"time, thread, dt and so on. The second entry is a MsgDest "
		"for the Reinit operation. It also uses ProcInfo. ",
		processShared, sizeof( processShared ) / sizeof( Finfo* )
	);
	///////////////////////////////////////////////////////////////////
	
	static DestFinfo initProc( "initProc", 
		"Handles Process call for the 'init' phase of the CompartmentWrapper "
		"calculations. These occur as a separate Tick cycle from the "
		"regular proc cycle, and should be called before the proc msg.",
		new ProcOpFunc< CompartmentWrapper >( &CompartmentWrapper::initProc ) );
	static DestFinfo initReinit( "initReinit", 
		"Handles Reinit call for the 'init' phase of the CompartmentWrapper "
		"calculations.",
		new ProcOpFunc< CompartmentWrapper >( &CompartmentWrapper::initReinit ) );
	static Finfo* initShared[] =
	{
		&initProc, &initReinit
	};
	
	static SharedFinfo init( "init", 
			"This is a shared message to receive Init messages from "
			"the scheduler objects. Its job is to separate the "
			"compartmental calculations from the message passing. "
			"It doesn't really need to be shared, as it does not use "
			"the reinit part, but the scheduler objects expect this "
			"form of message for all scheduled output. The first "
			"entry is a MsgDest for the Process operation. It has a "
			"single argument, ProcInfo, which holds lots of "
			"information about current time, thread, dt and so on. "
			"The second entry is a dummy MsgDest for the Reinit "
			"operation. It also uses ProcInfo. ",
		initShared, sizeof( initShared ) / sizeof( Finfo* )
	);
	
	///////////////////////////////////////////////////////////////////
	
	static DestFinfo handleChannel( "handleChannel", 
		"Handles conductance and Reversal potential arguments from Channel",
		new EpFunc2< CompartmentWrapper, double, double >( &CompartmentWrapper::handleChannel ) );
	// VmOut is declared above as it needs to be in scope for later funcs.
	
	static Finfo* channelShared[] =
	{
		&handleChannel, VmOut()
	};
	static SharedFinfo channel( "channel", 
			"This is a shared message from a compartment to channels. "
			"The first entry is a MsgDest for the info coming from "
			"the channel. It expects Gk and Ek from the channel "
			"as args. The second entry is a MsgSrc sending Vm ",
		channelShared, sizeof( channelShared ) / sizeof( Finfo* )
	);
	///////////////////////////////////////////////////////////////////
	// axialOut declared above as it is needed in file scope
	static DestFinfo handleRaxial( "handleRaxial", 
		"Handles Raxial info: arguments are Ra and Vm.",
		new EpFunc2< CompartmentWrapper, double, double >( 
			&CompartmentWrapper::handleRaxial )
	);

	static Finfo* axialShared[] =
	{
		axialOut(), &handleRaxial
	};
	static SharedFinfo axial( "axial", 
			"This is a shared message between asymmetric compartments. "
			"axial messages (this kind) connect up to raxial "
			"messages (defined below). The soma should use raxial "
			"messages to connect to the axial message of all the "
			"immediately adjacent dendritic compartments.This puts "
			"the (low) somatic resistance in series with these "
			"dendrites. Dendrites should then use raxial messages to"
			"connect on to more distal dendrites. In other words, "
			"raxial messages should face outward from the soma. "
			"The first entry is a MsgSrc sending Vm to the axialFunc"
			"of the target compartment. The second entry is a MsgDest "
			"for the info coming from the other compt. It expects "
			"Ra and Vm from the other compt as args. Note that the "
			"message is named after the source type. ",
		axialShared, sizeof( axialShared ) / sizeof( Finfo* )
	);
	
	///////////////////////////////////////////////////////////////////
	static DestFinfo handleAxial( "handleAxial", 
		"Handles Axial information. Argument is just Vm.",
		new EpFunc1< CompartmentWrapper, double >( &CompartmentWrapper::handleAxial ) );
	// rxialOut declared above as it is needed in file scope
	static Finfo* raxialShared[] =
	{
		&handleAxial, raxialOut()
	};
	static SharedFinfo raxial( "raxial", 
			"This is a raxial shared message between asymmetric "
			"compartments. The first entry is a MsgDest for the info "
			"coming from the other compt. It expects Vm from the "
			"other compt as an arg. The second is a MsgSrc sending "
			"Ra and Vm to the raxialFunc of the target compartment. ",
			raxialShared, sizeof( raxialShared ) / sizeof( Finfo* )
	);
	///////////////////////////////////////////////////////////////////
	// Value Finfos.
	///////////////////////////////////////////////////////////////////
	
		static ElementValueFinfo< CompartmentWrapper, double > Vm( "Vm", 
			"membrane potential",
			&CompartmentWrapper::setVm,
			&CompartmentWrapper::getVm
		);
		static ElementValueFinfo< CompartmentWrapper, double > Cm( "Cm", 
			"Membrane capacitance",
			&CompartmentWrapper::setCm,
			&CompartmentWrapper::getCm
		);
		static ElementValueFinfo< CompartmentWrapper, double > Em( "Em", 
			"Resting membrane potential",
			&CompartmentWrapper::setEm,
			&CompartmentWrapper::getEm
		);
		static ReadOnlyElementValueFinfo< CompartmentWrapper, double > Im( "Im", 
			"Current going through membrane",
			&CompartmentWrapper::getIm
		);
		static ElementValueFinfo< CompartmentWrapper, double > inject( "inject", 
			"Current injection to deliver into compartment",
			&CompartmentWrapper::setInject,
			&CompartmentWrapper::getInject
		);
		static ElementValueFinfo< CompartmentWrapper, double > initVm( "initVm", 
			"Initial value for membrane potential",
			&CompartmentWrapper::setInitVm,
			&CompartmentWrapper::getInitVm
		);
		static ElementValueFinfo< CompartmentWrapper, double > Rm( "Rm", 
			"Membrane resistance",
			&CompartmentWrapper::setRm,
			&CompartmentWrapper::getRm
		);
		static ElementValueFinfo< CompartmentWrapper, double > Ra( "Ra", 
			"Axial resistance of compartment",
			&CompartmentWrapper::setRa,
			&CompartmentWrapper::getRa
		);
		static ElementValueFinfo< CompartmentWrapper, double > diameter( "diameter", 
			"Diameter of compartment",
			&CompartmentWrapper::setDiameter,
			&CompartmentWrapper::getDiameter
		);
		static ElementValueFinfo< CompartmentWrapper, double > length( "length", 
			"Length of compartment",
			&CompartmentWrapper::setLength,
			&CompartmentWrapper::getLength
		);
		static ElementValueFinfo< CompartmentWrapper, double > x0( "x0", 
			"X coordinate of start of compartment",
			&CompartmentWrapper::setX0,
			&CompartmentWrapper::getX0
		);
		static ElementValueFinfo< CompartmentWrapper, double > y0( "y0", 
			"Y coordinate of start of compartment",
			&CompartmentWrapper::setY0,
			&CompartmentWrapper::getY0
		);
		static ElementValueFinfo< CompartmentWrapper, double > z0( "z0", 
			"Z coordinate of start of compartment",
			&CompartmentWrapper::setZ0,
			&CompartmentWrapper::getZ0
		);
		static ElementValueFinfo< CompartmentWrapper, double > x( "x",
			"x coordinate of end of compartment",
			&CompartmentWrapper::setX,
			&CompartmentWrapper::getX
		);
		static ElementValueFinfo< CompartmentWrapper, double > y( "y",
			"y coordinate of end of compartment",
			&CompartmentWrapper::setY,
			&CompartmentWrapper::getY
		);
		static ElementValueFinfo< CompartmentWrapper, double > z( "z", 
			"z coordinate of end of compartment",
			&CompartmentWrapper::setZ,
			&CompartmentWrapper::getZ
		);
	
	//////////////////////////////////////////////////////////////////
	// DestFinfo definitions
	//////////////////////////////////////////////////////////////////
		static DestFinfo injectMsg( "injectMsg", 
			"The injectMsg corresponds to the INJECT message in the "
			"GENESIS compartment. Unlike the 'inject' field, any value "
			"assigned by handleInject applies only for a single timestep."
			"So it needs to be updated every dt for a steady (or varying)"
			"injection current",
			new EpFunc1< CompartmentWrapper,  double >( &CompartmentWrapper::injectMsg )
		);
		
		static DestFinfo randInject( "randInject",
			"Sends a random injection current to the compartment. Must be"
			"updated each timestep."
			"Arguments to randInject are probability and current.",
			new EpFunc2< CompartmentWrapper, double, double > (
				&CompartmentWrapper::randInject ) );
	
		static DestFinfo cable( "cable", 
			"Message for organizing compartments into groups, called"
			"cables. Doesn't do anything.",
			new OpFunc0< CompartmentWrapper >( &CompartmentWrapper::cable )
		);
	///////////////////////////////////////////////////////////////////
	static Finfo* compartmentFinfos[] = 
	{
		&Vm,				// Value
		&Cm,				// Value
		&Em,				// Value
		&Im,				// ReadOnlyValue
		&inject,			// Value
		&initVm,			// Value
		&Rm,				// Value
		&Ra,				// Value
		&diameter,			// Value
		&length,			// Value
		&x0,				// Value
		&y0,				// Value
		&z0,				// Value
		&x,					// Value
		&y,					// Value
		&z,					// Value
		&injectMsg,			// DestFinfo
		&randInject,		// DestFinfo
		&injectMsg,			// DestFinfo
		&cable,				// DestFinfo
		&proc,				// SharedFinfo
		&init,				// SharedFinfo
		&channel,			// SharedFinfo
		&axial,				// SharedFinfo
		&raxial				// SharedFinfo
	};
	
	static string doc[] =
	{
		"Name", "Compartment",
		"Author", "Upi Bhalla",
		"Description", "Compartment object, for branching neuron models.",
	};	
	static Cinfo compartmentCinfo(
				"Compartment",
				Neutral::initCinfo(),
				compartmentFinfos,
				sizeof( compartmentFinfos ) / sizeof( Finfo* ),
				new Dinfo< CompartmentWrapper >(),
				doc,
				sizeof(doc)/sizeof(string)
	);
	
	return &compartmentCinfo;
}

static const Cinfo* compartmentCinfo = CompartmentWrapper::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the CompartmentWrapper class functions.
//////////////////////////////////////////////////////////////////

//~ CompartmentWrapper::CompartmentWrapper()
//~ {
	//~ Vm_ = -0.06;
	//~ Em_ = -0.06;
	//~ Cm_ = 1.0;
	//~ Rm_ = 1.0;
	//~ invRm_ = 1.0;
	//~ Ra_ = 1.0;
	//~ Im_ = 0.0;
	//~ lastIm_ = 0.0;
	//~ Inject_ = 0.0;
	//~ sumInject_ = 0.0;
	//~ initVm_ = -0.06;
	//~ A_ = 0.0;
	//~ B_ = 0.0;
	//~ x_ = 0.0;
	//~ y_ = 0.0;
	//~ z_ = 0.0;
	//~ x0_ = 0.0;
	//~ y0_ = 0.0;
	//~ z0_ = 0.0;
	//~ diameter_ = 0.0;
	//~ length_ = 0.0;
//~ }
//~ 
//~ CompartmentWrapper::~CompartmentWrapper()
//~ {
	//~ ;
//~ }

//~ bool CompartmentWrapper::rangeWarning( const string& field, double value )
//~ {
	//~ if ( value < CompartmentWrapper::EPSILON ) {
		//~ cout << "Warning: Ignored attempt to set " << field <<
				//~ " of compartment " <<
				//~ // c->target().e->name() << 
				//~ " to less than " << EPSILON << endl;
		//~ return 1;
	//~ }
	//~ return 0;
//~ }

// Value Field access function definitions.
void CompartmentWrapper::setVm( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setVm( e, q, value );
}

double CompartmentWrapper::getVm( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getVm( e, q );
}

void CompartmentWrapper::setEm( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setEm( e, q, value );
}

double CompartmentWrapper::getEm( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getEm( e, q );
}

void CompartmentWrapper::setCm( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setCm( e, q, value );
}

double CompartmentWrapper::getCm( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getCm( e, q );
}

void CompartmentWrapper::setRm( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setRm( e, q, value );
}

double CompartmentWrapper::getRm( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getRm( e, q );
}

void CompartmentWrapper::setRa( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setRa( e, q, value );
}

double CompartmentWrapper::getRa( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getRa( e, q );
}

//~ void CompartmentWrapper::setIm( const Eref& e, const Qinfo* q, double value )
//~ {
	//~ compartment_->setIm( e, q, value );
//~ }

double CompartmentWrapper::getIm( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getIm( e, q );
}

void CompartmentWrapper::setInject( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setInject( e, q, value );
}

double CompartmentWrapper::getInject( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getInject( e, q );
}

void CompartmentWrapper::setInitVm( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setInitVm( e, q, value );
}

double CompartmentWrapper::getInitVm( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getInitVm( e, q );
}

void CompartmentWrapper::setDiameter( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setDiameter( e, q, value );
}

double CompartmentWrapper::getDiameter( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getDiameter( e, q );
}

void CompartmentWrapper::setLength( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setLength( e, q, value );
}

double CompartmentWrapper::getLength( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getLength( e, q );
}

void CompartmentWrapper::setX0( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setX0( e, q, value );
}

double CompartmentWrapper::getX0( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getX0( e, q );
}

void CompartmentWrapper::setY0( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setY0( e, q, value );
}

double CompartmentWrapper::getY0( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getY0( e, q );
}

void CompartmentWrapper::setZ0( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setZ0( e, q, value );
}

double CompartmentWrapper::getZ0( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getZ0( e, q );
}

void CompartmentWrapper::setX( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setX( e, q, value );
}

double CompartmentWrapper::getX( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getX( e, q );
}

void CompartmentWrapper::setY( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setY( e, q, value );
}

double CompartmentWrapper::getY( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getY( e, q );
}

void CompartmentWrapper::setZ( const Eref& e, const Qinfo* q, double value )
{
	compartment_->setZ( e, q, value );
}

double CompartmentWrapper::getZ( const Eref& e, const Qinfo* q ) const
{
	return compartment_->getZ( e, q );
}

//////////////////////////////////////////////////////////////////
// CompartmentWrapper::Dest function definitions.
//////////////////////////////////////////////////////////////////

void CompartmentWrapper::process( const Eref& e, ProcPtr p )
{
}

void CompartmentWrapper::reinit( const Eref& e, ProcPtr p )
{
	//~ this->innerReinit( e, p );
}

//~ void CompartmentWrapper::innerReinit(  const Eref& e, ProcPtr p )
//~ {
//~ }

void CompartmentWrapper::initProc( const Eref& e, ProcPtr p )
{
	//~ // Separate variants for regular and SymCompartmentWrapper
	//~ this->innerInitProc( e, p ); 
}

//~ void CompartmentWrapper::innerInitProc( const Eref& e, ProcPtr p )
//~ {
//~ }

void CompartmentWrapper::initReinit( const Eref& e, ProcPtr p )
{
	//~ this->innerInitReinit( e, p );
}

//~ void CompartmentWrapper::innerInitReinit( const Eref& e, ProcPtr p )
//~ {
//~ }

void CompartmentWrapper::handleChannel(
	const Eref& e, const Qinfo* q, double Gk, double Ek )
{
}

void CompartmentWrapper::handleRaxial(
	const Eref& e, const Qinfo* q, double Ra, double Vm )
{
}

void CompartmentWrapper::handleAxial(
	const Eref& e, const Qinfo* q, double Vm )
{
}

void CompartmentWrapper::injectMsg(
	const Eref& e, const Qinfo* q, double current )
{
}

void CompartmentWrapper::randInject(
	const Eref& e, const Qinfo* q, double prob, double current )
{
	//~ if ( mtrand() < prob * dt_ ) {
		//~ sumInject_ += current;
		//~ Im_ += current;
	//~ }
}

//~ void CompartmentWrapper::cable()
//~ {
	//~ ;
//~ }

/////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

//~ void testCompartment()
//~ { ; }

//~ void testCompartmentWrapper()
//~ {
	//~ const Cinfo* comptCinfo = CompartmentWrapper::initCinfo();
	//~ Id comptId = Id::nextId();
	//~ vector< DimInfo > dims;
	//~ Element* n = new Element( comptId, comptCinfo, "compt", dims, 1, true );
	//~ assert( n != 0 );
	//~ Eref compter( n, 0 );
	//~ CompartmentWrapper* c = reinterpret_cast< CompartmentWrapper* >( compter.data() );
	//~ ProcInfo p;
	//~ p.dt = 0.002;
	//~ c->setInject( 1.0 );
	//~ c->setRm( 1.0 );
	//~ c->setRa( 0.0025 );
	//~ c->setCm( 1.0 );
	//~ c->setEm( 0.0 );
	//~ c->setVm( 0.0 );
	//~ 
	//~ // First, test charging curve for a single compartment
	//~ // We want our charging curve to be a nice simple exponential
	//~ // Vm = 1.0 - 1.0 * exp( - t / 1.0 );
	//~ double delta = 0.0;
	//~ double Vm = 0.0;
	//~ double tau = 1.0;
	//~ double Vmax = 1.0;
	//~ for ( p.currTime = 0.0; p.currTime < 2.0; p.currTime += p.dt ) 
	//~ {
		//~ Vm = c->getVm();
		//~ double x = Vmax - Vmax * exp( -p.currTime / tau );
		//~ delta += ( Vm - x ) * ( Vm - x );
		//~ c->process( compter, &p );
	//~ }
	//~ assert( delta < 1.0e-6 );
	//~ 
	//~ comptId.destroy();
	//~ cout << "." << flush;
//~ }
//~ 
//~ // Comment out this define if it takes too long (about 5 seconds on
//~ // a modest machine, but could be much longer with valgrind)
//~ #define DO_SPATIAL_TESTS
//~ /**
 //~ * Function to test the spatial spread of charge.
 //~ * We make the cable long enough to get another nice exponential.
 //~ * Vm = Vm0 * exp( -x/lambda)
 //~ * lambda = sqrt( Rm/Ra ) where these are the actual values, not
 //~ * the values per unit length.
 //~ * So here lambda = 20, so that each compt is lambda/20
 //~ */
//~ #include "../shell/Shell.h"
//~ void testCompartmentWrapperProcess()
//~ {
	//~ Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	//~ unsigned int size = 100;
	//~ vector< int > dims( 1, size );
	//~ 
	//~ double Rm = 1.0;
	//~ double Ra = 0.01;
	//~ double Cm = 1.0;
	//~ double dt = 0.01;
	//~ double runtime = 10;
	//~ double lambda = sqrt( Rm / Ra );
	//~ 
	//~ Id cid = shell->doCreate( "Compartment", Id(), "compt", dims );
	//~ assert( cid != Id() );
	//~ assert( cid()->dataHandler()->totalEntries() == size );
	//~ 
	//~ bool ret = Field< double >::setRepeat( cid, "initVm", 0.0 );
	//~ assert( ret );
	//~ Field< double >::setRepeat( cid, "inject", 0 );
	//~ // Only apply current injection in first compartment
	//~ Field< double >::set( ObjId( cid, 0 ), "inject", 1.0 ); 
	//~ Field< double >::setRepeat( cid, "Rm", Rm );
	//~ Field< double >::setRepeat( cid, "Ra", Ra );
	//~ Field< double >::setRepeat( cid, "Cm", Cm );
	//~ Field< double >::setRepeat( cid, "Em", 0 );
	//~ Field< double >::setRepeat( cid, "Vm", 0 );
	//~ 
	//~ // The diagonal message has a default stride of 1, so it connects
	//~ // successive compartments.
	//~ // Note that the src and dest elements here are identical, so we cannot
	//~ // use a shared message. The messaging system will get confused about
	//~ // direction to send data. So we split up the shared message that we
	//~ // might have used, below, into two individual messages.
	//~ // MsgId mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "raxial", ObjId( cid ), "axial" );
	//~ MsgId mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "axialOut", ObjId( cid ), "handleAxial" );
	//~ assert( mid != Msg::bad);
	//~ // mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "handleRaxial", ObjId( cid ), "raxialOut" );
	//~ mid = shell->doAddMsg( "Diagonal", ObjId( cid ), "raxialOut", ObjId( cid ), "handleRaxial" );
	//~ assert( mid != Msg::bad );
	//~ ObjId managerId = Msg::getMsg( mid )->manager().objId();
	//~ // Make the raxial data go from high to lower index compartments.
	//~ Field< int >::set( managerId, "stride", -1 );
	//~ 
//~ #ifdef DO_SPATIAL_TESTS
	//~ shell->doSetClock( 0, dt );
	//~ shell->doSetClock( 1, dt );
	//~ shell->doUseClock( "/compt", "init", 0 );
	//~ shell->doUseClock( "/compt", "process", 1 );
	//~ 
	//~ shell->doReinit();
	//~ shell->doStart( runtime );
	//~ 
	//~ double Vmax = Field< double >::get( ObjId( cid, 0 ), "Vm" );
	//~ 
	//~ double delta = 0.0;
	//~ // We measure only the first 50 compartments as later we 
	//~ // run into end effects because it is not an infinite cable
	//~ for ( unsigned int i = 0; i < 50; i++ ) {
		//~ double Vm = Field< double >::get( ObjId( cid, i ), "Vm" );
		//~ double x = Vmax * exp( - static_cast< double >( i ) / lambda );
		//~ delta += ( Vm - x ) * ( Vm - x );
	 	//~ // cout << i << " (x, Vm) = ( " << x << ", " << Vm << " )\n";
	//~ }
	//~ assert( delta < 1.0e-5 );
//~ #endif // DO_SPATIAL_TESTS
	//~ shell->doDelete( cid );
	//~ cout << "." << flush;
//~ }

#endif // DO_UNIT_TESTS
