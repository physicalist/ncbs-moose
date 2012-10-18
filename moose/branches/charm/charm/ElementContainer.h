#ifndef ELEMENT_CONTAINER_H
#define ELEMENT_CONTAINER_H

#include <vector>
#include <string>
using namespace std;

#include "moose.decl.h"

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

#include "DirectQbufEntry.h"

class Shell;
class Clock;

class ElementContainer : public CBase_ElementContainer {
  ProcInfo procInfo_;

  CkVec<Qinfo> qBuf_;
  CkVec<double> dBuf_;

  CkVec<DirectQbufEntry> qBufDirect_;
  CkVec<double> dBufDirect_;

  CkVec<ReduceBase *> reduceBuf_;
  
  Shell *shell_;
  Clock *clock_;

  unsigned int nDataBcastsReceived_;

  public:
  // entry methods
  ElementContainer(const CkCallback &cb);
  ElementContainer(CkMigrateMessage *) {}

  void doSerialUnitTests(const CkCallback &cb);
  void newIteration();
  void iterationDone();
  void exchange(ElementDataMsg *m);

  void registerWithShell(const CkCallback &cb);


  // interface for moose objects
  public:
  // called by Qinfo::addToQ
  void addToQ(const ObjId &oi, BindIndex bindIndex, ThreadId threadNum, const double *arg, int size);
  void addToQ(const ObjId &oi, BindIndex bindIndex, ThreadId threadNum, const double *arg1, int size1, const double *arg2, int size2);

  void addDirectToQ(const ObjId& src, const ObjId& dest, ThreadId threadNum, FuncId fid, const double* arg, unsigned int size);
  void addDirectToQ(const ObjId& src, const ObjId& dest, ThreadId threadNum, FuncId fid, const double* arg1, unsigned int size1, const double* arg2, unsigned int size2);
  void addVecDirectToQ(const ObjId& src, const ObjId& dest, ThreadId threadNum, FuncId fid, const double* arg, unsigned int entrySize, unsigned int numEntries);

  void addToReduceQ(ReduceBase *r, ThreadId tid);
  void clearReduceQ(unsigned int numThreads);

  // called by Shell
  void start(double runTime);
  void stop();


  // HACK! this is to enable unit testing when messages
  // are not funneled through charm++.
  // It assumes that some messages have been enqueued
  // previously, and empties out the queue of messages,
  // delivering items to the objects dictated by the msgs
  // called during tests
  void hackClearQ(int nCycles=1);


  unsigned int qSize() const;
  unsigned int dSize() const;



  private:
  void flushBufferedDataItems();
  ElementDataMsg *obtainBcastMsgFromBuffers();
  void readBuf(Qinfo *qinfo, unsigned int nQinfo, 
               DirectQbufEntry *qinfoDirect, unsigned int nQinfoDirect,
               const double *data, const double *dataDirect);

};

#endif // ELEMENT_CONTAINER_H

