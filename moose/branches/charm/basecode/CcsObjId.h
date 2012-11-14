#ifndef CCS_OBJID_H
#define CCS_OBJID_H

#include "charm++.h"
#include "pup.h"

#include "CcsId.h"
#include "CcsDataId.h"

struct CcsObjId {
  CcsId id;
  CcsDataId dataId;

  CcsObjId() : 
    id(),
    dataId()
  {}

  CcsObjId(CcsId i, CcsDataId d) : 
    id(i),
    dataId(d)
  {}

  CcsObjId(CcsId i) : 
    id(i),
    dataId(0)
  {}

  void pup(PUP::er &p){
    p|id;
    p|dataId;
  }

  void reduce(const CcsObjId &other){
    CkAssert(*this == other);
  }

  bool operator==(const CcsObjId &other){
    return (id == other.id && dataId == other.dataId);
  }
};

#endif // CCS_OBJID_H
