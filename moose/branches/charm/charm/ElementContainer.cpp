#include <map>
using namespace std;

#include "ElementContainer.h"
#include "SimulationParameters.h"
class Cinfo;
#include "../basecode/MsgId.h"
#include "../basecode/Finfo.h"
#include "../basecode/ThreadId.h"
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
#include "../shell/ShellHelper.h"

#include "Messages.h"
class PrepackedBuffer;
class ReduceFieldDimension;
#include "../shell/Shell.h"

// readonly
extern SimulationParameters readonlySimulationParameters; 
extern CProxy_ShellHelper readonlyShellHelperProxy;

ElementContainer::ElementContainer(const CkCallback &cb) : 
  clock_(NULL),
  nDataBcastsReceived_(0)
{
  procInfo_.numThreadsInGroup = 1;
  procInfo_.groupId = 1;
  procInfo_.threadIndexInGroup = 1;
  procInfo_.nodeIndexInGroup = CkMyPe();
  procInfo_.numNodesInGroup = CkNumPes();
  procInfo_.procIndex = 0;
  procInfo_.container = this;

  contribute(cb);
}

void ElementContainer::newIteration(){
  clock_->processPhase1(&procInfo_);
  // for now, we buffer all data items issued by
  // elements, copy them into one bcast message and send
  // this msg out. Extensions of this are possible; e.g. 
  // use several, fixed size msgs or MeshStreamer to avoid
  // copying
  flushBufferedDataItems();

  // "juggle clock ticks (whatever that means)"
  clock_->processPhase2(&procInfo_);
}

void ElementContainer::addToQ(const ObjId &oi, BindIndex bindIndex, ThreadId threadNum, const double *arg, int size){
  // we are passing threadId = 0, to the Qinfo ctor
  // since data in an ElementContainer is not accessible
  // to any other ElementContainers on the same PE/SMP node
  qBuf_.push_back(Qinfo(oi, bindIndex, threadNum, this, dBuf_.size(), size));
  //if(size > 0){
    //dBuf_.insert(dBuf_.end(), arg, arg + size);
  //}
  for(int i = 0; i < size; i++) dBuf_.push_back(arg[i]);
}


void ElementContainer::addToQ(const ObjId &oi, BindIndex bindIndex, ThreadId threadNum, const double *arg1, int size1, const double *arg2, int size2){
  qBuf_.push_back(Qinfo(oi, bindIndex, threadNum, this, dBuf_.size(), size1 + size2));
  //if(size1 > 0) dBuf_.insert(dBuf_.end(), arg1, arg1 + size1);
  for(int i = 0; i < size1; i++) dBuf_.push_back(arg1[i]);
  //if(size2 > 0) dBuf_.insert(dBuf_.end(), arg2, arg2 + size2);
  for(int i = 0; i < size2; i++) dBuf_.push_back(arg2[i]);
}

void ElementContainer::addDirectToQ(const ObjId& src, const ObjId& dest, ThreadId threadNum, FuncId fid, const double* arg, unsigned int size){

  CkAssert(threadNum == 0);
  CkVec< double >& vec = dBufDirect_;

  qBufDirect_.push_back(DirectQbufEntry(
                          Qinfo(src, Qinfo::DirectAdd, threadNum, this, vec.size(), size), 
                          ObjFid(dest, fid, size, 1)));
  //if ( size > 0 ) {
    //vec.insert( vec.end(), arg, arg + size );
  //}
  for(unsigned int i = 0; i < size; i++) vec.push_back(arg[i]);
}

void ElementContainer::addDirectToQ( const ObjId& src, const ObjId& dest, 
	ThreadId threadNum,
	FuncId fid, 
	const double* arg1, unsigned int size1,
        const double* arg2, unsigned int size2 ){

  CkAssert(threadNum == 0);
  CkVec< double >& vec = dBufDirect_;
  qBufDirect_.push_back(DirectQbufEntry(
                          Qinfo( src, Qinfo::DirectAdd, threadNum, this, vec.size(), size1 + size2 ),
                          ObjFid(dest, fid, size1 + size2, 1)));

  //if ( size1 > 0 ) vec.insert( vec.end(), arg1, arg1 + size1 );
  for(unsigned int i = 0; i < size1; i++) vec.push_back(arg1[i]);
  //if ( size2 > 0 ) vec.insert( vec.end(), arg2, arg2 + size2 );
  for(unsigned int i = 0; i < size2; i++) vec.push_back(arg2[i]);
}

void ElementContainer::addVecDirectToQ( const ObjId& src, const ObjId& dest, 
	ThreadId threadNum,
	FuncId fid, 
	const double* arg, unsigned int entrySize, unsigned int numEntries )
{
  CkAssert(threadNum == 0);

  if ( entrySize == 0 || numEntries == 0 )
    return;

  CkVec< double >& vec = dBufDirect_;

  qBufDirect_.push_back(DirectQbufEntry(
                          Qinfo(src, Qinfo::DirectAdd, threadNum, this, vec.size(), entrySize * numEntries),
                          ObjFid(dest, fid, entrySize, numEntries)
                          ));

  //vec.insert( vec.end(), arg, arg + entrySize * numEntries );
  for(unsigned int i = 0; i < entrySize * numEntries; i++) vec.push_back(arg[i]);
}

