#ifndef MOOSE_SET_GET_WRAPPER_H
#define MOOSE_SET_GET_WRAPPER_H

#include <vector>
using namespace std;

#include "converse.h"
#include "pup_stl.h"

template<typename A1>
struct SetGet1CcsWrapperBase {
  A1 a1_;
  bool hasData_;

  SetGet1CcsWrapperBase(const A1 &a1, bool hasData) :
    a1_(a1),
    hasData_(hasData)
  {}

  SetGet1CcsWrapperBase() :
    hasData_(false)
  {}

  void pup(PUP::er &p){
    p | hasData_;
    if(hasData_) p | a1_;
  }
};

template<typename A1> 
struct SetGet1CcsWrapper : public SetGet1CcsWrapperBase < A1 > {
  SetGet1CcsWrapper(const A1 &a1, bool hasData) : 
    SetGet1CcsWrapperBase< A1 >(a1, hasData)
  {}

  SetGet1CcsWrapper() : 
    SetGet1CcsWrapperBase< A1 >() 
  {}

  SetGet1CcsWrapper< A1 > &operator+=(const SetGet1CcsWrapper< A1 > &other){
    // both cannot have data
    assert(!this->hasData_ || !other.hasData_);
    if(!this->hasData_) *this = other;
    return *this;
  }
};

// this specialization provides concatenation 
// for vector<A>s
template<typename A1> 
struct SetGet1CcsWrapper< vector< A1 > > : public SetGet1CcsWrapperBase < vector< A1 > > {
  SetGet1CcsWrapper(const vector< A1 > &a1, bool hasData) : 
    SetGet1CcsWrapperBase< vector< A1 > >(a1, hasData) 
  {}

  SetGet1CcsWrapper() : 
    SetGet1CcsWrapperBase< vector< A1 > >() 
  {}

  SetGet1CcsWrapper< vector< A1 > > &operator+=(const SetGet1CcsWrapper< vector< A1 > > &other){
    this->a1_.insert(this->a1_.end(), 
                 other.a1_.begin(), 
                 other.a1_.end());
    return *this;
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
    p | hasData_;
    if(hasData_){
      p | a1_;
      p | a2_;
    }
  }
};



#endif // MOOSE_SET_GET_WRAPPER_H
