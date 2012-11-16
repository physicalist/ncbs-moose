#ifndef CCS_ID_H
#define CCS_ID_H

#include "converse.h"
#include "pup.h"
#include <string>
#include <assert.h>

struct CcsId {
  unsigned int id_;

  CcsId() : 
    id_(0)
  {}

  CcsId(unsigned int id) : 
    id_(id)
  {}

  CcsId( std::string path, std::string separator = "/" );

  void pup(PUP::er &p){
    p|id_;
  }

  void reduce(const CcsId &other){
    assert(*this == other);
  }

  bool operator==(const CcsId &other) const {
    return id_ == other.id_;
  }

  bool operator!=(const CcsId &other) const {
    return id_ != other.id_;
  }

  std::string path(std::string separator = "/") const;

  static bool isValid(CcsId id);
  static bool isValid(unsigned int id){
    return isValid(CcsId(id));
  }

  unsigned int value() const {
    return id_;
  }

  bool operator<(const CcsId &other) const {
    return value() < other.value();
  }
};

#endif // CCS_ID_H
