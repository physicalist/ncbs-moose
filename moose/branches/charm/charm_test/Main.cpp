#include "Main.h"

Main::Main(CkArgMsg *m){
  argMsg_ = m;
  thisProxy.commence();
}

void Main::commence(){
  CkPrintf("Test::commence\n");
  MooseParamCollection params;
  string serverName;
  int serverPort;

  params.add(string("--ccsServer"), &serverName, string("localhost"), true);
  params.add(string("--ccsPort"), &serverPort, -1, true);
  params.process(argMsg_->argc, argMsg_->argv);                      

  CkPrintf("server: %s\nport: %d\n", serverName.c_str(), serverPort);

  CkPrintf("ShellProxy connection...");
  shell_.ccsInit(serverName, serverPort);
  CkPrintf("done\n");

  CkPrintf("SetGetCcsClient connection...");
  SetGetCcsClient::connect(serverName, serverPort);
  CkPrintf("done\n");

  test();

  delete argMsg_;
  CkExit();
}

void Main::test(){
  rtHHNetwork(&shell_);
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
