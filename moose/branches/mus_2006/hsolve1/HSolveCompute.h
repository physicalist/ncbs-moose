/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSolve_h
#define _HSolve_h

#include "GateInfo.h"
#include "Node.h"
#include "HSolveModel.h"
#include "header.h"
#include <stack>
#include <set>
#include <cmath>
#include <vector>

// This macro is used for linear interpolation during table lookup of rate
// constants. Will not be included in coming versions.
#define WT_AVG( A, B, FRACTION ) \
        ( ( A ) * ( 1.0 - ( FRACTION ) ) + ( B ) * ( FRACTION ) )

template < class T >
class HSolveCompute
{
public:
	void initialize( HSolveModel< T >* model, const T& seed,
		double dt, int NDiv,
		double VLo, double VHi )
	{
		model_  = model;
		dt_     = dt;
		NDiv_   = NDiv;
		VLo_    = VLo;
		VHi_    = VHi;
		dV_     = ( VHi_ - VLo_ ) / NDiv_;
		
		constructTree( seed );
		constructMatrix( );
		constructChannelDatabase( );
		constructLookupTables( );
		concludeInit( );
	}

	void step( ) {
		updateMatrix( );
		forwardEliminate( );
		backwardSubstitute( );
		advanceChannels( );
	}

private:
	void constructTree( const T& seedIn ) {
		T parent, seed = seedIn;
		vector< T > child = model_->neighbours( seed );
		if ( child.size() != 1 )
			while ( !child.empty() ) {
				parent = seed;
				seed   = child[ 0 ];
				child  = model_->children( seed, parent );
			}
		
		unsigned long label = 0;
		stack< Node< T > > nodeStack;
		Node< T > currentNode;
		currentNode.self   = seed;
		currentNode.child  = model_->neighbours( currentNode.self );
		currentNode.state  = 0;
		currentNode.label  = label;
		nodeStack.push( currentNode );
		compartment_.push_back( currentNode.self );
		
		while ( !nodeStack.empty() )
			if ( currentNode.state < currentNode.child.size() ) {
				++label;
				
				if ( currentNode.state >= 1 ) {
					checkpoint_.push_back( currentNode.label );
					checkpoint_.push_back( label );
				}
				
				currentNode.parent = currentNode.self;
				currentNode.self   = currentNode.child[
				                         currentNode.state ];
				currentNode.child  = model_->children(
				                         currentNode.self,
				                         currentNode.parent );
				currentNode.state  = 0;
				currentNode.label  = label;
				nodeStack.push( currentNode );
				compartment_.push_back( currentNode.self );
			} else {
				nodeStack.pop();
				if( !nodeStack.empty() ) {
					++( nodeStack.top().state );
					currentNode = nodeStack.top();
				}
			}
		
		N_ = compartment_.size( );
		reverse( compartment_.begin(), compartment_.end() );
		reverse( checkpoint_.begin(), checkpoint_.end() );
		for ( unsigned long icp = 0; icp < checkpoint_.size(); ++icp )
			checkpoint_[ icp ] = ( N_ - 1 ) - checkpoint_[ icp ];
	}

	void constructMatrix( ) {
		A_.resize( 5 * N_ );
		double  Vm, Em, Cm, Rm,
		        Ra, Ra_parent,
		        R_inv, lambda, inject;
		
		unsigned long ia = 0, ic = 0;
		unsigned long checkpoint, parentIndex;
		for ( unsigned long icp = 0; icp < 1 + checkpoint_.size(); ++icp ) {
			checkpoint = icp < checkpoint_.size() ?
			             checkpoint_[ icp ] :
			             N_ - 1;
			
			for ( ; ic < 1 + checkpoint; ++ic, ia += 5 ) {
				model_->getFieldDouble( compartment_[ ic ], "Vm", Vm );
				model_->getFieldDouble( compartment_[ ic ], "Vm", Vm );
				model_->getFieldDouble( compartment_[ ic ], "Em", Em );
				model_->getFieldDouble( compartment_[ ic ], "Cm", Cm );
				model_->getFieldDouble( compartment_[ ic ], "Rm", Rm );
				model_->getFieldDouble( compartment_[ ic ], "Ra", Ra );
				model_->getFieldDouble( compartment_[ ic ], "Inject", inject );
				
				if ( ic < N_ - 1 ) {
					parentIndex = ic < checkpoint ?
					              1 + ic :
					              checkpoint_[ ++icp ];
					model_->getFieldDouble(
						compartment_[ parentIndex ],
						"Ra", Ra_parent );
					R_inv = 2.0 / ( Ra + Ra_parent );
					A_[ 5 * parentIndex + 3 ] += R_inv;
				} else
					R_inv = 0.0;
				
				lambda_.push_back( lambda = 2.0 * Cm / dt_ );
				iLeak_.push_back( Em / Rm );
				inject_.push_back( inject );
				V_.push_back( Vm );
				
				A_[ 1 + ia ]  = -R_inv;
				A_[ 2 + ia ]  = R_inv * R_inv;
				A_[ 3 + ia ] += R_inv + 1.0 / Rm + lambda;
			}
		}
	}

