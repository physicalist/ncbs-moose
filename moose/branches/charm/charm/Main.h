#ifndef MOOSE_MAIN_H
#define MOOSE_MAIN_H

template <typename POINTER_TYPE>
struct PointerContainer {
  POINTER_TYPE *ptr;
  void pup(PUP::er &p){
    pup(p, (char **)&ptr, sizeof(char *));
  }
};

#include <string>
using namespace std;
#include "moose.decl.h"
#include "shell.decl.h"
#include "Messages.h"

class Main : public CBase_Main {
  public:
  Main_SDAG_CODE

  Main(CkArgMsg *m);
  void commence();

  private:
  void createMooseParallelObjects();

  private:
  int nElementContainers_;
  bool doUnitTests_;

};

#endif // MOOSE_MAIN_H
