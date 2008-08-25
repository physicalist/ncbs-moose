/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include <set>
#include "SpikeGen.h"       // for generating spikes
#include <queue>            // for SynChanStruct in BioScan. Remove eventually.
#include "HSolveStruct.h"
#include "BioScan.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"

extern ostream& operator <<( ostream& s, const HinesMatrix& m );

const int HSolveActive::INSTANT_X = 1;
const int HSolveActive::INSTANT_Y = 2;
const int HSolveActive::INSTANT_Z = 4;

HSolveActive::HSolveActive()
{
	// Default lookup table size and boundaries
	vDiv_ = 3000;    // for voltage
	vMin_ = -0.100;
	vMax_ = 0.050;
	caDiv_ = 3000;   // for calcium
	caMin_ = 0.0;
	caMax_ = 1000.0;
}

void HSolveActive::setup( Id seed, double dt ) {
	this->HSolvePassive::setup( seed, dt );
	
	readChannels( );
	readGates( );
	readCalcium( );
	readSynapses( );
	
	createLookupTables( );
	cleanup( );
}

void HSolveActive::solve( ProcInfo info ) {
	if ( !Gk_.size() ) {
		Gk_.resize( channel_.size() );
		GkEk_.resize( channel_.size() );
	}
	
	advanceChannels( info->dt_ );
	calculateChannelCurrents( );
	updateMatrix( );
	HSolvePassive::forwardEliminate( );
	HSolvePassive::backwardSubstitute( );
	advanceCalcium( );
	advanceSynChans( info );
	sendSpikes( info );
}

//////////////////////////////////////////////////////////////////////
// Setup of data structures
//////////////////////////////////////////////////////////////////////

void HSolveActive::readChannels( ) {
	vector< Id >::iterator icompt;
	vector< Id >::iterator ichan;
	int nChannel;
	double Gbar, Ek;
	double X, Y, Z;
	double Xpower, Ypower, Zpower;
	int instant;
	
	for ( icompt = compartmentId_.begin(); icompt != compartmentId_.end(); ++icompt )
	{
		nChannel = BioScan::channels( *icompt, channelId_ );
		
		// todo: discard channels with Gbar = 0.0
		channelCount_.push_back( ( unsigned char ) nChannel );
		
		ichan = channelId_.end() - nChannel;
		for ( ; ichan != channelId_.end(); ++ichan ) {
			channel_.resize( channel_.size() + 1 );
			ChannelStruct& channel = channel_.back();
			
			Eref elm = ( *ichan )();
			get< double >( elm, "Gbar", Gbar );
			get< double >( elm, "Ek", Ek );
			get< double >( elm, "X", X );
			get< double >( elm, "Y", Y );
			get< double >( elm, "Z", Z );
			get< double >( elm, "Xpower", Xpower );
			get< double >( elm, "Ypower", Ypower );
			get< double >( elm, "Zpower", Zpower );
			get< int >( elm, "instant", instant );
			
			channel.Gbar_ = Gbar;
			channel.GbarEk_ = Gbar * Ek;
			channel.setPowers( Xpower, Ypower, Zpower );
			channel.instant_ = instant;
			
			if ( Xpower )
				state_.push_back( X );
			if ( Ypower )
				state_.push_back( Y );
			if ( Zpower )
				state_.push_back( Z );
		}
	}
}

void HSolveActive::readGates( ) {
	vector< Id >::iterator ichan;
	unsigned int nGates;
	int useConcentration;
	for ( ichan = channelId_.begin(); ichan != channelId_.end(); ++ichan ) {
		nGates = BioScan::gates( *ichan, gateId_ );
		gCaDepend_.insert( gCaDepend_.end(), nGates, 0 );
		Eref elm = ( *ichan )();
		get< int >( elm, "useConcentration", useConcentration );
		if ( useConcentration )
			gCaDepend_.back() = 1;
	}
}