	void constructChannelDatabase( ) {
		vector< T > channel, gate;
		double Gbar, Ek;
		
		for ( unsigned long ic = 0; ic < N_; ++ic ) {
			channel = model_->channels( compartment_[ ic ] );
			channelCount_.push_back( ( unsigned char )( channel.size( ) ) );
			gateCount_.push_back( '\0' );
			
			for ( unsigned long ichan = 0; ichan < channel.size(); ++ichan ) {
				model_->getFieldDouble( channel[ ichan ], "Gbar", Gbar );
				model_->getFieldDouble( channel[ ichan ], "Ek", Ek );
				Gbar_.push_back( Gbar );
				GbarEk_.push_back( Gbar * Ek );
				
				gate = model_->gates( channel[ ichan ] );
				gateCount_.back() += gate.size();
				gateCount1_.push_back( gate.size() );
				
				for ( unsigned long igate = 0; igate < gate.size(); ++igate ) {
					gateInfo_.push_back (
						GateInfo< T >( model_, gate[ igate ] )
					);
					state_.push_back( gateInfo_.back().state() );
					power_.push_back( gateInfo_.back().power() );
				}
			}
		}
	}

	void constructLookupTables( ) {
		set< GateInfo< T > > family( gateInfo_.begin(), gateInfo_.end() );
		typename set< GateInfo< T > >::iterator ifamily;
		lookupBlocSize_ = 2 * family.size();
		
		typename vector< GateInfo< T > >::iterator ig;
		for ( ig = gateInfo_.begin(); ig != gateInfo_.end(); ++ig )
			gateFamily_.push_back (
				static_cast< unsigned char > (
				/*!!!*/	distance( family.begin(),
						family.find( *ig ) ) ) );
		
		double alpha, beta, V;
		lookup_.resize( NDiv_ * lookupBlocSize_ );
		vector< double >::iterator il = lookup_.begin();
		for ( int igrid = 0; igrid < NDiv_; ++igrid ) {
			V = VLo_ + igrid * dV_;
			for ( ifamily = family.begin(); ifamily != family.end(); ++ifamily ) {
				alpha = ifamily->alpha( V );
				beta  = ifamily->beta( V );
				
				/* Refine wrt roundoff error */
				*( il++ ) = ( 2.0 - dt_ * ( alpha + beta ) )
				            / ( 2.0 + dt_ * ( alpha + beta ) );
				*( il++ ) = dt_ * alpha / ( 1.0 + dt_
				            * ( alpha + beta ) / 2.0 );
			}
		}
	}

	void concludeInit( ) {
	//	compartment_.clear( );
	//	gateInfo_.clear( );
	}

	// This function needs cleanup. Many immediate optimizations come in here */
	void updateMatrix( ) {
		double Gk, GkEk,
		       GkSum, GkEkSum;
		double state;
		
		unsigned char ichan;
		unsigned char igate;
		vector< unsigned char >::iterator icco = channelCount_.begin();
		vector< unsigned char >::iterator igco = gateCount1_.begin();
		vector< double >::iterator igbar   = Gbar_.begin();
		vector< double >::iterator igbarek = GbarEk_.begin();
		vector< double >::iterator ipower  = power_.begin();
		vector< double >::iterator istate  = state_.begin();
		vector< double >::iterator ia      = A_.begin();
		vector< double >::iterator iv      = V_.begin();
		vector< double >::iterator ilambda = lambda_.begin();
		vector< double >::iterator iileak  = iLeak_.begin();
		vector< double >::iterator iinject = inject_.begin();
		for ( unsigned long ic = 0; ic < N_; ++ic ) {
			GkSum = GkEkSum = 0.0;
			for ( ichan = 0; ichan < *icco; ++ichan, ++igco ) {
				Gk   = *( igbar++ );
				GkEk = *( igbarek++ );
				for ( igate = 0; igate < *igco; ++igate ) {
					state = pow( *( istate++ ), *( ipower++ ) );
					Gk   *= state;
					GkEk *= state;
				}
				GkSum   += Gk;
				GkEkSum += GkEk;
			}
			
			*ia         = *( 3 + ia ) + GkSum;
			*( 4 + ia ) = *iileak + *ilambda * *iv + GkEkSum + *iinject;
			++icco, ia += 5, ++ilambda, ++iv, ++iileak, ++iinject;
		}
	}

