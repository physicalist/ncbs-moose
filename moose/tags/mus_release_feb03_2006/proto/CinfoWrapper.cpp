#include "header.h"
typedef double ProcArg;
typedef int  SynInfo;
#include "Cinfo.h"
#include "CinfoWrapper.h"


Finfo* CinfoWrapper::fieldArray_[] =
{
///////////////////////////////////////////////////////
// Field definitions
///////////////////////////////////////////////////////
	new ReadOnlyValueFinfo< string >(
		"name", &CinfoWrapper::getName, "string" ),
	new ReadOnlyValueFinfo< string >(
		"author", &CinfoWrapper::getAuthor, "string" ),
	new ReadOnlyValueFinfo< string >(
		"description", &CinfoWrapper::getDescription, "string" ),
	new ReadOnlyValueFinfo< string >(
		"baseName", &CinfoWrapper::getBaseName, "string" ),
	new ReadOnlyArrayFinfo< string >(
		"fields", &CinfoWrapper::getFields, "string" ),
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// MsgDest definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Synapse definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Shared definitions
///////////////////////////////////////////////////////
};

const Cinfo CinfoWrapper::cinfo_(
	"Cinfo",
	"Upinder S. Bhalla, 2005, NCBS",
	"Cinfo: Cinfo class. Provides class information.",
	"Neutral",
	CinfoWrapper::fieldArray_,
	sizeof(CinfoWrapper::fieldArray_)/sizeof(Finfo *),
	&CinfoWrapper::create
);

///////////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////////

string CinfoWrapper::getFields(
	const Element* e , unsigned long index )
{
	const CinfoWrapper* f = static_cast< const CinfoWrapper* >( e );
	if ( f->fields_.size() > index )
		return f->fields_[ index ];
	return f->fields_[ 0 ];
}

///////////////////////////////////////////////////////
// Synapse function definitions
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Connection function definitions
///////////////////////////////////////////////////////
