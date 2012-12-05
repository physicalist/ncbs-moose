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

  private:
  // these are 'visitor' classes for param list
  // this one extracts all values required by the
  // parallel driver of moose
  class MooseParamExtractionWorker {
    public:
    void matchValue(int argi, int argc, char **argv, MooseParam *param){
      assert(argi + 1 < argc);
      param->extract(argv[argi + 1]);
    }

    void matchNoValue(int argi, int argc, char **argv, MooseParam *param){
      param->extract("true");
    }

    void noMatch(int argi, int argc, char **argv){
      cerr << "unrecognized parameter `" << argv[argi] << "'; skipping" << endl;
    }
  };
  
  // this one "purges" the arg list for moose-core,
  // i.e. it removes all args required only by the 
  // parallel driver, since those are not understood
  // by the core
  class MooseParamPurgeWorker {
    vector< string > *purged_;

    public:
    MooseParamPurgeWorker(vector< string > *purged) : 
      purged_(purged)
    {
    }

    void matchValue(int argi, int argc, char **argv, MooseParam *param){
    }

    void matchNoValue(int argi, int argc, char **argv, MooseParam *param){
    }

    void noMatch(int argi, int argc, char **argv){
      (*purged_).push_back(argv[argi]);
    }
  };
 


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

  bool contains(const char *description){
    return params_.find(MooseEnv::FlagPrefix_ + string(description)) != params_.end();
  }

  template<typename WorkerClass>
  bool traverse(int argc, char **argv, WorkerClass &worker){
    map<string, MooseParam *>::iterator it;

    // check the parameters for which the user has
    // supplied values
    for(int i = 1; i < argc; ++i){
      string searchString(argv[i]);
      it = params_.find(searchString);

      //CkPrintf("MooseParams: search %s found %d\n", searchString.c_str(), it != params_.end());

      if(it != params_.end()){
        MooseParam *param = it->second;
        if(param->hasValue()){
          //CkPrintf("MooseParams: matchValue %s\n", searchString.c_str());
          worker.matchValue(i, argc, argv, param);
          ++i;
        }
        else{
          //CkPrintf("MooseParams: matchNoValue %s\n", searchString.c_str());
          worker.matchNoValue(i, argc, argv, param);
        }
        param->hasExtractedValue();
      }
      else{
        //CkPrintf("MooseParams: noMatch %s\n", searchString.c_str());
        worker.noMatch(i, argc, argv);
      }
    }

    // set the value of the parameter, either to a
    // user-supplied value, or to the default;
    // complain if user has not supplied values for
    // compulsory parameters
    int nMissing = 0;
    for(it = params_.begin(); it != params_.end(); ++it){
      MooseParam *param = it->second;
      //CkPrintf("MooseParams: check param %s\n", it->first.c_str());
      if(param->isCompulsory() && !param->extractedValue()){
        //CkPrintf("MooseParams: parameter `%s' is compulsory and unspecified: fail\n", it->first.c_str());
        nMissing++;
      }
      else {
        //CkPrintf("MooseParams: parameter `%s' good\n", it->first.c_str());
        it->second->push();
      }
    }

    return (nMissing == 0);
  }

  bool process(int argc, char **argv){
    MooseParamExtractionWorker worker;
    return traverse(argc, argv, worker);
  }

  bool purge(int argc, char **argv, vector< string > &purgedArgs){
    MooseParamPurgeWorker worker(&purgedArgs);
    return traverse(argc, argv, worker);
  }
};

#endif // MOOSE_PARAMS_H
