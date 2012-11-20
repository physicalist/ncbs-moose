#ifndef MOOSE_SHELL_PROXY_HELPERS_H
#define MOOSE_SHELL_PROXY_HELPERS_H

#include <string>
#include <vector>

using namespace std;

#include "CcsId.h"
#include "CcsObjId.h"
#include "CcsDataId.h"

#include "pup_stl.h"

struct CreateStruct {
  string type_;
  CcsId parent_;
  string name_;
  vector<int> dimensions_;
  bool isGlobal_;
  bool qFlag_;

  CreateStruct() {}

  CreateStruct(string type, CcsId parent, string name, vector<int> dimensions, bool isGlobal, bool qFlag) : 
    type_(type),
    parent_(parent),
    name_(name),
    dimensions_(dimensions),
    isGlobal_(isGlobal),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | type_;
    p | parent_;
    p | name_;
    p | dimensions_;
    p | isGlobal_;
    p | qFlag_;
  }
};

struct AddMsgStruct {
  string msgType_;
  CcsObjId src_;
  string srcField_;
  CcsObjId dest_;
  string destField_;
  bool qFlag_;

  AddMsgStruct() {}

  AddMsgStruct(string msgType, CcsObjId &src, string srcField, CcsObjId dest, string destField, bool qFlag) : 
    msgType_(msgType),
    src_(src),
    srcField_(srcField),
    dest_(dest),
    destField_(destField),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | msgType_;
    p | src_;
    p | srcField_;
    p | dest_;
    p | destField_;
    p | qFlag_;
  }
};

struct MoveStruct {
  CcsId orig_;
  CcsId newParent_;
  bool qFlag_;

  MoveStruct() {}

  MoveStruct(CcsId orig, CcsId newParent, bool qFlag) : 
    orig_(orig),
    newParent_(newParent),
    qFlag_(qFlag)
  {}
};

struct CopyStruct {
  CcsId orig_;
  CcsId newParent_;
  string newName_;
  unsigned int n_;
  bool toGlobal_;
  bool copyExtMsgs_;
  bool qFlag_;

  CopyStruct() {}

  CopyStruct(CcsId orig, CcsId newParent, string newName, unsigned int n, bool toGlobal, bool copyExtMsgs, bool qFlag) : 
    orig_(orig),
    newParent_(newParent),
    newName_(newName),
    n_(n),
    toGlobal_(toGlobal),
    copyExtMsgs_(copyExtMsgs),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | orig_;
    p | newParent_;
    p | newName_;
    p | n_;
    p | toGlobal_;
    p | copyExtMsgs_;
    p | qFlag_;
  }
};

struct UseClockStruct {
  string path_;
  string field_;
  unsigned int tick_;
  bool qFlag_;

  UseClockStruct() {}

  UseClockStruct(string path, string field, unsigned int tick, bool qFlag) : 
    path_(path),
    field_(field),
    tick_(tick),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | path_;
    p | field_;
    p | tick_;
    p | qFlag_;
  }
};

struct LoadModelStruct {
  string fname_;
  string modelpath_;
  string solverClass_;
  bool qFlag_;

  LoadModelStruct() {}

  LoadModelStruct(string fname, string modelpath, string solverClass, bool qFlag) : 
    fname_(fname),
    modelpath_(modelpath),
    solverClass_(solverClass),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | fname_;
    p | modelpath_;
    p | solverClass_;
    p | qFlag_;
  }
};

struct WriteSbmlStruct {
  string fname_;
  string modelpath_;
  bool qFlag_;

  WriteSbmlStruct() {}

  WriteSbmlStruct(string fname, string modelpath, bool qFlag) : 
    fname_(fname),
    modelpath_(modelpath),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | fname_;
    p | modelpath_;
    p | qFlag_;
  }
};

struct SetClockStruct {
  unsigned int tickNum_;
  double dt_;
  bool qFlag_;

  SetClockStruct() {}

  SetClockStruct(unsigned int tickNum, double dt, bool qFlag) : 
    tickNum_(tickNum),
    dt_(dt),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    p | tickNum_;
    p | dt_;
    p | qFlag_;
  }
};

struct AdoptStruct {
  CcsId parent_;
  CcsId child_;

  AdoptStruct() {}

  AdoptStruct(CcsId parent, CcsId child) : 
    parent_(parent),
    child_(child)
  {}

  void pup(PUP::er &p){
    p | parent_;
    p | child_;
  }
};

#endif // MOOSE_SHELL_PROXY_HELPERS_H