void HSolveActive::readCalcium( ) {
	CaConcStruct caConc;
	double Ca, CaBasal, tau, B;
	vector< Id > caConcId;
	vector< int > caTargetIndex;
	vector< int > caDependIndex;
	map< Id, int > caConcIndex;
	int nTarget, nDepend;
	vector< Id >::iterator iconc;
	
	for ( unsigned int ichan = 0; ichan < channel_.size(); ++ichan ) {
		caConcId.clear( );
		
		nTarget = BioScan::caTarget( channelId_[ ichan ], caConcId );
		if ( nTarget == 0 )
			// No calcium pools fed by this channel.
			caTargetIndex.push_back( -1 );
		
		nDepend = BioScan::caDepend( channelId_[ ichan ], caConcId );
		if ( nDepend == 0 )
			// Channel does not depend on calcium.
			caDependIndex.push_back( -1 );
		
		if ( caConcId.size() == 0 )
			continue;
		
		for ( iconc = caConcId.begin(); iconc != caConcId.end(); ++iconc )
			if ( caConcIndex.find( *iconc ) == caConcIndex.end() ) {
				Eref elm = ( *iconc )();
				get< double >( elm, "Ca", Ca );
				get< double >( elm, "CaBasal", CaBasal );
				get< double >( elm, "tau", tau );
				get< double >( elm, "B", B );
				
				caConc.c_ = Ca - CaBasal;
				caConc.factor1_ = 4.0 / ( 2.0 + dt_ / tau ) - 1.0;
				caConc.factor2_ = 2.0 * B * dt_ / ( 2.0 + dt_ / tau );
				caConc.CaBasal_ = CaBasal;
				
				caConc_.push_back( caConc );
				caConcIndex[ *iconc ] = caConc_.size() - 1;
			}
		
		if ( nTarget != 0 )
			caTargetIndex.push_back( caConcIndex[ caConcId.front() ] );
		if ( nDepend != 0 )
			caDependIndex.push_back( caConcIndex[ caConcId.back() ] );
	}
	
	caTarget_.resize( channel_.size() );
	caDepend_.resize( channel_.size() );
	ca_.resize( caConc_.size() );
	caActivation_.resize( caConc_.size() );
	
	for ( unsigned int ichan = 0; ichan < channel_.size(); ++ichan ) {
		if ( caTargetIndex[ ichan ] == -1 )
			caTarget_[ ichan ] = 0;
		else
			caTarget_[ ichan ] = &caActivation_[ caTargetIndex[ ichan ] ];
		
		if ( caDependIndex[ ichan ] == -1 )
			caDepend_[ ichan ] = 0;
		else
			caDepend_[ ichan ] = &ca_[ caDependIndex[ ichan ] ];
	}
}

void HSolveActive::readSynapses( ) {
	vector< Id > spikeId;
	vector< Id > synId;
	vector< Id >::iterator syn;
	vector< Id >::iterator spike;
	SpikeGenStruct spikegen;
	SynChanStruct synchan;
	
	for ( unsigned int ic = 0; ic < nCompt_; ++ic ) {
		/* Request for synchans in current compartment. Also send 'dt', so that
		 * the synchan elements can be initialized.
		 */
		BioScan::synchan( compartmentId_[ ic ], synId );
		for ( syn = synId.begin(); syn != synId.end(); ++syn ) {
			synchan.compt_ = ic;
			synchan.elm_ = ( *syn )();
			BioScan::synchanFields( *syn, synchan );
			synchan_.push_back( synchan );
		}
		
		int nSpike = BioScan::spikegen( compartmentId_[ ic ], spikeId );
		if ( nSpike == 0 )
			continue;
		
		// Very unlikely that there will be >1 spikegens in a compartment,
		// but lets take care of it anyway.
		for ( spike = spikeId.begin(); spike != spikeId.end(); ++spike ) {
			spikegen.compt_ = ic;
			spikegen.elm_ = ( *spike )();
			Eref elm = ( *spike )();
			get< double >( elm, "threshold", spikegen.threshold_ );
			get< double >( elm, "refractT", spikegen.refractT_ );
			get< double >( elm, "state", spikegen.state_ );
			spikegen_.push_back( spikegen );
		}
	}
}

