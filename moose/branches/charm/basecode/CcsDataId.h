#ifndef CCS_DATA_ID_H
#define CCS_DATA_ID_H

#include "converse.h"
#include "pup.h"

struct CcsDataId {
  unsigned long long index_;

  CcsDataId() : 
    index_(-1)
  {}

  CcsDataId(unsigned long long index) : 
    index_(index)
  {}

  CcsDataId(unsigned long long data, unsigned int field, unsigned int numFieldBits) : 
    index_( ( data << numFieldBits ) + field )
  {}

  void pup(PUP::er &p){
    p|index_;
  }

  bool operator==(const CcsDataId &other) const {
    return (index_ == other.index_);
  }

  unsigned long long value() const {
    return index_;
  }

  static const CcsDataId bad(){
    static CcsDataId bad_( ~0U );
    return bad_;
  }
};

#endif // CCS_DATA_ID_H
