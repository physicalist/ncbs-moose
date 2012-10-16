#ifndef MOOSE_SHELL_HELPER_H
#define MOOSE_SHELL_HELPER_H
#include <string>

class Shell;
#include "../charm/shell.decl.h"

class ShellHelper : public CBase_ShellHelper {

  private: 
  Shell *shell_;
  Id shellId_;

  public:
  ShellHelper(int argc, string *s_argv, const CkCallback &cb);
  void destroyShell(const CkCallback &cb);

  public:
  Shell *getShell();
};

class ShellMain : public CBase_ShellMain {
  public:
  ShellMain_SDAG_CODE

  ShellMain(CkArgMsg *msg);

};

#endif // MOOSE_SHELL_HELPER_H
