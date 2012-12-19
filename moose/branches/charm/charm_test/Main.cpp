#include "Main.h"
#include "ParamStorage.h"

Main::Main(CkArgMsg *m){
  argMsg_ = m;
  thisProxy.commence();
}

ShellProxy *globalShellProxy = NULL;

void Main::commence(){
  CkPrintf("Test::commence\n");

  globalShellProxy = &shell_;

  MooseParamCollection params;
  ParamStorage p;
  // whether to listen for incoming connection from moose-core; true by default.
  // when specified as false, charm_test skips the acceptJobReply. this is done
  // when it is known that moose-core has already started up previously, and hence 
  // will not be initiating a connection to charm_test
  int listen;

  //params.add(string("--ccsServer"), &serverName, string("localhost"), true);
  //params.add(string("exe"), &p.exe, ParamStorage::defaultExe, true);
  //params.add(string("dir"), &p.dir, ParamStorage::defaultDir, true);
  //params.add(string("launcher"), &p.launcher, ParamStorage::defaultLauncher, true);
  //// no default for nodelist; must be specified explicitly
  //params.add(string("nodelist"), &p.nodelist, true);
  //params.add(string("npes"), &p.nPes, ParamStorage::defaultNPes, true);
  params.add(string("ccsHost"), &p.ccsHost, ParamStorage::defaultCcsHost, true);
  params.add(string("ccsPort"), &p.ccsPort, ParamStorage::defaultCcsPort, true);
  params.add(string("listenPort"), &p.listenPort, ParamStorage::defaultListenPort, true);
  params.add(string("listen"), &listen, 1, true);

  if(!params.process(argMsg_->argc, argMsg_->argv)){
    CkPrintf("[main] test not started; missing parameters\n");
    CkExit();
  }

  // fix directory from relative to absolute,
  // if needed
  /*
  if(!MooseEnv::isAbsolutePath(p.dir)){
    p.dir = MooseEnv::CurrentWorkingDirectory_ + MooseEnv::DirectorySeparator_ + p.dir;
  }
  */

  if(listen) acceptJobReply(p);
  // when we return from accepJobReply, the job will have started up
  // and will be ready for CCS commands

  // we can then connect to the job's CCS server
  CkPrintf("server: %s\nport: %d\n", p.ccsHost.c_str(), p.ccsPort);

  CkPrintf("ShellProxy connection...");
  shell_.ccsInit(p.ccsHost, p.ccsPort);
  CkPrintf("done\n");

  CkPrintf("SetGetCcsClient connection...");
  SetGetCcsClient::connect(p.ccsHost, p.ccsPort);
  CkPrintf("done\n");

  test();

  shell_.doQuit();

  delete argMsg_;
  CkExit();
}

void Main::test(){
  //rtHHNetwork(100, &shell_);
  //speedTestMultiNodeIntFireNetwork(2048, 20, &shell_);
  vector <CcsId> ids(FieldCcsClient< vector<CcsId> >::get(CcsObjId(string("/classes")), "children")); 
  vector <string> names(FieldCcsClient< vector<string> >::get(CcsObjId(string("/classes")), "childrenNames")); 
  assert(ids.size() == names.size());

  for(int i = 0; i < ids.size(); i++){
    cout << "class " << i << " id: " << ids[i] << " name: " << names[i] << endl; 
  }

  vector< CcsCinfo > cinfos;
  shell_.doGetCinfos(cinfos);

  for(int i = 0; i < cinfos.size(); i++){
    cout << "cinfo " << i << " name: " << cinfos[i].name() << endl; 
  }
}

extern void MooseSetTypeCodes(map<string, string> &table);
extern void MooseSetClassCodes(map<string, string> &table);
extern string MooseGetTypeCode(map<string, string> &table, string in);

extern void initializeSetGetCcsHandlerStrings(map<string, string> &typeCodeTable, map<string, string> &classCodeTable);

void defineHandlerStrings(){
  map<string, string> typeCodeTable;
  map<string, string> classCodeTable;

  MooseSetClassCodes(classCodeTable);
  MooseSetTypeCodes(typeCodeTable);

  initializeSetGetCcsHandlerStrings(typeCodeTable, classCodeTable);
}

// this function is a dummy, never actually called by the charm_test code, but
// expected as extern by CcsId/CcsObjId. so, we include it for successful compilation

extern "C" ShellProxy *getParserShellProxy() { return globalShellProxy; }

#include "test.def.h"