void HSolveActive::createLookupTables( ) {
	std::set< Id > caSet;
	std::set< Id > vSet;
	vector< Id > caGate;
	vector< Id > vGate;
	map< Id, unsigned int > caType;
	map< Id, unsigned int > vType;
	
	for ( unsigned int ig = 0; ig < gateId_.size(); ++ig )
		if ( gCaDepend_[ ig ] )
			caSet.insert( gateId_[ ig ] );
		else
			vSet.insert( gateId_[ ig ] );
	
	caGate.insert( caGate.end(), caSet.begin(), caSet.end() );
	vGate.insert( vGate.end(), vSet.begin(), vSet.end() );
	
	for ( unsigned int ig = 0; ig < caGate.size(); ++ig )
		caType[ caGate[ ig ] ] = ig;
	for ( unsigned int ig = 0; ig < vGate.size(); ++ig )
		vType[ vGate[ ig ] ] = ig;
	
	lookupGroup_.push_back(
		RateLookupGroup( caMin_, caMax_, caDiv_, caGate.size() ) );
	lookupGroup_.push_back(
		RateLookupGroup( vMin_, vMax_, vDiv_, vGate.size() ) );
	RateLookupGroup& caLookupGroup = lookupGroup_[ 0 ];
	RateLookupGroup& vLookupGroup = lookupGroup_[ 1 ];
	
	vector< double > grid;
	vector< double > A, B;
	vector< double >::iterator ia, ib;
	double a, b;
	
	// Calcium-dependent lookup tables
	if ( caGate.size() ) {
		grid.resize( 1 + caDiv_ );
		double dca = ( caMax_ - caMin_ ) / caDiv_;
		for ( int igrid = 0; igrid <= caDiv_; ++igrid )
			grid[ igrid ] = caMin_ + igrid * dca;
	}
	
	for ( unsigned int ig = 0; ig < caGate.size(); ++ig ) {
		BioScan::rates( caGate[ ig ], grid, A, B );
		
		ia = A.begin();
		ib = B.begin();
		for ( unsigned int igrid = 0; igrid < grid.size(); ++igrid ) {
			a = *ia;
			b = *ib;
			
			//~ *ia = ( 2.0 - dt_ * b ) / ( 2.0 + dt_ * b );
			//~ *ib = dt_ * a / ( 1.0 + dt_ * b / 2.0 );
			//~ *ia = dt_ * a;
			//~ *ib = 1.0 + dt_ * b / 2.0;
			++ia, ++ib;
		}
		
		caLookupGroup.addTable( ig, A, B );
	}
	
	// Voltage-dependent lookup tables
	if ( vGate.size() ) {
		grid.resize( 1 + vDiv_ );
		double dv = ( vMax_ - vMin_ ) / vDiv_;
		for ( int igrid = 0; igrid <= vDiv_; ++igrid )
			grid[ igrid ] = vMin_ + igrid * dv;
	}
	
	for ( unsigned int ig = 0; ig < vGate.size(); ++ig ) {
		BioScan::rates( vGate[ ig ], grid, A, B );
		
		ia = A.begin();
		ib = B.begin();
		for ( unsigned int igrid = 0; igrid < grid.size(); ++igrid ) {
			a = *ia;
			b = *ib;
			
			//~ *ia = ( 2.0 - dt_ * b ) / ( 2.0 + dt_ * b );
			//~ *ib = dt_ * a / ( 1.0 + dt_ * b / 2.0 );
			//~ *ia = dt_ * a;
			//~ *ib = 1.0 + dt_ * b / 2.0;
			++ia, ++ib;
		}
		
		vLookupGroup.addTable( ig, A, B );
	}
	
	lookup_.reserve( gateId_.size() );
	for ( unsigned int ig = 0; ig < gateId_.size(); ++ig )
		if ( gCaDepend_[ ig ] )
			lookup_.push_back( caLookupGroup.slice( caType[ gateId_[ ig ] ] ) );
		else
			lookup_.push_back( vLookupGroup.slice( vType[ gateId_[ ig ] ] ) );
}

void HSolveActive::cleanup( ) {
//	compartmentId_.clear( );
}

//////////////////////////////////////////////////////////////////////
// Numerical integration
//////////////////////////////////////////////////////////////////////

void HSolveActive::calculateChannelCurrents( ) {
	vector< ChannelStruct >::iterator ichan;
	vector< double >::iterator igk = Gk_.begin();
	vector< double >::iterator igkek = GkEk_.begin();
	double* istate = &state_[ 0 ];
	
	for ( ichan = channel_.begin(); ichan != channel_.end(); ++ichan ) {
		ichan->process( istate, *igk, *igkek );
		++igk, ++igkek;
	}
}

void HSolveActive::updateMatrix( ) {
	HJ_.assign( HJCopy_.begin(), HJCopy_.end() );
	
	double GkSum, GkEkSum;
	vector< unsigned char >::iterator icco = channelCount_.begin();
	vector< double >::iterator ihs     = HS_.begin();
	vector< double >::iterator iv      = V_.begin();
	vector< double >::iterator igk     = Gk_.begin();
	vector< double >::iterator igkek   = GkEk_.begin();
	vector< CompartmentStruct >::iterator ic;
	for ( ic = compartment_.begin(); ic != compartment_.end(); ++ic ) {
		GkSum   = 0.0;
		GkEkSum = 0.0;
		for ( unsigned char ichan = 0; ichan < *icco; ++ichan ) {
			GkSum   += *( igk++ );
			GkEkSum += *( igkek++ );
		}
		
		*ihs         = *( 2 + ihs ) + GkSum;
		*( 3 + ihs ) = *iv * ic->CmByDt + ic->EmByRm + ic->inject + GkEkSum;
		
		++icco, ihs += 4, ++iv;
	}
	
	vector< SynChanStruct >::iterator isyn;
	for ( isyn = synchan_.begin(); isyn != synchan_.end(); ++isyn ) {
		unsigned ic = isyn->compt_;
		HS_[ 4 * ic ] += isyn->Gk_;
		HS_[ 4 * ic + 3 ] += isyn->Gk_ * isyn->Ek_;
	}
	
	stage_ = 0;    // Update done.
}

