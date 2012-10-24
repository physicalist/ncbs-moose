#ifndef MOOSE_SHELL_CCS_INTERFACE_OPERATION_H
#define MOOSE_SHELL_CCS_INTERFACE_OPERATION_H

#include "conv-ccs.h"
class Shell;

class ShellCcsInterfaceOperation {
  protected:
  char *msg_;
  CcsDelayedReply delayedReply_;

  protected:
  ShellCcsInterfaceOperation(char *msg, CcsDelayedReply delayedReply) : 
    msg_(msg),
    delayedReply_(delayedReply)
  {}

  public:
  virtual void exec(Shell *shell) = 0;

  virtual ~ShellCcsInterfaceOperation(){
    CmiFree(msg_);
  }

};

class SetCweOperation : public ShellCcsInterfaceOperation {
  public:
  SetCweOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class GetCweOperation : public ShellCcsInterfaceOperation {
  public:
  GetCweOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class CreateOperation : public ShellCcsInterfaceOperation {
  public:
  CreateOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class DeleteOperation : public ShellCcsInterfaceOperation {
  public:
  DeleteOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class AddMsgOperation : public ShellCcsInterfaceOperation {
  public:
  AddMsgOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class QuitOperation : public ShellCcsInterfaceOperation {
  public:
  QuitOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class StartOperation : public ShellCcsInterfaceOperation {
  public:
  StartOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}

  void exec(Shell *shell);
  void resume();

  public:
  // static method that is invoked as a callback for
  // split-phase methods such as doStart(), i.e. those
  // that have not yet completed upon return from call
  static void splitPhaseCallback(void *param, void *msg);

};

class ReinitOperation : public ShellCcsInterfaceOperation {
  public:
  ReinitOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class StopOperation : public ShellCcsInterfaceOperation {
  public:
  StopOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class TerminateOperation : public ShellCcsInterfaceOperation {
  public:
  TerminateOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class MoveOperation : public ShellCcsInterfaceOperation {
  public:
  MoveOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class CopyOperation : public ShellCcsInterfaceOperation {
  public:
  CopyOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class FindOperation : public ShellCcsInterfaceOperation {
  public:
  FindOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class UseClockOperation : public ShellCcsInterfaceOperation {
  public:
  UseClockOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class LoadModelOperation : public ShellCcsInterfaceOperation {
  public:
  LoadModelOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class WriteSbmlOperation : public ShellCcsInterfaceOperation {
  public:
  WriteSbmlOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class SyncDataHandlerOperation : public ShellCcsInterfaceOperation {
  public:
  SyncDataHandlerOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class ReacDiffMeshOperation : public ShellCcsInterfaceOperation {
  public:
  ReacDiffMeshOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class SetClockOperation : public ShellCcsInterfaceOperation {
  public:
  SetClockOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class CleanSimulationOperation : public ShellCcsInterfaceOperation {
  public:
  CleanSimulationOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};

class AdoptOperation : public ShellCcsInterfaceOperation {
  public:
  AdoptOperation(char *msg, CcsDelayedReply delayedReply) : 
    ShellCcsInterfaceOperation(msg, delayedReply)
  {}
  void exec(Shell *shell);
};


#endif // MOOSE_SHELL_CCS_INTERFACE_OPERATION_H
