#include "Main.h"
#include "doubleEq.h"
#include "randnum.h"

void Main::speedTestMultiNodeIntFireNetwork( unsigned int size, unsigned int runsteps, ShellProxy *shell )
{

        CkPrintf("speedTestMultiNodeIntFireNetwork\n");

	static const double thresh = 0.1;
	static const double Vmax = 1.0;
	static const double refractoryPeriod = 0.4;
	static const double weightMax = 0.1;
	static const double delayMax = 4;
	static const double timestep = 0.2;
	static const double connectionProbability = 0.2;
	/*
	// Applicable for size == 1024
	static const double testVm[] = {
		-3.41353,
		-2.50449,
		-2.64486,
		-0.387621,
		-1.08778,
		-1e-07,
		-0.187895,
		-0.716069,
		-1e-07,
		-0.0155043,
		-1e-07
	};
	// Applicable for size == 2048
	static const double testVm[] = {
		-1e-07,
		-0.0505845,
		0.0456282,
		-5.61229,
		-1e-07,
		-1e-07,
		-2.72,
		-0.0441219,
		-1e-07,
		-1e-07,
		-1e-07,
		-2.4886,
		-3.89123,
		-0.479408,
		-2.84331,
		-2.7225,
		-1e-07,
		-1.74236,
		-0.35803,
		-1.89033,
		-1.81688
	};
	*/

	vector< int > dims( 1, size );
	CcsId i2 = shell->doCreate( "IntFire", CcsId(), "test2", dims );
        // No operator() allowed on Id
	//assert( i2()->getName() == "test2" );
        // no Eref's allowed
	//Eref e2 = i2.eref();
        cout << "i2 path: " << shell->doGetPath(i2) << endl;
        CcsObjId o2(i2);

	CcsId synId( i2.value() + 1 );
        // no Element's allowed
	//Element* syn = synId();
	//assert( syn->getName() == "synapse" );

	//assert( syn->dataHandler()->totalEntries() == 0 );

	//DataId di( 1 ); // DataId( data, field )
	//Eref syne( syn, di );

	/*
	unsigned int numThreads = 1;
	if ( Qinfo::numSimGroup() >= 2 ) {
		numThreads = Qinfo::simGroup( 1 )->numThreads;
	}
	*/

	MsgId mid = shell->doAddMsg( "Sparse", i2, "spike",
		CcsObjId( synId, 0 ), "addSpike" );
	
	//const Msg* m = Msg::getMsg( mid );
        // No Eref's allowed; get ObjId directly from mid
	//Eref mer = m->manager();

        CcsObjId msgMgrObjId = shell->doGetMsgMgr(mid);

        cout << "creating random connectivity graph" << endl;
	SetGet2CcsClient< double, long >::set( msgMgrObjId, "setRandomConnectivity", 
		connectionProbability, 5489UL );

	// SetGet1< unsigned int >::set( mer, "loadBalance", numThreads ); 
	vector< unsigned int > synArraySizes;
	// unsigned int start = syn->dataHandler()->getNumData2( synArraySizes );

	//unsigned int nd = syn->dataHandler()->totalEntries();
	unsigned int nd = 2 * size * size * connectionProbability;
	//unsigned int nd = 104576;

	vector< double > temp( size, 0.0 );
	for ( unsigned int i = 0; i < size; ++i )
		temp[i] = mtrand() * Vmax;

        cout << "setting Vm" << endl;
	bool ret = FieldCcsClient< double >::setVec( i2, "Vm", temp );
	assert( ret );

	temp.clear();
	temp.resize( size, thresh );
        cout << "setting thresh" << endl;
	ret = FieldCcsClient< double >::setVec( i2, "thresh", temp );
	assert( ret );
	temp.clear();
	temp.resize( size, refractoryPeriod );

        cout << "setting refractory period" << endl;
	ret = FieldCcsClient< double >::setVec( i2, "refractoryPeriod", temp );
	assert( ret );

	vector< double > weight;
	weight.reserve( nd );
	vector< double > delay;
	delay.reserve( nd );
	for ( unsigned int i = 0; i < nd; ++i ) {
		weight.push_back( 2.0 * ( mtrand() - 0.5 ) * weightMax );
		delay.push_back( mtrand() * delayMax );
	}

        cout << "setting synaptic weights" << endl;
	ret = FieldCcsClient< double >::setVec( synId, "weight", weight );
	assert( ret );

        cout << "setting synaptic delays" << endl;
	ret = FieldCcsClient< double >::setVec( synId, "delay", delay );
	assert( ret );

        // No Element's allowed; make er0 ObjId directly
	//Element* ticke = Id( 2 )();
	//Eref er0( ticke, DataId( 0 ) );
        CcsObjId clockObjId( CcsId(2), CcsDataId(0) );

        cout << "setting up clock" << endl;
	shell->doAddMsg( "Single", clockObjId, "process0",
		o2, "process" );
	shell->doSetClock( 0, timestep );

        shell->doStart( static_cast< double >( timestep * runsteps) + 0.0 ); 


	/*
	for ( unsigned int i = 0; i < size; i += 100 ) {
		double ret = Field< double >::get( Eref( e2.element(), i ), "Vm" );
		if ( fabs( ret - testVm[i/100] ) > 1e-5 )
			cout << "1000 step IntFire test Error: " << i << ": got = " 
				<< ret << ", expected = " << testVm[i/100] << endl;
	}
	*/

	cout << "." << flush;
	shell->doDelete( synId );
	shell->doDelete( i2 );
	//shell->doQuit();
}
