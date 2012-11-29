#include "ParamStorage.h"
#include "MooseParams.h"
#include "MooseEnv.h"

//string ParamStorage::defaultExe = string("moose");
//string ParamStorage::defaultLauncher = string("charmrun");
//string ParamStorage::defaultDir = getDefaultDir(); 
//int    ParamStorage::defaultNPes = 1;
string ParamStorage::defaultCcsHost = "localhost";
int    ParamStorage::defaultCcsPort = 12345;
int    ParamStorage::defaultListenPort = 9999;

string ParamStorage::getDefaultDir(){
  string cwd = MooseEnv::getcwd_();
  cwd += (MooseEnv::DirectorySeparator_ + MooseEnv::PreviousDirectory_);
  return cwd;
}


