#include "CcsCinfo.h"
#include <iostream>
#include <sstream>

// define a deep-copy constructor: doesn't copy 
// baseCinfo_, since that cannot be set independently
// of other cinfos. see pymoose/moosemodule.cpp:defineAllClasses
void CcsCinfo::copy(const CcsCinfo &other){
  name_ = other.name_;
  doc_ = other.doc_;

  for(unsigned int i = 0; i < other.destFinfos_.size(); ++i){
    destFinfos_.push_back(new CcsFinfo(*other.destFinfos_[i]));
  }

  for(unsigned int i = 0; i < other.lookupFinfos_.size(); ++i){
    lookupFinfos_.push_back(new CcsFinfo(*other.lookupFinfos_[i]));
  }

  for(unsigned int i = 0; i < other.fieldElementFinfos_.size(); ++i){
    fieldElementFinfos_.push_back(new CcsFinfo(*other.fieldElementFinfos_[i]));
  }
}

CcsCinfo::CcsCinfo(){
}

CcsCinfo::CcsCinfo(const CcsCinfo &other){
  copy(other);
}

CcsCinfo &CcsCinfo::operator=(const CcsCinfo &other){
  copy(other);
  return *this;
}

map<string, CcsCinfo*>& CcsCinfo::cinfoMap()
{
	static map<std::string, CcsCinfo*> lookup_;
	return lookup_;
}

const CcsCinfo* CcsCinfo::find( const string& name )
{
	map<string, CcsCinfo*>::iterator i = cinfoMap().find(name);
	if ( i != cinfoMap().end() )
		return i->second;
	return 0;
}

const CcsCinfo* CcsCinfo::baseCinfo() const
{
	return baseCinfo_;
}

string CcsCinfo::getBaseClass() const 
{
	if ( baseCinfo_ )
		return baseCinfo_->name();
	else
		return "none";
}

const string &CcsCinfo::name() const
{
	return name_;
}

string CcsCinfo::getDocs() const
{
    ostringstream doc;
    for (map <string, string>::const_iterator ii = doc_.begin(); ii != doc_.end(); ++ii){
        doc << ii->first << ":\t\t" << ii->second << endl;
    }
	return doc.str();
}

CcsFinfo* CcsCinfo::getDestFinfo( unsigned int i )
{
	if ( i >= getNumDestFinfo() )
		return &dummy_;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumDestFinfo() )
			return destFinfos_[ i - baseCinfo_->getNumDestFinfo() ];
		else
			return const_cast< CcsCinfo* >( baseCinfo_ )->getDestFinfo( i );
	}

	return destFinfos_[i];
}

unsigned int CcsCinfo::getNumDestFinfo() const
{
	if ( baseCinfo_ )
		return destFinfos_.size() + baseCinfo_->getNumDestFinfo();
	else 
		return destFinfos_.size();
}

CcsFinfo* CcsCinfo::getLookupFinfo( unsigned int i )
{
	if ( i >= getNumLookupFinfo() )
		return &dummy_;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumLookupFinfo() )
			return lookupFinfos_[ i - baseCinfo_->getNumLookupFinfo() ];
		else
			return const_cast< CcsCinfo* >(baseCinfo_)->getLookupFinfo( i );
	}

	return lookupFinfos_[i];
}

unsigned int CcsCinfo::getNumLookupFinfo() const
{
	if ( baseCinfo_ )
		return lookupFinfos_.size() + baseCinfo_->getNumLookupFinfo();
	else 
		return lookupFinfos_.size();
}

////////////////////////////////////////////////////////////////////
CcsFinfo* CcsCinfo::getFieldElementFinfo( unsigned int i )
{
	if ( i >= getNumFieldElementFinfo() )
		return &dummy_;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumFieldElementFinfo() )
			return fieldElementFinfos_[ i - baseCinfo_->getNumFieldElementFinfo() ];
		else
			return const_cast< CcsCinfo* >( baseCinfo_ )->getFieldElementFinfo( i );
	}

	return fieldElementFinfos_[i];
}

unsigned int CcsCinfo::getNumFieldElementFinfo() const
{
	if ( baseCinfo_ )
		return fieldElementFinfos_.size() + baseCinfo_->getNumFieldElementFinfo();
	else 
		return fieldElementFinfos_.size();
}

#define PUP_VEC_CCSFINFO_PTRS(Name)\
{\
  int nFinfos;\
  if(!p.isUnpacking()){\
    nFinfos = Name##Finfos_.size();\
  }\
  p | nFinfos;\
  if(p.isUnpacking()){\
    Name##Finfos_.resize(nFinfos);\
    for(int i = 0; i < nFinfos; i++){\
      Name##Finfos_[i] = new CcsFinfo;\
    }\
  }\
  for(int i = 0; i < nFinfos; i++){\
    p | *Name##Finfos_[i];\
  }\
}

// before packing up the cinfos, we must change from pointer-based 
// addressing to an offset-based scheme for baseCinfo_. Similarly,
// after unpacking, we will use the local offset to get a pointer to
// the base cinfo.
void CcsCinfo::pup(PUP::er &p){
  int baseIndex;
  if(!p.isUnpacking()){
    baseIndex = (int)(baseCinfo_);
  }
  p | baseIndex;
  if(p.isUnpacking()){
    baseCinfo_ = (CcsCinfo *)(baseIndex);
  }

  p | name_;
  p | doc_;

  PUP_VEC_CCSFINFO_PTRS(dest)
  PUP_VEC_CCSFINFO_PTRS(lookup)
  PUP_VEC_CCSFINFO_PTRS(fieldElement)
}

void CcsCinfo::setBaseCinfoIndex(int i){
  baseCinfo_ = (CcsCinfo *)i;
}

void CcsCinfo::setBaseCinfoFromIndex(CcsCinfo *base){
  // interpret baseCinfo_ pointer as an integer offset
  // into an array (whose base addr is 'base') and get
  // proper c++ pointer to base cinfo
  int index = (int) baseCinfo_;
  if(index < 0){
    baseCinfo_ = NULL;
  }
  else{
    baseCinfo_ = base + ((int) baseCinfo_);
  }
}

void CcsCinfo::setBaseCinfo(CcsCinfo *base){
  baseCinfo_ = base;
}

vector< CcsFinfo * > &CcsCinfo::getDestFinfos(){
  return destFinfos_;
}

vector< CcsFinfo * > &CcsCinfo::getLookupFinfos(){
  return lookupFinfos_;
}

vector< CcsFinfo * > &CcsCinfo::getFieldElementFinfos(){
  return fieldElementFinfos_;
}

void CcsCinfo::setName(const string &name){
  name_ = name;
}

void CcsCinfo::setDoc(const map< string, string > &doc){
  doc_ = doc;
}

DummyCcsFinfo CcsCinfo::dummy_;
