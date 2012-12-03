#include <map>
#include <iostream>
#include <vector>
#include <string>
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

#include "Messages.h"
class PrepackedBuffer;
class ReduceFieldDimension;
#include "../shell/Shell.h"

#include "LookupHelper.h"

// readonly
extern unsigned int readonlyNumElementContainers;
extern unsigned int readonlyNumUsefulElementContainers;
extern SimulationParameters readonlySimulationParameters; 
extern CProxy_LookupHelper readonlyLookupHelperProxy;

ElementContainer::ElementContainer(const CkCallback &cb) : 
  shell_(NULL),
  clock_(NULL),
  lookup_(NULL),
  lookupRegistrationIdx_(-1),
  nDataBcastsReceived_(0)
{
  procInfo_.numThreadsInGroup = 1;
  procInfo_.groupId = 1;
  procInfo_.threadIndexInGroup = -1;
  procInfo_.nodeIndexInGroup = CkMyPe();
  procInfo_.numNodesInGroup = CkNumPes();
  procInfo_.procIndex = 0;

  contribute(cb);
}

void ElementContainer::newIteration(){
  //CkPrintf("[%d] ElementContainer::newIteration threadId %d\n", thisIndex, procInfo_.threadIndexInGroup);
  clock_->processPhase1(&procInfo_);
  // for now, we buffer all data items issued by
  // elements, copy them into one bcast message and send
  // this msg out. Extensions of this are possible; e.g. 
  // use several, fixed size msgs or MeshStreamer to avoid
  // copying
  flushBufferedDataItems();

  // once a container has received all the messages intended
  // for it, it contributes to a reduction that transfers 
  // control to Shell::iterationDone (via LookupHelper::iterationDone),
  // where we check whether (i) we have received a stop command from
  // the parser or (ii) the simulation clock has expired.
  // this is done by Shell (see reinitIterationDone, runIterationDone)
}

void ElementContainer::addToQ(const ObjId &oi, BindIndex bindIndex, const double *arg, int size){
  // we are passing threadId = 0, to the Qinfo ctor
  // since data in an ElementContainer is not accessible
  // to any other ElementContainers on the same PE/SMP node
  qBuf_.push_back(Qinfo(oi, bindIndex, lookupRegistrationIdx_, dBuf_.size(), size));
  //if(size > 0){
    //dBuf_.insert(dBuf_.end(), arg, arg + size);
  //}
  for(int i = 0; i < size; i++) dBuf_.push_back(arg[i]);
}


void ElementContainer::addToQ(const ObjId &oi, BindIndex bindIndex, const double *arg1, int size1, const double *arg2, int size2){
  qBuf_.push_back(Qinfo(oi, bindIndex, lookupRegistrationIdx_, dBuf_.size(), size1 + size2));
  //if(size1 > 0) dBuf_.insert(dBuf_.end(), arg1, arg1 + size1);
  for(int i = 0; i < size1; i++) dBuf_.push_back(arg1[i]);
  //if(size2 > 0) dBuf_.insert(dBuf_.end(), arg2, arg2 + size2);
  for(int i = 0; i < size2; i++) dBuf_.push_back(arg2[i]);
}

void ElementContainer::addDirectToQ(const ObjId& src, const ObjId& dest, FuncId fid, const double* arg, unsigned int size){

  CkVec< double >& vec = dBufDirect_;

  qBufDirect_.push_back(DirectQbufEntry(
                          Qinfo(src, Qinfo::DirectAdd, lookupRegistrationIdx_, vec.size(), size), 
                          ObjFid(dest, fid, size, 1)));
  //if ( size > 0 ) {
    //vec.insert( vec.end(), arg, arg + size );
  //}
  for(unsigned int i = 0; i < size; i++) vec.push_back(arg[i]);
}

void ElementContainer::addDirectToQ( const ObjId& src, const ObjId& dest, 
	FuncId fid, 
	const double* arg1, unsigned int size1,
        const double* arg2, unsigned int size2 ){

  CkVec< double >& vec = dBufDirect_;
  qBufDirect_.push_back(DirectQbufEntry(
                          Qinfo( src, Qinfo::DirectAdd, lookupRegistrationIdx_, vec.size(), size1 + size2 ),
                          ObjFid(dest, fid, size1 + size2, 1)));

  //if ( size1 > 0 ) vec.insert( vec.end(), arg1, arg1 + size1 );
  for(unsigned int i = 0; i < size1; i++) vec.push_back(arg1[i]);
  //if ( size2 > 0 ) vec.insert( vec.end(), arg2, arg2 + size2 );
  for(unsigned int i = 0; i < size2; i++) vec.push_back(arg2[i]);
}

