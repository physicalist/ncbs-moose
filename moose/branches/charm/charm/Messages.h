#ifndef MOOSE_MESSAGES_H
#define MOOSE_MESSAGES_H

#include "../basecode/Qinfo.h"
#include "DirectQbufEntry.h"
#include "moose.decl.h"

struct ElementDataMsg : public CMessage_ElementDataMsg {
  Qinfo *qinfo_;
  double *data_;

  DirectQbufEntry *qinfoDirect_;
  double *dataDirect_;

  unsigned int nQinfo_;
  unsigned int nQinfoDirect_;

  ElementDataMsg(unsigned int nQinfo, unsigned int nQinfoDirect) :
    nQinfo_(nQinfo),
    nQinfoDirect_(nQinfoDirect)
  {}
};

#endif // MOOSE_MESSAGES_H

