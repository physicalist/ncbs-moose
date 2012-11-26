#include "Main.h"
#include "doubleEq.h"
#include "randnum.h"

void Main::rtHHNetwork(ShellProxy *shell){
  const double EREST = -0.07;

  // AP measured in millivolts wrt EREST, at a sample interval of 
  // 100 usec
  double actionPotl[] = { 0,
    1.2315, 2.39872, 3.51917, 4.61015, 5.69088, 6.78432, 7.91934, 9.13413,
    10.482, 12.0417, 13.9374, 16.3785, 19.7462, 24.7909, 33.0981, 47.967,
    73.3833, 98.7377, 105.652, 104.663, 101.815, 97.9996, 93.5261, 88.6248,
    83.4831, 78.2458, 73.0175, 67.8684, 62.8405, 57.9549, 53.217, 48.6206,
    44.1488, 39.772, 35.4416, 31.0812, 26.5764, 21.7708, 16.4853, 10.6048,
    4.30989, -1.60635, -5.965, -8.34834, -9.3682, -9.72711,
    -9.81085, -9.78371, -9.71023, -9.61556, -9.50965, -9.39655,
    -9.27795, -9.15458, -9.02674, -8.89458, -8.75814, -8.61746,
    -8.47254, -8.32341, -8.17008, -8.01258, -7.85093, -7.68517,
    -7.51537, -7.34157, -7.16384, -6.98227, -6.79695, -6.60796,
    -6.41542, -6.21944, -6.02016, -5.81769, -5.61219, -5.40381,
    -5.19269, -4.979, -4.76291, -4.54459, -4.32422, -4.10197,
    -3.87804, -3.65259, -3.42582, -3.19791, -2.96904, -2.7394,
    -2.50915, -2.27848, -2.04755, -1.81652, -1.58556, -1.3548,
    -1.12439, -0.894457, -0.665128, -0.436511, -0.208708, 0.0181928,
  };


  ////////////////////////////////////////////////////////////////
  // Check construction and result of HH squid simulation
  ////////////////////////////////////////////////////////////////
  cout << "HHnet: creating objects" << endl;
  vector< int > dims( 1, 1 );
  CcsId nid = shell->doCreate( "Neutral", CcsId(), "n", dims, 1 );
  CcsId comptId = shell->doCreate( "Compartment", nid, "compt", dims, 1 );
  CcsId naId = shell->doCreate( "HHChannel", comptId, "Na", dims, 1 );
  MsgId mid = shell->doAddMsg( "OneToOne", CcsObjId( comptId ), "channel", 
      CcsObjId( naId ), "channel" );
  assert( mid != Msg::bad );
  CcsId kId = shell->doCreate( "HHChannel", comptId, "K", dims, 1 );
  mid = shell->doAddMsg( "OneToOne", CcsObjId( comptId ), "channel", 
      CcsObjId( kId ), "channel" );
  assert( mid != Msg::bad );

  //////////////////////////////////////////////////////////////////////
  // set up compartment properties
  //////////////////////////////////////////////////////////////////////

  cout << "HHnet: setting compartment props" << endl;
  FieldCcsClient< double >::set( comptId, "Cm", 0.007854e-6 );
  FieldCcsClient< double >::set( comptId, "Ra", 7639.44e3 ); // does it matter?
  FieldCcsClient< double >::set( comptId, "Rm", 424.4e3 );
  FieldCcsClient< double >::set( comptId, "Em", EREST + 0.010613 );
  FieldCcsClient< double >::set( comptId, "inject", 0.1e-6 );
  FieldCcsClient< double >::set( comptId, "initVm", EREST );

  //////////////////////////////////////////////////////////////////////
  // set up Na channel properties
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: set up Na channel" << endl;
  FieldCcsClient< double >::set( naId, "Gbar", 0.94248e-3 );
  FieldCcsClient< double >::set( naId, "Ek", EREST + 0.115 );
  FieldCcsClient< double >::set( naId, "Xpower", 3.0 );
  FieldCcsClient< double >::set( naId, "Ypower", 1.0 );

  //////////////////////////////////////////////////////////////////////
  // set up K channel properties
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: set up K channel" << endl;
  FieldCcsClient< double >::set( kId, "Gbar", 0.282743e-3 );
  FieldCcsClient< double >::set( kId, "Ek", EREST - 0.012 );
  FieldCcsClient< double >::set( kId, "Xpower", 4.0 );

  //////////////////////////////////////////////////////////////////////
  // set up m-gate of Na.
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: get Na channel children" << endl;
  vector< CcsId > kids; // These are the HHGates.
  kids = FieldCcsClient< vector< CcsId > >::get( naId, "children" );
  if( kids.size() != 3 ){
    cout << "Na nKids: " << kids.size() << endl;
    assert(false);
  }
  vector< double > parms;
  // For the alpha:
  // A = 0.1e6*(EREST*0.025), B = -0.1e6, C= -1, D= -(EREST+0.025), F = -0.01
  // For the beta: A = 4.0e3, B = 0, C = 0.0, D = -EREST, F = 0.018
  // xdivs = 100, xmin = -0.1, xmax = 0.05
  unsigned int xdivs = 150;
  double xmin = -0.1;
  double xmax = 0.05;
  parms.push_back( 0.1e6 * ( EREST + 0.025 ) );	// A alpha
  parms.push_back( -0.1e6 );				// B alpha
  parms.push_back( -1 );					// C alpha
  parms.push_back( -(EREST + 0.025 ) );	// D alpha
  parms.push_back( -0.01 );				// F alpha

  parms.push_back( 4e3 );		// A beta
  parms.push_back( 0 );		// B beta
  parms.push_back( 0 );		// C beta
  parms.push_back( -EREST );	// D beta
  parms.push_back( 0.018 );	// F beta

  parms.push_back( xdivs );
  parms.push_back( xmin );
  parms.push_back( xmax );

  cout << "HHnet: set Na-m gate alpha" << endl;
  SetGet1CcsClient< vector< double > >::set( kids[0], "setupAlpha", parms );
  cout << "HHnet: set Na-m gate interpolation" << endl;
  FieldCcsClient< bool >::set( kids[0], "useInterpolation", 1 );

  //////////////////////////////////////////////////////////////////////
  // set up h-gate of NA.
  //////////////////////////////////////////////////////////////////////
  // Alpha rates: A = 70, B = 0, C = 0, D = -EREST, F = 0.02
  // Beta rates: A = 1e3, B = 0, C = 1.0, D = -(EREST + 0.03 ), F = -0.01
  parms.resize( 0 );
  parms.push_back( 70 );
  parms.push_back( 0 );
  parms.push_back( 0 );
  parms.push_back( -EREST );
  parms.push_back( 0.02 );

  parms.push_back( 1e3 );		// A beta
  parms.push_back( 0 );		// B beta
  parms.push_back( 1 );		// C beta
  parms.push_back( -( EREST + 0.03 ) );	// D beta
  parms.push_back( -0.01 );	// F beta

  parms.push_back( xdivs );
  parms.push_back( xmin );
  parms.push_back( xmax );

  cout << "HHnet: set Na-h gate alpha" << endl;
  SetGet1CcsClient< vector< double > >::set( kids[1], "setupAlpha", parms );
  cout << "HHnet: set Na-h gate interpolation" << endl;
  FieldCcsClient< bool >::set( kids[1], "useInterpolation", 1 );

  //////////////////////////////////////////////////////////////////////
  // set up n-gate of K.
  //////////////////////////////////////////////////////////////////////
  // Alpha rates: A = 1e4 * (0.01 + EREST), B = -1e4, C = -1.0, 
  //	D = -(EREST + 0.01 ), F = -0.01
  // Beta rates: 	A = 0.125e3, B = 0, C = 0, D = -EREST ), F = 0.08
  cout << "HHnet: get K channel children" << endl;
  kids = FieldCcsClient< vector< CcsId > >::get( kId, "children" );
  parms.resize( 0 );
  parms.push_back(  1e4 * (0.01 + EREST) );
  parms.push_back( -1e4 );
  parms.push_back( -1.0 );
  parms.push_back( -( EREST + 0.01 ) );
  parms.push_back( -0.01 );

  parms.push_back( 0.125e3 );		// A beta
  parms.push_back( 0 );		// B beta
  parms.push_back( 0 );		// C beta
  parms.push_back( -EREST );	// D beta
  parms.push_back( 0.08 );	// F beta

  parms.push_back( xdivs );
  parms.push_back( xmin );
  parms.push_back( xmax );

  cout << "HHnet: set K-n gate alpha" << endl;
  SetGet1CcsClient< vector< double > >::set( kids[0], "setupAlpha", parms );
  cout << "HHnet: set K-n gate interpolation" << endl;
  FieldCcsClient< bool >::set( kids[0], "useInterpolation", 1 );

  //////////////////////////////////////////////////////////////////////
  // Set up SpikeGen and SynChan
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: create SynChan, SpikeGen" << endl;
  CcsId synChanId = shell->doCreate( "SynChan", comptId, "synChan", dims, 1);
  CcsId synId( synChanId.value() + 1 );
  CcsId axonId = shell->doCreate( "SpikeGen", comptId, "axon", dims, 1 );
  bool ret;
  
  cout << "synId path: " << shell->doGetPath(synId) << endl;

  // Not valid anymore: cannot access Element from Id/ObjId in parser 
  //assert( synId()->getName() == "synapse" );

  cout << "HHnet: configure SynChan" << endl;
  ret = FieldCcsClient< double >::set( synChanId, "tau1", 1.0e-3 );
  assert( ret );
  ret = FieldCcsClient< double >::set( synChanId, "tau2", 1.0e-3 );
  assert( ret );
  ret = FieldCcsClient< double >::set( synChanId, "Gbar", 0.01 );
  assert( ret );
  ret = FieldCcsClient< double >::set( synChanId, "Ek", 0.0 );
  assert( ret );

  mid = shell->doAddMsg( "OneToOne", 
      CcsObjId( comptId, CcsDataId( 0 ) ), "VmOut",
      CcsObjId( axonId, CcsDataId( 0 ) ), "Vm" );
  assert( mid != Msg::bad );

  mid = shell->doAddMsg( "OneToOne", 
      CcsObjId( comptId, CcsDataId( 0 ) ), "channel",
      CcsObjId( synChanId, CcsDataId( 0 ) ), "channel" );
  assert( mid != Msg::bad );

  // This is a hack, should really inspect msgs to automatically figure
  // out how many synapses are needed.
  /*
     ret = Field< unsigned int >::set( synChanId, "num_synapse", 2 );
     assert( ret );
     MsgId mid = shell->doAddMsg( "single", 
     ObjId( axonId, DataId( 0, 0 ) ), "event",
     ObjId( synId, DataId( 0, 0 ) ), "addSpike" );
     assert( mid != Msg::bad );
   */

  ret = FieldCcsClient< double >::set( axonId, "threshold", 0.0 );
  ret = FieldCcsClient< double >::set( axonId, "refractT", 0.01 );
  ret = FieldCcsClient< bool >::set( axonId, "edgeTriggered", 1 );

  /*
     ret = Field< double >::set( ObjId( synId, DataId( 0, 0 ) ), 
     "weight", 1.0 );
     assert( ret);
     ret = Field< double >::set( ObjId( synId, DataId( 0, 0 ) ), 
     "delay", 0.001 );
     assert( ret);
   */

  //////////////////////////////////////////////////////////////////////
  // Make a copy with lots of duplicates
  //////////////////////////////////////////////////////////////////////
  int numCopies = 10;
  CcsId copyParentId = shell->doCreate( "Neutral", CcsId(), "copy", dims );
  CcsId copyId = shell->doCopy( comptId, copyParentId, 
      "comptCopies", numCopies, false, false );
  // cout << "localEntries = " << copyId()->dataHandler()->localEntries() << "numCopies = " << numCopies << endl;

  // These are not valid anymore, since the parser is not 
  // in the same address space as the actual simulation
  /*
  unsigned int numLocal = numCopies / Shell::numNodes();
  assert( copyId()->dataHandler()->localEntries() == numLocal );
  assert( copyId()->dataHandler()->numDimensions() == 1 );
  */

  kids = FieldCcsClient< vector< CcsId > >::get( copyId, "children" );
  if( kids.size() != 4 ){
    cout << "HHnet: Copy nKids: " << kids.size() << endl;
    assert(false);
  }
  
  for(int i = 0; i < kids.size(); i++){
    cout << "HHnet: kids[" << i << "] path: " << shell->doGetPath(kids[i]) << endl;
  }
  // Cannot use operator() on Id to get Element
  /*
  assert( kids[0]()->getName() == "Na" );
  assert( kids[1]()->getName() == "K" );
  assert( kids[2]()->getName() == "synChan" );
  assert( kids[3]()->getName() == "axon" );
  */

  /*
  assert( kids[0]()->dataHandler()->localEntries() == numLocal );
  assert( kids[1]()->dataHandler()->localEntries() == numLocal );
  assert( kids[2]()->dataHandler()->localEntries() == numLocal );
  assert( kids[3]()->dataHandler()->localEntries() == numLocal );
  */

  ////////////////////////////////////////////////////////
  // Check that the HHGate data is accessible in copies.
  ////////////////////////////////////////////////////////
  cout << "HHnet: get kids of kids[1]" << endl;
  vector< CcsId > gateKids = FieldCcsClient< vector< CcsId > >::get( kids[1], "children" );
  assert ( gateKids.size() == 3 );
  // Cannot use operator() on Id to get Element
  // assert ( gateKids[0]()->dataHandler() != 0 );
  cout << "HHnet: get kparms" << endl;
  vector< double > kparms = FieldCcsClient< vector< double > >::get( 
      gateKids[0], "alpha" );
  assert( kparms.size() == 5 );
  for ( unsigned int i = 0; i < 5; ++i ) {
    assert( doubleEq( kparms[i], parms[i] ) );
  }
  ////////////////////////////////////////////////////////
  // Check that regular fields are the same in copies.
  ////////////////////////////////////////////////////////

  cout << "HHnet: get kids[0].Ek" << endl;
  double chanEk = FieldCcsClient< double >::get( 
      CcsObjId( kids[0], (numCopies * 234)/1000  ), 
      "Ek" ); 
  assert( doubleEq( chanEk, EREST + 0.115 ) );
  cout << "HHnet: get kids[1].Ek" << endl;
  chanEk = FieldCcsClient< double >::get( 
      CcsObjId( kids[1], (numCopies * 567)/1000 ), 
      "Ek" ); 
  assert( doubleEq( chanEk, EREST - 0.012 ) );
  cout << "HHnet: get kids[2].tau1" << endl;
  double tau1 = FieldCcsClient< double >::get( 
      CcsObjId( kids[2], (numCopies * 890)/1000 ), 
      "tau1" ); 
  assert( doubleEq( tau1, 0.001 ) );

  //////////////////////////////////////////////////////////////////////
  // Make table to monitor one of the compartments.
  //////////////////////////////////////////////////////////////////////

  cout << "HHnet: create table" << endl;
  CcsId tabId = shell->doCreate( "Table", copyParentId, "tab", dims );
  cout << "HHnet: connect table to copy" << endl;
  mid = shell->doAddMsg( "single", CcsObjId( tabId, 0 ), "requestData",
      CcsObjId( copyId, numCopies/2 ), "get_Vm" );
  assert( mid != Msg::bad );

  //////////////////////////////////////////////////////////////////////
  // Schedule, Reset, and run.
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: set clock" << endl;
  shell->doSetClock( 0, 1.0e-5 );
  shell->doSetClock( 1, 1.0e-5 );
  shell->doSetClock( 2, 1.0e-5 );
  shell->doSetClock( 3, 1.0e-4 );

  cout << "HHnet: use clock" << endl;
  shell->doUseClock( "/copy/comptCopies", "init", 0 );
  shell->doUseClock( "/copy/comptCopies", "process", 1 );
  shell->doUseClock( "/copy/comptCopies/##", "process", 2 );
  // shell->doUseClock( "/copy/compt/Na,/n/compt/K", "process", 2 );
  shell->doUseClock( "/copy/tab", "process", 3 );

  cout << "HHnet: reinit and start" << endl;
  shell->doReinit();
  shell->doReinit();
  shell->doStart( 0.01 );

  //////////////////////////////////////////////////////////////////////
  // Check output
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: check output" << endl;
  vector< double > vec = FieldCcsClient< vector< double > >::get( tabId, "vec" );
  // assert( vec.size() == 101 );
  double delta = 0;
  for ( unsigned int i = 0; i < 100; ++i ) {
    double ref = EREST + actionPotl[i] * 0.001;
    delta += (vec[i] - ref) * (vec[i] - ref);
  }
  assert( sqrt( delta/100 ) < 0.001 );

  ////////////////////////////////////////////////////////////////
  // Connect up the network.
  ////////////////////////////////////////////////////////////////
  double connectionProbability = 1.5 / sqrt( numCopies );
  CcsId synCopyId( kids[2].value() + 1 );

  cout << "HHnet: add msg" << endl;
  mid = shell->doAddMsg( "Sparse", kids[3], "event", 
      synCopyId, "addSpike" );
  assert( mid != Msg::bad );
  // Not allowed to use Eref in parser
  // Eref manager = Msg::getMsg( mid )->manager();
  // Use following shell proxy function instead
  CcsObjId msgMgrObjId = shell->doGetMsgMgr(mid);

  cout << "HHnet: set msg random connectivity" << endl;
  SetGet2CcsClient< double, long >::set( msgMgrObjId, "setRandomConnectivity",
      connectionProbability, 1234UL );

  // shell->doSyncDataHandler( kids[2], "get_numSynapses", synCopyId );
  shell->doSyncDataHandler( synCopyId );

  // Make it twice as big as expected probability, for safety.
  unsigned int numConnections = 
    2 * numCopies * static_cast< unsigned int >( 
        sqrt( double( numCopies ) ) );
  mtseed( 1000UL );
  vector< double > weight( numConnections );
  vector< double > delay( numConnections );
  double delayRange = 10e-3;
  double delayMin = 5e-3;
  double weightMax = 0.1;
  for ( unsigned int i = 0; i < numConnections; ++i ) {
    weight[i] = mtrand() * weightMax;
    delay[i] = mtrand() * delayRange + delayMin;
  }
  cout << "HHnet: set synapse weights" << endl;
  ret = FieldCcsClient< double >::setVec( synCopyId, "weight", weight );
  assert( ret );
  cout << "HHnet: set synapse delays" << endl;
  ret = FieldCcsClient< double >::setVec( synCopyId, "delay", delay );
  assert( ret );

  const double injectRange = 0.1e-6;
  vector< double > inject( numCopies, 0 );
  for ( unsigned int i = 0; i < numCopies; ++i )
    inject[i] = mtrand() * injectRange;
  cout << "HHnet: set injection current" << endl;
  ret = FieldCcsClient< double >::setVec( copyId, "inject", inject );
  assert( ret );

  //////////////////////////////////////////////////////////////////////
  // Reset, and run again. This time long enough to have lots of 
  // synaptic activity
  //////////////////////////////////////////////////////////////////////
  cout << "HHnet: reinit and run again" << endl;
  shell->doReinit();
  shell->doReinit();
  shell->doStart( 0.1 );

  cout << "HHnet: set plot" << endl;
  SetGet2CcsClient< string, string >::set( CcsObjId( tabId ), 
      "xplot", "hhnet.plot", "hhnet" );


#if 0
#endif
  ////////////////////////////////////////////////////////////////
  // Clear it all up
  ////////////////////////////////////////////////////////////////
  cout << "HHnet: clean up" << endl;
  shell->doDelete( copyParentId );
  shell->doDelete( nid );
  cout << "." << flush;
}
