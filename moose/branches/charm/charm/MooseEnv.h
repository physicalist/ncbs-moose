#ifndef MOOSE_ENV_H
#define MOOSE_ENV_H

#include <string>
using namespace std;

struct MooseEnv {
  static string PreviousDirectory_;
  static string DirectorySeparator_;
  static string CurrentWorkingDirectory_;
  static string FlagPrefix_;

  static string getcwd_();
  static bool isAbsolutePath(const string &path);
  static unsigned int getNumCores();
};

#endif // MOOSE_ENV_H
