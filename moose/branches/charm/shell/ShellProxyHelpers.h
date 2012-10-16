#ifndef MOOSE_SHELL_PROXY_HELPERS_H
#define MOOSE_SHELL_PROXY_HELPERS_H

#include <string>
#include <vector>

using namespace std;

struct CreateStruct {
  string type_;
  Id parent_;
  string name_;
  vector<int> dimensions_;
  bool isGlobal_;
  bool qFlag_;

  CreateStruct() {}

  CreateStruct(string type, Id parent, string name, vector<int> dimensions, bool isGlobal, bool qFlag) : 
    type_(type),
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
  ObjId src_;
  string srcField_;
  ObjId dest_;
  string destField_;
  bool qFlag_;

  AddMsgStruct() {}

  AddMsgStruct(string msgType, ObjId &src, string srcField, ObjId dest, string destField, bool qFlag) : 
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
  Id orig_;
  Id newParent_;
  bool qFlag_;

  MoveStruct() {}

  MoveStruct(Id orig, Id newParent, bool qFlag) : 
    orig_(orig),
    newParent_(newParent),
    qFlag_(qFlag)
  {}
};

struct CopyStruct {
  Id orig_;
  Id newParent_;
  string newName_;
  unsigned int n_;
  bool toGlobal_;
  bool copyExtMsgs_;
  bool qFlag_;

  CopyStruct() {}

  CopyStruct(Id orig, Id newParent, string newName, unsigned int n, bool toGlobal, bool copyExtMsgs, bool qFlag) : 
    orig_(orig),
    newParent_(newParent),
    newName_(newName),
    n_(n),
    toGlobal_(toGlobal),
    copyExtMsgs_(copyExtMsgs),
    qFlag_(qFlag)
  {}

  void pup(PUP::er &p){
    PUP::sizer psz;
    psz | orig_;
    psz | newParent_;
    psz | newName_;
    psz | n_;
    psz | toGlobal_;
    psz | copyExtMsgs_;
    psz | qFlag_;
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
  Id parent_;
  Id child_;

  AdoptStruct() {}

  AdoptStruct(Id parent, Id child) : 
    parent_(parent),
    child_(child)
  {}

  void pup(PUP::er &p){
    p | parent_;
    p | child_;
  }
};

#endif // MOOSE_SHELL_PROXY_HELPERS_H
