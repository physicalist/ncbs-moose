#ifndef MOOSE_SHELL_CCS_INTERFACE_OPERATION_H
#define MOOSE_SHELL_CCS_INTERFACE_OPERATION_H

#include <string>
using namespace std;


#include "conv-ccs.h"
class Shell;

class ShellCcsInterfaceOperation {
  private:
  string name_;

  protected:
  char *msg_;
  CcsDelayedReply delayedReply_;

  protected:
  ShellCcsInterfaceOperation(char *msg, CcsDelayedReply delayedReply, const string &name) : 
    msg_(msg),
    delayedReply_(delayedReply),
    name_(name)
  {}

  public:
  virtual void exec(Shell *shell) = 0;

  // flag returned tells whether the operation has 
  // completed, and it is ok to delete it. operations 
  // such as reinit and start do not complete right
  // away (i.e. upon return from exec)
  virtual bool done(){
    return true;
  }


  virtual ~ShellCcsInterfaceOperation(){
    CmiFree(msg_);
  }

  const string &getName() const {
    return name_;
  }

};

class SetCweOperation : public ShellCcsInterfaceOperation {
  public:
  SetCweOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class GetCweOperation : public ShellCcsInterfaceOperation {
  public:
  GetCweOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class CreateOperation : public ShellCcsInterfaceOperation {
  public:
  CreateOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class DeleteOperation : public ShellCcsInterfaceOperation {
  public:
  DeleteOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class AddMsgOperation : public ShellCcsInterfaceOperation {
  public:
  AddMsgOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class QuitOperation : public ShellCcsInterfaceOperation {
  public:
  QuitOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class StartOperation : public ShellCcsInterfaceOperation {
  public:
  StartOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}

  void exec(Shell *shell);
  void resume();
  bool done();

  public:
  // static method that is invoked as a callback for
  // split-phase methods such as doStart(), i.e. those
  // that have not yet completed upon return from call
  static void splitPhaseCallback(void *param, void *msg);


  public:
  static const string name_;
};

class ReinitOperation : public ShellCcsInterfaceOperation {
  public:
  ReinitOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);
  void resume();
  bool done();

  public:
  static void splitPhaseCallback(void *param, void *msg);

  public:
  static const string name_;
};

class StopOperation : public ShellCcsInterfaceOperation {
  public:
  StopOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class TerminateOperation : public ShellCcsInterfaceOperation {
  public:
  TerminateOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class MoveOperation : public ShellCcsInterfaceOperation {
  public:
  MoveOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class CopyOperation : public ShellCcsInterfaceOperation {
  public:
  CopyOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class FindOperation : public ShellCcsInterfaceOperation {
  public:
  FindOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class UseClockOperation : public ShellCcsInterfaceOperation {
  public:
  UseClockOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class LoadModelOperation : public ShellCcsInterfaceOperation {
  public:
  LoadModelOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class WriteSbmlOperation : public ShellCcsInterfaceOperation {
  public:
  WriteSbmlOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class SyncDataHandlerOperation : public ShellCcsInterfaceOperation {
  public:
  SyncDataHandlerOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class ReacDiffMeshOperation : public ShellCcsInterfaceOperation {
  public:
  ReacDiffMeshOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class SetClockOperation : public ShellCcsInterfaceOperation {
  public:
  SetClockOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class CleanSimulationOperation : public ShellCcsInterfaceOperation {
  public:
  CleanSimulationOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class AdoptOperation : public ShellCcsInterfaceOperation {
  public:
  AdoptOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class GetPathOperation : public ShellCcsInterfaceOperation {
  public:
  GetPathOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};


class GetObjIdPathOperation : public ShellCcsInterfaceOperation {
  public:
  GetObjIdPathOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class GetIsValidOperation : public ShellCcsInterfaceOperation {
  public:
  GetIsValidOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class WildcardOperation : public ShellCcsInterfaceOperation {
  public:
  WildcardOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class MsgMgrOperation : public ShellCcsInterfaceOperation {
  public:
  MsgMgrOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};

class CinfosOperation : public ShellCcsInterfaceOperation {
  public:
  CinfosOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply, name_)
  {}
  void exec(Shell *shell);

  public:
  static const string name_;
};




#endif // MOOSE_SHELL_CCS_INTERFACE_OPERATION_H
