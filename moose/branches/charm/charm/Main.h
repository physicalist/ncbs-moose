#ifndef MOOSE_MAIN_H
#define MOOSE_MAIN_H

#include <string>
using namespace std;
#include "moose.decl.h"
#include "shell.decl.h"

class Main : public CBase_Main {
  public:
  Main_SDAG_CODE

  Main(CkArgMsg *m);
  void commence();
  static void RetryHeadNodeConnect(void *arg, double);
  void trySendReply();

  private:
  void createMooseParallelObjects(CkArgMsg *m);
  void sendSocketReply();

  private:
  bool doUnitTests_;
  int replyPort_;
  string replyAddress_;
  int retryPeriod_;
  int retryAttempts_;

};

#endif // MOOSE_MAIN_H
