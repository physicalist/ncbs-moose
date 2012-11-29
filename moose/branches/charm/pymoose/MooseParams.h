#ifndef MOOSE_PARAMS_H
#define MOOSE_PARAMS_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "../charm/MooseEnv.h"

using namespace std;

class MooseParam {
  bool hasValue_;
  bool extractedValue_;
  bool compulsory_;

  public:
  MooseParam(bool hasValue, bool compulsory) : 
    hasValue_(hasValue),
    extractedValue_(false),
    compulsory_(compulsory)
  {}


  virtual void extract(const char *val) = 0;
  virtual void push() = 0;

  bool hasValue() const {
    return hasValue_;
  }

  void hasExtractedValue(){
    extractedValue_ = true;
  }

  bool extractedValue() const {
    return extractedValue_;
  }

  bool isCompulsory() const {
    return compulsory_;
  }
};

template<typename ParamType>
class TypedMooseParam : public MooseParam {
  ParamType *saveTo_;
  ParamType parsedValue_;
  ParamType defaultValue_;

  public:
  TypedMooseParam(ParamType *saveTo, ParamType defaultValue, bool hasValue, bool compulsory) : 
    MooseParam(hasValue, compulsory), 
    saveTo_(saveTo),
    defaultValue_(defaultValue)
  {}

  TypedMooseParam(ParamType *saveTo, bool hasValue, bool compulsory) : 
    MooseParam(hasValue, compulsory), 
    saveTo_(saveTo)
  {}

  void extract(const char *val) {}

  void push(){
    if(extractedValue()) *saveTo_ = parsedValue_;
    else *saveTo_ = defaultValue_;
  }
};

#include <stdlib.h>
#include <assert.h>

template<>
inline void TypedMooseParam<bool>::extract(const char *val){
  string valString(val);
  if(valString == "true"){
    parsedValue_ = true;  
  }
  else{
    parsedValue_ = false;
  }
}

template<>
inline void TypedMooseParam<int>::extract(const char *val){
  parsedValue_ = atoi(val);
}

template<>
inline void TypedMooseParam<unsigned int>::extract(const char *val){
  parsedValue_ = strtoul(val, NULL, 0);
}

template<>
inline void TypedMooseParam<float>::extract(const char *val){
  parsedValue_ = atof(val);
}

template<>
inline void TypedMooseParam<double>::extract(const char *val){
  parsedValue_ = strtod(val, NULL);
}

template<>
inline void TypedMooseParam<string>::extract(const char *val){
  parsedValue_ = string(val);
}

class MooseParamCollection {
  map<string, MooseParam *> params_;

  public:
  template<typename ParamType>
  void add(const string &description, ParamType *saveTo, ParamType defaultValue, bool hasValue){
    // since this parameter has a default value, it is
    // not compulsory
    params_[MooseEnv::FlagPrefix_ + description] = new TypedMooseParam<ParamType>(saveTo, defaultValue, hasValue, false);
  }

  template<typename ParamType>
  void add(const string &description, ParamType *saveTo, bool hasValue){
    // since this parameter doesn't have a default 
    // value, it is compulsory
    params_[MooseEnv::FlagPrefix_ + description] = new TypedMooseParam<ParamType>(saveTo, hasValue, true);
  }

  bool process(int argc, char **argv){
    map<string, MooseParam *>::iterator it;

    // check the parameters for which the user has
    // supplied values
    for(int i = 0; i < argc; ++i){
      string searchString(argv[i]);
      it = params_.find(searchString);

      if(it != params_.end()){
        MooseParam *param = it->second;
        if(param->hasValue()){
          assert(i+1 < argc);
          param->extract(argv[i+1]);
          ++i;
        }
        else{
          param->extract("true");
        }
        param->hasExtractedValue();
      }
      else{
        cerr << "unrecognized parameter `" << searchString << "'; skipping" << endl;
      }
    }

    // set the value of the parameter, either to a
    // user-supplied value, or to the default;
    // complain if user has not supplied values for
    // compulsory parameters
    int nMissing = 0;
    for(it = params_.begin(); it != params_.end(); ++it){
      MooseParam *param = it->second;
      if(param->isCompulsory() && !param->extractedValue()){
        cerr << "parameter `" << it->first << "' is compulsory" << endl;
        nMissing++;
      }
      else {
        it->second->push();
      }
    }

    if(nMissing > 0) return false;
    else return true;
  }
};

#endif // MOOSE_PARAMS_H
