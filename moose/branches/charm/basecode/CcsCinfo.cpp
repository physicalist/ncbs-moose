#include "CcsCinfo.h"
#include <iostream>
#include <sstream>

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

DummyCcsFinfo CcsCinfo::dummy_;