void ElementContainer::addVecDirectToQ( const ObjId& src, const ObjId& dest, 
	FuncId fid, 
	const double* arg, unsigned int entrySize, unsigned int numEntries )
{
  if ( entrySize == 0 || numEntries == 0 )
    return;

  CkVec< double >& vec = dBufDirect_;

  qBufDirect_.push_back(DirectQbufEntry(
                          Qinfo(src, Qinfo::DirectAdd, lookupRegistrationIdx_, vec.size(), entrySize * numEntries),
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
  
  if(qBuf_.size() > 0) memcpy(msg->qinfo_, &qBuf_[0], numQinfo * sizeof(Qinfo));
  if(dBuf_.size() > 0) memcpy(msg->data_, &dBuf_[0], numData * sizeof(double));
  if(qBufDirect_.size() > 0) memcpy(msg->qinfoDirect_, &qBufDirect_[0], numDirectQinfo * sizeof(DirectQbufEntry));
  if(dBufDirect_.size() > 0) memcpy(msg->dataDirect_, &dBufDirect_[0], numDirectData * sizeof(double));

  return msg;
}

void ElementContainer::exchange(ElementDataMsg *m){
  nDataBcastsReceived_++;
  readBuf(m->qinfo_, m->nQinfo_, 
          m->qinfoDirect_, m->nQinfoDirect_, 
          m->data_, m->dataDirect_);
  delete m;

  if(nDataBcastsReceived_ == readonlyNumUsefulElementContainers){
    nDataBcastsReceived_ = 0;
    // when all expected bcasts have been received, 
    // synchronize and (conditionally) start next 
    // iteration
    shell_->containerCheckin();
  }
}

void ElementContainer::readBuf(Qinfo *qinfo, unsigned int nQinfo, 
                               DirectQbufEntry *qinfoDirect, unsigned int nQinfoDirect,
                               const double *data, const double *dataDirect){

  // All containers receive each message,
  // but only the workers should process it. Below,
  // we enforce this constraint. If we are not 
  // changing the DataHandler code (in particular 
  // DataHandler::execThread()), there is no way around
  // this hack. This is because the container assigned
  // to the shell (i.e. the container with reg. index
  // 0) is considered by BlockHandler's execThread method
  // to deliver the message to all the moose objects
  // assigned to this PE.
  if(lookupRegistrationIdx_ == 0) return;

  // first look at all indirect qinfo entries
  readIndirectBuf(qinfo, nQinfo, data);
  // then examine the direct qinfo entries, of type DirectQbufEntry
  readDirectBuf(qinfoDirect, nQinfoDirect, dataDirect);
}

void ElementContainer::readIndirectBuf(Qinfo *qinfo, unsigned int nQinfo, const double *data){
  for (unsigned int i = 0; i < nQinfo; ++i) {
    Qinfo &qi = qinfo[i];
    qi.setThreadNum(lookupRegistrationIdx_); 
    const Element* e = qi.src().element();
    if (e) e->exec(&qi, data + qi.dataIndex());
  }
}

void ElementContainer::readDirectBuf(DirectQbufEntry *qinfoDirect, unsigned int nQinfoDirect, const double *dataDirect){
  for(unsigned int i = 0; i < nQinfoDirect; i++){
    DirectQbufEntry &qi = qinfoDirect[i];
    qi.qinfo_.setThreadNum(lookupRegistrationIdx_); 
    const Element *e = qi.qinfo_.src().element();
    if(e) e->exec(&qi.qinfo_, &qi.ofid_, dataDirect + qi.qinfo_.dataIndex());
  }
}

void ElementContainer::hackClearQ(int nCycles){
  for(int i = 0; i < nCycles; i++){
    // FIXME remove copying overhead by having a double
    // buffer scheme with pointer swapping
    // copy into temporary buffers
    CkVec< Qinfo > qBuf(qBuf_);
    CkVec< DirectQbufEntry > qBufDirect(qBufDirect_);
    CkVec< double > data(dBuf_);
    CkVec< double > dataDirect(dBufDirect_);

    qBuf_.resize(0);
    dBuf_.resize(0);
    qBufDirect_.resize(0);
    dBufDirect_.resize(0);

    // unlike in readBuf(), here we don't discriminate
    // against the 0-th container on this PE, because
    // hackClearQ is used only in testing, and therefore
    // should always be done by the 0-th/test/shell container

    // read from temporary buffers, and write into
    // qBuf_, etc.
    readIndirectBuf(qBuf.getVec(), qBuf.size(), data.getVec());
    readDirectBuf(qBufDirect.getVec(), qBufDirect.size(), dataDirect.getVec());
  }
}

unsigned int ElementContainer::qSize() const {
  return qBuf_.size(); 
}

unsigned int ElementContainer::dSize() const {
  return dBuf_.size(); 
}

void ElementContainer::addToReduceQ(ReduceBase *r){
  reduceBuf_.push_back(r);
}

void ElementContainer::clearReduceQ(unsigned int numThreads){
  // need to do some primary combining and then submit to 
  // charm++ reduction, which should have a customized reducer
  // mind that the MPI version uses MPI_Allgather

  // This function doesn't do anything at all, since according to 
  // USB, at the moment there is no compelling use-case for reductions 
}

void ElementContainer::registerSelf(const CkCallback &cb){
  lookup_ = readonlyLookupHelperProxy.ckLocalBranch();
  lookupRegistrationIdx_ = lookup_->registerContainer(this);
  shell_ = lookup_->getShell();
  clock_ = lookup_->getClock();
  procInfo_.threadIndexInGroup = lookupRegistrationIdx_;
  contribute(cb);
}

void ElementContainer::start(){
  // if this is shell's container (allocated only 
  // because the unit tests require it) then we shouldn't
  // ask it to do any simulation work; instead, just have
  // it send an empty data message, which is required for
  // correct count of received messages
  if(procInfo_.threadIndexInGroup == 0) flushBufferedDataItems();
  else thisProxy[thisIndex].newIteration();
}

void ElementContainer::reinit(){
  if(procInfo_.threadIndexInGroup == 0) flushBufferedDataItems();
  else thisProxy[thisIndex].newIteration();
}

void ElementContainer::stop(){
  clock_->stop();
}

void ElementContainer::processPhase2(){
  clock_->processPhase2(&procInfo_);
}

ThreadId ElementContainer::getRegistrationIndex(){
  return lookupRegistrationIdx_;
}

