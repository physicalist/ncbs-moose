#include "Finfo.h"
#include <string>
#include <typeinfo>
using namespace std;

Finfo::Finfo( const string& name, const string& doc )
	: name_( name ), doc_( doc )
{
	;
}

const string& Finfo::name( ) const
{
	return name_;
}

// Default virtual functions.
string Finfo::rttiType() const
{
	return typeid( *this ).name();
}

vector< string > Finfo::innerSrc() const
{
	static vector< string > ret;
	return ret;
}

vector< string > Finfo::innerDest() const
{
	static vector< string > ret;
	return ret;
}
