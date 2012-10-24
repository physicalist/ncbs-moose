#include <string>
#include <vector>
#include <map>

using namespace std;

#include "../basecode/ProcInfo.h"
class Element;
#include "../basecode/DataId.h"
class Eref;
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../basecode/Eref.h"
#include "../basecode/BindIndex.h"
#include "../basecode/FuncId.h"
#include "../basecode/ThreadId.h"
#include "../basecode/Qinfo.h"

class Cinfo;
#include "../basecode/MsgId.h"
#include "../basecode/Finfo.h"
class OpFunc;
#include "../basecode/Cinfo.h"
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"
class DataHandler;
struct DimInfo;
class MsgFuncBinding;
class DestFinfo;
#include "../basecode/Element.h"
#
#include "LookupHelper.h"
class Eref;
#include "../basecode/Id.h"
#include "../basecode/DataHandler.h"
#include "../shell/Shell.h"

extern Id init( int argc, char** argv, bool& doUnitTests, bool& doRegressionTests );

LookupHelper::LookupHelper(CkVec< string > &s_argv, const CkCallback &cb){
  // make C strings out of C++ strings
  argv_ = new char *[s_argv.size()];
  argc_ = s_argv.size();
  for(unsigned int i = 0; i < argc_; i++){
    argv_[i] = new char[s_argv[i].size()+1];
    std::copy(s_argv[i].begin(), s_argv[i].end(), argv_[i]);
    argv_[i][s_argv[i].size()] = '\0';
  }

  contribute(cb);
}

void LookupHelper::initShell(const CkCallback &cb){
  bool doUnitTests = 0;
  bool doRegressionTests = 1;
  shellId_ = init( argc_, argv_, doUnitTests, doRegressionTests );

  for(unsigned int i = 0; i < argc_; i++){
    delete[] argv_[i];
    argv_[i] = NULL;
  }
  delete[] argv_;
  argv_ = NULL;

  shell_ = reinterpret_cast< Shell* >( shellId_()->dataHandler()->data( 0 ) );
  Id clockId(1);
  clock_ = reinterpret_cast< Clock* >( clockId()->dataHandler()->data( 0 ) );
  shell_->setClockPointer(clock_);

  contribute(cb);
}

void LookupHelper::iterationDone(){
  shell_->iterationDone();
}

Element *LookupHelper::get(Id id){
  return elements_[id.value()];
}

void LookupHelper::set(Id id, Element *e){
  elements_[id.value()] = e;
}

Shell *LookupHelper::getShell(){
  return shell_;
}

Clock *LookupHelper::getClock(){
  return clock_;
}

vector< Element * > &LookupHelper::elements(){
  return elements_;
}

ThreadId LookupHelper::registerContainer(ElementContainer *container){
  return shell_->registerContainer(container);
}

ElementContainer *LookupHelper::getContainer(ThreadId id){
  return shell_->getContainer(id);
}

extern void testAsync();
extern void testMsg();
extern void testShell();
extern void testScheduling();
extern void testBuiltins();
extern void testKinetics();
extern void testBiophysics();
extern void testHSolve();
extern void testGeom();
extern void testMesh();

extern void testMpiMsg();
extern void testMpiShell();
extern void testMpiBuiltins();
extern void testMpiScheduling();

#ifdef USE_SMOLDYN
extern void testSmoldyn();
#endif

void LookupHelper::doSerialUnitTests(const CkCallback &cb){
  testAsync();
  testMsg();
  testShell();
  testScheduling();
  testBuiltins();
  testKinetics();
  testBiophysics();
  testHSolve();
  testGeom();
  testMesh();
#ifdef USE_SMOLDYN
  testSmoldyn();
#endif
  contribute(cb);
}

void LookupHelper::doParallelUnitTests(const CkCallback &cb){
  testMpiMsg();
  cout << "." << flush;
  testMpiShell();
  cout << "." << flush;
  testMpiBuiltins();
  cout << "." << flush;
  //testMpiScheduling();
  //cout << "." << flush;
  contribute(cb);
}

void LookupHelper::invokeFinishCallback(){
  shell_->invokeFinishCallback();
}
