#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

using namespace std;
#include "Cinfo.h"
#include "Finfo.h"
#include "DummyFinfo.h"

// This file contains only those Cinfo definitions that
// are required by the parser.

static DummyFinfo dummy( 
		"dummy", 
		"This Finfo is a dummy. If you are reading this you have used an invalid index");



const Cinfo* Cinfo::find( const string& name )
{
	map<string, Cinfo*>::iterator i = cinfoMap().find(name);
	if ( i != cinfoMap().end() )
		return i->second;
	return 0;
}

const Cinfo* Cinfo::baseCinfo() const
{
	return baseCinfo_;
}

const std::string& Cinfo::name() const
{
	return name_;
}

string Cinfo::getDocs() const
{
    ostringstream doc;
    for (map <string, string>::const_iterator ii = doc_.begin(); ii != doc_.end(); ++ii){
        doc << ii->first << ":\t\t" << ii->second << endl;
    }
	return doc.str();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getSharedFinfo( unsigned int i )
{
	if ( i >= getNumSharedFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumSharedFinfo() )
			return sharedFinfos_[ i - baseCinfo_->getNumSharedFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getSharedFinfo( i );
	}

	return sharedFinfos_[i];
}

Finfo* Cinfo::getValueFinfo( unsigned int i )
{
	if ( i >= getNumValueFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumValueFinfo() )
			return valueFinfos_[ i - baseCinfo_->getNumValueFinfo() ];
		else
			return const_cast< Cinfo* >(baseCinfo_)->getValueFinfo( i );
	}

	return valueFinfos_[i];
}


////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getSrcFinfo( unsigned int i )
{
	if ( i >= getNumSrcFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumSrcFinfo() )
			return srcFinfos_[ i - baseCinfo_->getNumSrcFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getSrcFinfo( i );
	}

	return srcFinfos_[i];
}



////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getDestFinfo( unsigned int i )
{
	if ( i >= getNumDestFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumDestFinfo() )
			return destFinfos_[ i - baseCinfo_->getNumDestFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getDestFinfo( i );
	}

	return destFinfos_[i];
}

unsigned int Cinfo::getNumDestFinfo() const
{
	if ( baseCinfo_ )
		return destFinfos_.size() + baseCinfo_->getNumDestFinfo();
	else 
		return destFinfos_.size();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getLookupFinfo( unsigned int i )
{
	if ( i >= getNumLookupFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumLookupFinfo() )
			return lookupFinfos_[ i - baseCinfo_->getNumLookupFinfo() ];
		else
			return const_cast< Cinfo* >(baseCinfo_)->getLookupFinfo( i );
	}

	return lookupFinfos_[i];
}

unsigned int Cinfo::getNumLookupFinfo() const
{
	if ( baseCinfo_ )
		return lookupFinfos_.size() + baseCinfo_->getNumLookupFinfo();
	else 
		return lookupFinfos_.size();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getFieldElementFinfo( unsigned int i )
{
	if ( i >= getNumFieldElementFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumFieldElementFinfo() )
			return fieldElementFinfos_[ i - baseCinfo_->getNumFieldElementFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getFieldElementFinfo( i );
	}

	return fieldElementFinfos_[i];
}

unsigned int Cinfo::getNumFieldElementFinfo() const
{
	if ( baseCinfo_ )
		return fieldElementFinfos_.size() + baseCinfo_->getNumFieldElementFinfo();
	else 
		return fieldElementFinfos_.size();
}

// finfo::name

