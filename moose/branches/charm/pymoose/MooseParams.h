#ifndef MOOSE_PARAMS_H
#define MOOSE_PARAMS_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class MooseParam {
  bool hasValue_;
  bool extractedValue_;

  public:
  MooseParam(bool hasValue) : 
    hasValue_(hasValue),
    extractedValue_(false)
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
};

template<typename ParamType>
class TypedMooseParam : public MooseParam {
  ParamType *saveTo_;
  ParamType parsedValue_;
  ParamType defaultValue_;

  public:
  TypedMooseParam(ParamType *saveTo, ParamType defaultValue, bool hasValue) : 
    MooseParam(hasValue), 
    saveTo_(saveTo),
    defaultValue_(defaultValue)
  {}

  void extract(const char *val);

  void push(){
    if(extractedValue()) *saveTo_ = parsedValue_;
    else *saveTo_ = defaultValue_;
  }
};

template<>
void TypedMooseParam<bool>::extract(const char *val){
  string valString(val);
  if(valString == "true"){
    parsedValue_ = true;  
  }
  else{
    parsedValue_ = false;
  }
}

template<>
void TypedMooseParam<int>::extract(const char *val){
  parsedValue_ = atoi(val);
}

template<>
void TypedMooseParam<unsigned int>::extract(const char *val){
  parsedValue_ = strtoul(val, NULL, 0);
}

template<>
void TypedMooseParam<float>::extract(const char *val){
  parsedValue_ = atof(val);
}

template<>
void TypedMooseParam<double>::extract(const char *val){
  parsedValue_ = strtod(val, NULL);
}

class MooseParamCollection {
  map<string, MooseParam *> params_;

  public:
  template<typename ParamType>
  void add(const string &description, ParamType *saveTo, ParamType defaultValue, bool hasValue){
    params_[description] = new TypedMooseParam<ParamType>(saveTo, defaultValue, hasValue);
  }

  void process(int argc, char **argv){
    map<string, MooseParam *>::iterator it;

    for(int i = 0; i < argc; ++i){
      string searchString(argv[i]);
      it = params_.find(searchString);

      if(it != params_.end()){
        MooseParam *param = it->second;
        if(param->hasValue()){
          assert(i+1 < argc);
          param->extract(argv[i+1]);
          param->hasExtractedValue();
          ++i;
        }
        else{
          param->extract("true");
          param->hasExtractedValue();
        }
      }
    }

    for(it = params_.begin(); it != params_.end(); ++it){
      it->second->push();
    }
  }
};

#endif // MOOSE_PARAMS_H
