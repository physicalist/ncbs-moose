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
#include "../basecode/MsgId.h"
#include "../basecode/moose_version.h"
#include "../basecode/CcsCinfo.h"

#define MOOSE_CCS_TIMEOUT 120

class ShellProxy {
  private:
  CcsServer shellServer_;
  bool isRunning_;

  public:

  ShellProxy() :
    isRunning_(false)
  {}

  ~ShellProxy() {}

  string doVersion();
  string doRevision();
  void setCwe( CcsId cwe );
  CcsId getCwe();
  bool isRunning() const;
  CcsId doCreate( string type, CcsId parent, string name, 
      vector< int > dimensions = vector< int >( 1, 1 ),
      bool isGlobal = 0, bool qFlag = 0 );
  bool doDelete( CcsId id, bool qFlag = 0 );
  MsgId doAddMsg( const string& msgType, 
      CcsObjId src, const string& srcField, 
      CcsObjId dest, const string& destField, bool qFlag = 0);
  void doQuit( bool qFlag = 0 );
  void doStart( double runtime, bool qFlag = 0 );
  void doNonBlockingStart( double runtime, bool qFlag = 0 );
  void doReinit( bool qFlag = 0 );
  void doStop( bool qFlag = 0 );
  void doTerminate( bool qFlag = 0 );
  void doMove( CcsId orig, CcsId newParent, bool qFlag = 0 );
  CcsId doCopy( CcsId orig, CcsId newParent, string newName,
      unsigned int n, bool toGlobal, bool copyExtMsgs, 
      bool qFlag = 0 );
  CcsObjId doFind( string& path );
  void doUseClock( string path, string field, unsigned int tick,
      bool qFlag = 0 );
  CcsId doLoadModel( const string& fname, const string& modelpath, 
      const string& solverClass = "", bool qFlag = 0 );
  int doWriteSBML( const string& fname, const string& modelpath,
      bool qFlag = 0 );
  void doSyncDataHandler( CcsId tgt );
  void doReacDiffMesh( CcsId baseCompartment );
  void start( double runTime );
  //void recvGet( const Eref& e, const Qinfo* q, PrepackedBuffer pb );
  void doSetClock( unsigned int tickNum, double dt, bool qFlag = 0 );
  void cleanSimulation();

  ////////////////////////////////////////////////////////////////
  // Utility functions
  ////////////////////////////////////////////////////////////////
  bool adopt(CcsId parent, CcsId child);

  // replacement for Id::path(); sends
  // a CCS request, which calls Neutral::path(), so that
  // we don't need to include any of that functionality
  // in the parser.
  string doGetPath(const CcsId &id);
  string doGetObjIdPath(const CcsObjId &id);

  // replacement for Id::isValid
  bool doGetIsValid(const CcsId &id);

  // invokes Shell::wildcard
  void wildcard(string path, vector<CcsId> &list);

  // invokes Msg::getMsg(mid)->manager().objId();
  CcsObjId doGetMsgMgr(MsgId mid);

  unsigned int getNumPes();
  void doGetCinfos(vector< CcsCinfo > &ret);

  /* XXX where is this used?
     XXX redefine if necessary
  /// Digests outcome of calculation for max index of ragged array
  void digestReduceFieldDimension( 
      const Eref& er, const ReduceFieldDimension* arg );
  */

  public:
  void ccsInit(string serverName, int serverPort);


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

  // these are additions to keep parser separate
  // from moose core
  static const char *doGetPathHandlerString; 
  static const char *doGetObjIdPathHandlerString; 
  static const char *doGetIsValidHandlerString; 
  static const char *doWildcardHandlerString; 
  static const char *doGetMsgMgrHandlerString; 
  static const char *doGetCinfosHandlerString; 
};


#endif // MOOSE_SHELL_PROXY_H
