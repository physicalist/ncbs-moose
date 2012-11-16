#ifndef MOOSE_SET_GET_WRAPPER_H
#define MOOSE_SET_GET_WRAPPER_H

#include "converse.h"
#include "pup.h"

template<typename A1>
struct SetGet1CcsWrapper {
  A1 a1_;
  bool hasData_;

  SetGet1CcsWrapper(A1 a1, bool hasData) : 
    a1_(a1),
    hasData_(hasData)
  {}

  SetGet1CcsWrapper() {}

  void pup(PUP::er &p){
    p | a1_;
    p | hasData_;
  }
};

template<typename A1, typename A2>
struct SetGet2CcsWrapper {
  A1 a1_;
  A2 a2_;
  bool hasData_;

  SetGet2CcsWrapper(A1 a1, A2 a2, bool hasData) : 
    a1_(a1),
    a2_(a2),
    hasData_(hasData)
  {}

  SetGet2CcsWrapper() {}

  void pup(PUP::er &p){
    p | a1_;
    p | a2_;
    p | hasData_;
  }
};



#endif // MOOSE_SET_GET_WRAPPER_H
