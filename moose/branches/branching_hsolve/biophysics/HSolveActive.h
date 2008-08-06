/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_ACTIVE_H
#define _HSOLVE_ACTIVE_H

class HSolveActive: public HSolvePassive
{
public:
	HSolveActive();
	
	void setup( Id seed, double dt );
	void solve( ProcInfo info );

protected:
	double                    vMin_;
	double                    vMax_;
	int                       vDiv_;
	double                    caMin_;
	double                    caMax_;
	int                       caDiv_;

	vector< double >          Gk_;
	vector< double >          GkEk_;
	vector< double >          state_;
	vector< int >             instant_;
	vector< RateLookup >      lookup_;
	vector< RateLookupGroup > lookupGroup_;
	vector< ChannelStruct >   channel_;
	vector< SpikeGenStruct >  spikegen_;
	vector< SynChanStruct >   synchan_;
	vector< CaConcStruct >    caConc_;
	vector< double >          ca_;
	vector< double >          caActivation_;
	vector< double* >         caTarget_;
	vector< double* >         caDepend_;
	vector< Id >              channelId_;
	vector< Id >              gateId_;
	vector< bool >            gCaDepend_;
	vector< unsigned char >   channelCount_;

private:
	// Setting up of data structures
	void readChannels( );
	void readGates( );
	void readCalcium( );
	void readSynapses( );
	void createLookupTables( );
	void cleanup( );

	// Integration
	void calculateChannelCurrents( );
	void updateMatrix( );
	void forwardEliminate( );
	void backwardSubstitute( );
	void advanceCalcium( );
	void advanceChannels( double dt );
	void advanceSynChans( ProcInfo info );
	void sendSpikes( ProcInfo info );

	static const int INSTANT_X;
	static const int INSTANT_Y;
	static const int INSTANT_Z;
};

#endif // _HSOLVE_ACTIVE_H
