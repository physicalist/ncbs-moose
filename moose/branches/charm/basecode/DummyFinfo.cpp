#include <string>
#include <vector>
using namespace std;

class Cinfo;
class Eref;

#include "Finfo.h"
#include "DummyFinfo.h"

DummyFinfo::DummyFinfo(const string &name, const string &doc) : 
  Finfo(name, doc)
{}

DummyFinfo::~DummyFinfo(){
}

void DummyFinfo::registerFinfo(Cinfo *c){
}

bool DummyFinfo::strSet( const Eref& tgt, const string& field, const string& arg ) const {
}

bool DummyFinfo::strGet( const Eref& tgt, const string& field, string& returnValue ) const {
}
