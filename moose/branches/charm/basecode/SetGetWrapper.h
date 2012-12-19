#ifndef MOOSE_SET_GET_WRAPPER_H
#define MOOSE_SET_GET_WRAPPER_H

#include <vector>
#include <iostream>
using namespace std;

#include "converse.h"
#include "pup_stl.h"
#include "CcsId.h"
#include "CcsObjId.h"

template<typename A1>
struct SetGet1CcsWrapperBase {
  A1 a1_;
  bool hasData_;
  int pe_;

  SetGet1CcsWrapperBase(const A1 &a1, bool hasData, int pe) :
    a1_(a1),
    hasData_(hasData),
    pe_(pe)
  {}

  SetGet1CcsWrapperBase() :
    hasData_(false),
    pe_(-1)
  {}

  void pup(PUP::er &p){
    p | pe_;
    p | hasData_;
    if(hasData_) p | a1_;
  }
};


template<typename A1> 
struct SetGet1CcsWrapper : public SetGet1CcsWrapperBase < A1 > {
  SetGet1CcsWrapper(const A1 &a1, bool hasData, int pe) : 
    SetGet1CcsWrapperBase< A1 >(a1, hasData, pe)
  {}

  SetGet1CcsWrapper() : 
    SetGet1CcsWrapperBase< A1 >() 
  {}

  SetGet1CcsWrapper< A1 > &operator+=(const SetGet1CcsWrapper< A1 > &other){
    // XXX relax this assumption for now. otherwise, doFind() fails when 
    // querying for objects from their paths.
    // both cannot have data
    //assert(!this->hasData_ || !other.hasData_);
    if(!this->hasData_) *this = other;
    return *this;
  }
};

template<typename A1>
std::ostream &operator<<(std::ostream &out, SetGet1CcsWrapper< A1 > &wrap){
  if(wrap.hasData_){
    out << "pe: " << wrap.pe_ << " (" << wrap.a1_ << ")";
  }
  else{
    out << "pe: " << wrap.pe_ << " (.)";
  }
  return out;
}

// this specialization provides concatenation 
// for vector<A>s
template<typename A1> 
struct SetGet1CcsWrapper< vector< A1 > > : public SetGet1CcsWrapperBase < vector< A1 > > {
  SetGet1CcsWrapper(const vector< A1 > &a1, bool hasData, int pe) : 
    SetGet1CcsWrapperBase< vector< A1 > >(a1, hasData, pe) 
  {}

  SetGet1CcsWrapper() : 
    SetGet1CcsWrapperBase< vector< A1 > >() 
  {}

  SetGet1CcsWrapper< vector< A1 > > &operator+=(const SetGet1CcsWrapper< vector< A1 > > &other){
    this->a1_.insert(this->a1_.end(), 
                 other.a1_.begin(), 
                 other.a1_.end());
    if(other.hasData_ && !this->hasData_){
      this->hasData_ = true;
      this->pe_ = other.pe_;
    }
    return *this;
  }
};

template<typename A1>
std::ostream &operator<<(std::ostream &out, SetGet1CcsWrapper< vector< A1 > > &wrap){
  if(wrap.hasData_){
    out << "pe: " << wrap.pe_ << " [";
    if(wrap.a1_.size() > 0){
      for(int i = 0; i < wrap.a1_.size() - 1; i++){ 
        out << wrap.a1_[i] << ", ";
      }
      out << wrap.a1_[wrap.a1_.size() - 1];
    }
    out << "]";
  }
  else{
    out << "pe: " << wrap.pe_ << " [.]";
  }
  return out;
}



template<typename A1, typename A2>
struct SetGet2CcsWrapper {
  A1 a1_;
  A2 a2_;
  bool hasData_;
  int pe_;

  SetGet2CcsWrapper(A1 a1, A2 a2, bool hasData, int pe) : 
    a1_(a1),
    a2_(a2),
    hasData_(hasData),
    pe_(pe)
  {}

  SetGet2CcsWrapper() {}

  void pup(PUP::er &p){
    p | pe_;
    p | hasData_;
    if(hasData_){
      p | a1_;
      p | a2_;
    }
  }
};

template<typename A1, typename A2>
std::ostream &operator<<(std::ostream &out, SetGet2CcsWrapper< A1, A2 > &wrap){
  if(wrap.hasData_){
    out << "pe: " << wrap.pe_ << " (" << wrap.a1_ << ", " << wrap.a2_ << ")";
  }
  else{
    out << "pe: " << wrap.pe_ << " (.)";
  }
  return out;
}

#endif // MOOSE_SET_GET_WRAPPER_H
