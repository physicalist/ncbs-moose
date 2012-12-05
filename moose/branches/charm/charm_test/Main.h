#include "ccs-client.h"
#include "test.decl.h"
#include "pup.h"

#include "CcsId.h"
#include "CcsDataId.h"
#include "CcsObjId.h"
#include "ShellProxy.h"
#include "SetGetCcsClient.h"

#include "MooseParams.h"

class ProcInfo;
class Qinfo;
class Id;
class ObjId;
class DataId;
class Element;
class FuncId;
class Eref;
class Cinfo;
#include "../msg/Msg.h"

#include <iostream>
using namespace std;

class ParamStorage;


class Main : public CBase_Main {
  ShellProxy shell_;
  CkArgMsg *argMsg_;

  public:
  Main(CkArgMsg *m);
  void commence();

  private:
  void acceptJobReply(ParamStorage &p);
  string findHostAddr();
  template<typename T>
  string toString(const T &t);

  void test();
  void rtHHNetwork(int numCopies, ShellProxy *shell);
  void speedTestMultiNodeIntFireNetwork( unsigned int size, unsigned int runsteps, ShellProxy *shell );
};


