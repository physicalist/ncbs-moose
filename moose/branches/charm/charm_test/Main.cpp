#include "Main.h"
#include "ParamStorage.h"

Main::Main(CkArgMsg *m){
  argMsg_ = m;
  thisProxy.commence();
}



void Main::commence(){
  CkPrintf("Test::commence\n");

  MooseParamCollection params;
  ParamStorage p;

  //params.add(string("--ccsServer"), &serverName, string("localhost"), true);
  params.add(string("--exe"), &p.exe, ParamStorage::defaultExe, true);
  params.add(string("--pwd"), &p.pwd, ParamStorage::defaultPwd, true);
  params.add(string("--launcher"), &p.launcher, ParamStorage::defaultLauncher, true);
  params.add(string("--npes"), &p.nPes, ParamStorage::defaultNPes, true);
  params.add(string("--ccsPort"), &p.ccsPort, ParamStorage::defaultCcsPort, true);
  params.add(string("--listenPort"), &p.listenPort, ParamStorage::defaultListenPort, true);

  params.process(argMsg_->argc, argMsg_->argv);                      

  string serverName = launchJob(p);
  // when we return from launchJob, the job will have started up
  // and will be ready for CCS commands

  // we can then connect to the job's CCS server
  CkPrintf("server: %s\nport: %d\n", serverName.c_str(), p.ccsPort);

  CkPrintf("ShellProxy connection...");
  shell_.ccsInit(serverName, p.ccsPort);
  CkPrintf("done\n");

  CkPrintf("SetGetCcsClient connection...");
  SetGetCcsClient::connect(serverName, p.ccsPort);
  CkPrintf("done\n");

  test();

  delete argMsg_;
  CkExit();
}

void Main::test(){
  rtHHNetwork(&shell_);
  speedTestMultiNodeIntFireNetwork(2048, 20, &shell_);
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

#include "test.def.h"
