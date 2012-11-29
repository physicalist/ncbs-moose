#ifndef PARAM_STORAGE_H
#define PARAM_STORAGE_H

#include <string>
using namespace std;

struct ParamStorage {
  //string exe;
  //string dir;
  //string launcher;
  //string nodelist;
  //string replyAddress;
  string ccsHost; 

  //int nPes;
  int ccsPort;
  int listenPort;

  // constants
  //static string defaultExe;
  //static string defaultDir;
  //static string defaultLauncher;
  static string defaultCcsHost; 
  //static int    defaultNPes;
  static int    defaultCcsPort;
  static int    defaultListenPort;

  static string getDefaultDir();
  static string getcwd();
};

#endif // PARAM_STORAGE_H
