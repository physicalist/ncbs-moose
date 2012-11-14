#ifndef CCS_DATA_ID_H
#define CCS_DATA_ID_H

#include "charm++.h"
#include "pup.h"

struct CcsDataId {
  unsigned long long index_;

  CcsDataId() : 
    index_(-1)
  {}

  CcsDataId(unsigned long long index) : 
    index_(index)
  {}

  void pup(PUP::er &p){
    p|index_;
  }

  bool operator==(const CcsDataId &other){
    return (index_ == other.index_);
  }
};

#endif // CCS_DATA_ID_H