void ElementContainer::flushBufferedDataItems(){
  ElementDataMsg *msg = obtainBcastMsgFromBuffers();
  thisProxy.exchange(msg);

  qBuf_.resize(0);
  dBuf_.resize(0);
  qBufDirect_.resize(0);
  dBufDirect_.resize(0);
}

ElementDataMsg *ElementContainer::obtainBcastMsgFromBuffers(){
  unsigned int numQinfo = qBuf_.size();
  unsigned int numData = dBuf_.size();
  unsigned int numDirectQinfo = qBufDirect_.size();
  unsigned int numDirectData = dBufDirect_.size();

  ElementDataMsg *msg = new (numQinfo, numData, numDirectQinfo, numDirectData) ElementDataMsg(numQinfo, numDirectQinfo);
  
  memcpy(msg->qinfo_, &qBuf_[0], numQinfo * sizeof(Qinfo));
  memcpy(msg->data_, &dBuf_[0], numData * sizeof(double));
  memcpy(msg->qinfoDirect_, &qBufDirect_[0], numDirectQinfo * sizeof(DirectQbufEntry));
  memcpy(msg->dataDirect_, &dBufDirect_[0], numDirectData * sizeof(double));

  return msg;
}

void ElementContainer::exchange(ElementDataMsg *m){
  nDataBcastsReceived_++;
  readBuf(m->qinfo_, m->nQinfo_, 
          m->qinfoDirect_, m->nQinfoDirect_, 
          m->data_, m->dataDirect_);
  delete m;

  if(nDataBcastsReceived_ == procInfo_.nElementContainers){
    nDataBcastsReceived_ = 0;
    // when all expected bcasts have been received, 
    // synchronize and (conditionally) start next 
    // iteration
    contribute(CkCallback(CkIndex_ElementContainer::iterationDone(), CkArrayIndex1D(0), thisProxy));
  }
  delete m;
}

void ElementContainer::readBuf(Qinfo *qinfo, unsigned int nQinfo, 
                               DirectQbufEntry *qinfoDirect, unsigned int nQinfoDirect,
                               const double *data, const double *dataDirect){
  // first look at all indirect qinfo entries
  for (unsigned int i = 0; i < nQinfo; ++i) {
    Qinfo &qi = qinfo[i];
    qi.setContainer(this); 
    const Element* e = qi.src().element();
    if (e) e->exec(&qi, data + qi.dataIndex());
  }

  // then examine the direct qinfo entries, of type DirectQbufEntry
  for(unsigned int i = 0; i < nQinfoDirect; i++){
    DirectQbufEntry &qi = qinfoDirect[i];
    qi.qinfo_.setContainer(this); 
    const Element *e = qi.qinfo_.src().element();
    if(e) e->exec(&qi.qinfo_, &qi.ofid_, dataDirect + qi.qinfo_.dataIndex());
  }
}

void ElementContainer::hackClearQ(int nCycles){
  for(int i = 0; i < nCycles; i++){
    // copy into temporary buffers
    CkVec< Qinfo > qBuf(qBuf_);
    CkVec< DirectQbufEntry > qBufDirect(qBufDirect_);
    CkVec< double > data(dBuf_);
    CkVec< double > dataDirect(dBufDirect_);

    qBuf_.resize(0);
    dBuf_.resize(0);
    qBufDirect_.resize(0);
    dBufDirect_.resize(0);

    // read from temporary buffers, and write into
    // qBuf_, etc.
    readBuf(qBuf.getVec(), qBuf.size(),
            qBufDirect.getVec(), qBufDirect.size(), 
            data.getVec(), dataDirect.getVec());
  }
}

unsigned int ElementContainer::qSize() const {
  return qBuf_.size(); 
}

unsigned int ElementContainer::dSize() const {
  return dBuf_.size(); 
}

void ElementContainer::addToReduceQ(ReduceBase *r, ThreadId threadNum){
  CkAssert(threadNum == 0);
  reduceBuf_.push_back(r);
}

void ElementContainer::clearReduceQ(unsigned int numThreads){
  // need to do some primary combining and then submit to 
  // charm++ reduction, which should have a customized reducer
  // mind that the MPI version uses MPI_Allgather

  // This function doesn't do anything at all, since at the 
  // moment, there is no compelling case for supporting reductions 
}

void ElementContainer::registerWithShell(const CkCallback &cb){
  shell_ = readonlyShellHelperProxy.ckLocalBranch()->getShell();
  shell_->registerContainer(this);
  contribute(cb);
}

void ElementContainer::start(double runTime){
  clock_->setRunTime(runTime);
  thisProxy[thisIndex].newIteration();
}

void ElementContainer::stop(){
  clock_->stop();
}

void ElementContainer::iterationDone(){
  if(shell_->getStop()){
    shell_->setStop(false);
  }
  else{
    thisProxy.newIteration();
  }
}
