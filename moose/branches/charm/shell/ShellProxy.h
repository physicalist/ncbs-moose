/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef MOOSE_SHELL_PROXY_H
#define MOOSE_SHELL_PROXY_H

#include <string>
#include <vector>
using namespace std;

#include "ccs-client.h"

class Eref;
class Qinfo;
class Id;
class ObjId;

#define MOOSE_CCS_TIMEOUT 120

class ShellProxy {
  private:
  CcsServer shellServer_;

  bool isRunning_;
  void destroy( const Eref& e, const Qinfo* q, Id eid);

  public:

  ShellProxy();
  ~ShellProxy();

  string doVersion();
  string doRevision();
  void setCwe( Id cwe );
  Id getCwe();
  bool isRunning() const;
  Id doCreate( string type, Id parent, string name, 
      vector< int > dimensions = vector< int >( 1, 1 ),
      bool isGlobal = 0, bool qFlag = 0 );
  bool doDelete( Id id, bool qFlag = 0 );
  MsgId doAddMsg( const string& msgType, 
      ObjId src, const string& srcField, 
      ObjId dest, const string& destField, bool qFlag = 0);
  void doQuit( bool qFlag = 0 );
  void doStart( double runtime, bool qFlag = 0 );
  void doNonBlockingStart( double runtime, bool qFlag = 0 );
  void doReinit( bool qFlag = 0 );
  void doStop( bool qFlag = 0 );
  void doTerminate( bool qFlag = 0 );
  void doMove( Id orig, Id newParent, bool qFlag = 0 );
  void doCopy( Id orig, Id newParent, string newName,
      unsigned int n, bool toGlobal, bool copyExtMsgs, 
      bool qFlag = 0 );
  ObjId doFind( string& path );
  void doUseClock( string path, string field, unsigned int tick,
      bool qFlag = 0 );
  Id doLoadModel( const string& fname, const string& modelpath, 
      const string& solverClass = "", bool qFlag = 0 );
  int doWriteSBML( const string& fname, const string& modelpath,
      bool qFlag = 0 );
  void doSyncDataHandler( Id tgt );
  void doReacDiffMesh( Id baseCompartment );
  void clearRestructuringQ();
  void start( double runTime );
  //void recvGet( const Eref& e, const Qinfo* q, PrepackedBuffer pb );
  void doSetClock( unsigned int tickNum, double dt, bool qFlag = 0 );
  void cleanSimulation();

  ////////////////////////////////////////////////////////////////
  // Utility functions
  ////////////////////////////////////////////////////////////////
#ifndef USE_CHARMPP
  static bool adopt( Id parent, Id child );
#else
  bool adopt(Id parent, Id child);
#endif

  /* XXX where is this used?
     XXX redefine if necessary
  /// Digests outcome of calculation for max index of ragged array
  void digestReduceFieldDimension( 
      const Eref& er, const ReduceFieldDimension* arg );
  */


  // statics
  public:
  static const char *setCweHandlerString; 
  static const char *getCweHandlerString; 
  static const char *doCreateHandlerString; 
  static const char *doDeleteHandlerString; 
  static const char *doAddMsgHandlerString; 
  static const char *doQuitHandlerString; 
  static const char *doStartHandlerString; 
  static const char *doReinitHandlerString; 
  static const char *doStopHandlerString; 
  static const char *doTerminateHandlerString; 
  static const char *doMoveHandlerString; 
  static const char *doCopyHandlerString; 
  static const char *doFindHandlerString; 
  static const char *doUseClockHandlerString; 
  static const char *doLoadModelHandlerString; 
  static const char *doWriteSBMLHandlerString; 
  static const char *doSyncDataHandlerString; 
  static const char *doReacDiffMeshHandlerString; 
  static const char *doSetClockHandlerString; 
  static const char *doCleanSimulationHandlerString; 
  static const char *doAdoptHandlerString; 
};


#endif // MOOSE_SHELL_PROXY_H
