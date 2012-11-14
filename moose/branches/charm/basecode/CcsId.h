#ifndef CCS_ID_H
#define CCS_ID_H

#include "charm++.h"
#include "pup.h"

struct CcsId {
  unsigned int id_;

  CcsId(unsigned int id) : 
    id_(id)
  {}

  CcsId() : 
    id_(0)
  {}

  void pup(PUP::er &p){
    p|id_;
  }

  void reduce(const CcsId &other){
    CkAssert(*this == other);
  }

  bool operator==(const CcsId &other){
    return id_ == other.id_;
  }
};

#endif // CCS_ID_H