	void forwardEliminate( ) {
		vector< double >::iterator ia = A_.begin();
		vector< unsigned long >::iterator icp;
		unsigned long ic = 0;
		for ( icp = checkpoint_.begin(); icp != checkpoint_.end();
		      ++icp, ++ic, ia += 5 ) {
			for ( ; ic < *icp; ++ic, ia += 5 ) {
				*( 5 + ia ) -= *( 2 + ia ) / *ia;
				*( 9 + ia ) -= *( 1 + ia ) * *( 4 + ia ) / *ia;
			}
			A_[ 5 * *++icp ]    -= *( 2 + ia ) / *ia;
			A_[ 4 + 5 * *icp ]  -= *( 1 + ia ) * *( 4 + ia ) / *ia;
		}
		
		for ( ; ic < N_ - 1; ++ic, ia += 5 ) {
			*( 5 + ia ) -= *( 2 + ia ) / *ia;
			*( 9 + ia ) -= *( 1 + ia ) * *( 4 + ia ) / *ia;
		}
	}

	void backwardSubstitute( ) {
		static vector< double > VMid;
		if ( VMid.empty( ) )
			VMid.resize( N_ );
		
		long ic = ( long )( N_ ) - 1;
		vector< double >::reverse_iterator ivmid = VMid.rbegin();
		vector< double >::reverse_iterator iv = V_.rbegin();
		vector< double >::reverse_iterator ia = A_.rbegin();
		vector< unsigned long >::reverse_iterator icp;
		
		*ivmid = *ia / *( 4 + ia );
		*iv    = 2 * *ivmid - *iv;
		--ic, ++ivmid, ++iv, ia += 5;
		
		for ( icp = checkpoint_.rbegin();
		      icp != checkpoint_.rend();
		      icp += 2, --ic, ++ivmid, ++iv, ia += 5 ) {
			for ( ; ic > ( long )( *(1 + icp) );
			      --ic, ++ivmid, ++iv, ia += 5 ) {
				*ivmid = ( *ia - *( 3 + ia ) * *( ivmid - 1 ) )
				         / *( 4 + ia );
				*iv    = 2 * *ivmid - *iv;
			}
			
			*ivmid = ( *ia - *( 3 + ia ) * VMid[ *icp ] )
			         / *( 4 + ia );
			*iv    = 2 * *ivmid - *iv;
		}
		
		for ( ; ic >= 0; --ic, ++ivmid, ++iv, ia += 5 ) {
			*ivmid = ( *ia - *( 3 + ia ) * *( ivmid - 1 ) )
			         / *( 4 + ia );
			*iv    = 2 * *ivmid - *iv;
		}
	}

	void advanceChannels( ) {
		vector< double >::iterator iv;
		vector< double >::iterator ibase;
		vector< double >::iterator ilookup;
		vector< double >::iterator istate = state_.begin();
		vector< unsigned char >::iterator ifamily = gateFamily_.begin();
		vector< unsigned char >::iterator igco = gateCount_.begin();
		double distance, fraction;
		unsigned char ig;
		for ( iv = V_.begin(); iv != V_.end(); ++iv, ++igco )
			if ( *igco ) {
				distance = ( *iv - VLo_ ) / dV_;
				ibase    = lookup_.begin() + lookupBlocSize_ *
				           ( unsigned long )( distance );
				fraction = distance - floor( distance );
				for ( ig = 0; ig < *igco; ++ig, ++istate ) {
					ilookup = ibase + 2 * *( ifamily++ );
					*istate = *istate * \
						WT_AVG( *ilookup, *( lookupBlocSize_ + ilookup ), fraction ) + \
						WT_AVG( *( 1 + ilookup ), *( ( 1 + lookupBlocSize_ ) + ilookup ), fraction );
				}
			}
	}

/* "Protected" access for now. Private more appropriate, once interface to
 * set and get values from within solver is in place.
 */
protected:
	HSolveModel< T >*        model_;
	double                   dt_;
	unsigned long            N_;
	vector< T >              compartment_;
	vector< double >         V_;
	vector< double >         A_;
	vector< unsigned long >  checkpoint_;
	vector< double >         lambda_;
	vector< double >         iLeak_;
	vector< double >         inject_;
	vector< unsigned char >  channelCount_;
	vector< unsigned char >  gateCount_;
	vector< unsigned char >  gateCount1_;
	vector< double >         Gbar_;
	vector< double >         GbarEk_;
	vector< GateInfo< T > >  gateInfo_;
	vector< double >         state_;
	vector< double >         power_;
	vector< double >         lookup_;
	vector< unsigned char >  gateFamily_;
	int                      lookupBlocSize_;
	int                      NDiv_;
	double                   VLo_, VHi_, dV_;
};

#endif