void HSolveActive::advanceCalcium( ) {
	unsigned char ichan;
	vector< double* >::iterator icatarget = caTarget_.begin();
	vector< double >::iterator igk = Gk_.begin();
	vector< double >::iterator igkek = GkEk_.begin();
	vector< double >::iterator ivmid = VMid_.begin();
	vector< unsigned char >::iterator icco;
	
	caActivation_.assign( caActivation_.size(), 0.0 );
	
double v;
vector< double >::iterator iv = V_.begin();
	for ( icco = channelCount_.begin(); icco != channelCount_.end(); ++icco ) {
		for ( ichan = 0; ichan < *icco; ++ichan, ++icatarget, ++igk, ++igkek )
		{
			v = 2 * *ivmid - *iv;
			if ( *icatarget )
				**icatarget += *igkek - *igk * v;
				//~ **icatarget += *igkek - *igk * *ivmid;
		}
		++ivmid, ++iv;
	}
	
	vector< CaConcStruct >::iterator icaconc;
	vector< double >::iterator icaactivation = caActivation_.begin();
	vector< double >::iterator ica = ca_.begin();
	for ( icaconc = caConc_.begin(); icaconc != caConc_.end(); ++icaconc ) {
		*ica = icaconc->process( *icaactivation );
		++ica, ++icaactivation;
	}
}

void HSolveActive::advanceChannels( double dt ) {
	vector< double >::iterator iv;
	vector< double >::iterator istate = state_.begin();
	vector< double* >::iterator icadepend = caDepend_.begin();
	vector< RateLookup >::iterator ilookup = lookup_.begin();
	vector< unsigned char >::iterator icco = channelCount_.begin();
	
	LookupKey key;
	LookupKey keyCa;
	double C1, C2;
	vector< ChannelStruct >::iterator ichan = channel_.begin();
	vector< ChannelStruct >::iterator nextChan;
	for ( iv = V_.begin(); iv != V_.end(); ++iv, ++icco ) {
		if ( *icco == 0 )
			continue;
		
		ilookup->getKey( *iv, key );
		nextChan = ichan + *icco;
		for ( ; ichan < nextChan; ++ichan, ++icadepend ) {
			if ( ichan->Xpower_ ) {
				ilookup->rates( key, C1, C2 );
				//~ *istate = *istate * C1 + C2;
				//~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
				if ( ichan->instant_ & INSTANT_X )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
				
				++ilookup, ++istate;
			}
			
			if ( ichan->Ypower_ ) {
				ilookup->rates( key, C1, C2 );
				//~ *istate = *istate * C1 + C2;
				//~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
				if ( ichan->instant_ & INSTANT_Y )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
								
				++ilookup, ++istate;
			}
			
			if ( ichan->Zpower_ ) {
				if ( *icadepend ) {
					ilookup->getKey( **icadepend, keyCa );
					ilookup->rates( keyCa, C1, C2 );
				} else
					ilookup->rates( key, C1, C2 );
				
				//~ *istate = *istate * C1 + C2;
				//~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
				if ( ichan->instant_ & INSTANT_Z )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
				
				++ilookup, ++istate;
			}
		}
	}
}

void HSolveActive::advanceSynChans( ProcInfo info ) {
	vector< SynChanStruct >::iterator isyn;
	for ( isyn = synchan_.begin(); isyn != synchan_.end(); ++isyn )
		isyn->process( info );
}

void HSolveActive::sendSpikes( ProcInfo info ) {
	vector< SpikeGenStruct >::iterator ispike;
	for ( ispike = spikegen_.begin(); ispike != spikegen_.end(); ++ispike ) {
		/* Scope resolution used here to resolve ambiguity between the "set"
		 * function (used here for setting element field values) which belongs
		 * in the global namespace, and the STL "set" container, which is in the
		 * std namespace.
		 */
		::set< double >( ispike->elm_, "Vm", V_[ ispike->compt_ ] );
		SetConn c( ispike->elm_, 0 );
		SpikeGen::processFunc( &c, info );
	}
}
