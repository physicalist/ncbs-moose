#ifndef MOOSE_CCS_REDUCTION_WRAPPER_H
#define MOOSE_CCS_REDUCTION_WRAPPER_H

#include "../basecode/MsgId.h"

// This class only works for those wrapped 
// data types that do not require pup()ing, 
// namely primitive data types, contiguous structs,
// etc.

template<typename T>
struct CcsReductionWrapper {
  T data_;

  CcsReductionWrapper(const T &data) : 
    data_(data)
  {}

  CcsReductionWrapper<T> &operator+=(const CcsReductionWrapper<T> &other){
    data_ += other.data_;
    return *this;
  }
};

template<>
struct CcsReductionWrapper<bool> {
  bool data_;

  CcsReductionWrapper<bool> &operator+=(const CcsReductionWrapper<bool> &other){
    data_ &= other.data_;
    return *this;
  }
};

#endif // MOOSE_CCS_REDUCTION_WRAPPER_H
