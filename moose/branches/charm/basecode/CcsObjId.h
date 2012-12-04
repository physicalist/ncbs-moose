#ifndef CCS_OBJID_H
#define CCS_OBJID_H

#include "converse.h"
#include "pup.h"

#include "CcsId.h"
#include "CcsDataId.h"

#include <assert.h>

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
    assert(*this == other);
  }

  bool operator==(const CcsObjId &other) const {
    return (id == other.id && dataId == other.dataId);
  }

  std::string path(std::string separator = "/") const;

  static const CcsObjId bad();
};

#include <iostream>
std::ostream &operator<<(std::ostream &, CcsObjId &);

#endif // CCS_OBJID_H
