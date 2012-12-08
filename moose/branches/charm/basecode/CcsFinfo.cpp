#include "CcsFinfo.h"

CcsFinfo::CcsFinfo(const string &name, const string &doc) : 
  name_(name),
  doc_(doc)
{}

string CcsFinfo::docs( ) const
{
	return doc_;
}

const string &CcsFinfo::name( ) const
{
	return name_;
}

// Silly variation needed to handle template expectations for
// name field.
string CcsFinfo::getName( ) const
{
	return name_;
}

DummyCcsFinfo::DummyCcsFinfo() : 
  CcsFinfo("dummy", "This is a dummy Finfo. If you're reading this, you have used a bad Finfo index.")
{}
