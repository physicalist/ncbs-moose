#ifndef PARAM_STORAGE_H
#define PARAM_STORAGE_H

#include <string>
using namespace std;

struct ParamStorage {
  string exe;
  string pwd;
  string launcher;
  int nPes;
  int ccsPort;
  int listenPort;



  // constants
  static string defaultExe;
  static string defaultPwd;
  static string defaultLauncher;
  static int    defaultNPes;
  static int    defaultCcsPort;
  static int    defaultListenPort;
};

#endif // PARAM_STORAGE_H
