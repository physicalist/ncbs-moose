#ifndef CCS_CINFO_H
#define CCS_CINFO_H

#include <string>
#include <vector>
#include <map>

#include "CcsFinfo.h"

using namespace std;

// Class that encapsulates all the components of Cinfo
// that are required by the parser

class CcsCinfo {

  private:
  CcsCinfo *baseCinfo_;
  string name_;
  map< string, string > doc_;

  vector< CcsFinfo * > destFinfos_; 
  vector< CcsFinfo * > lookupFinfos_; 
  vector< CcsFinfo * > fieldElementFinfos_;

  public:
  const CcsCinfo *baseCinfo() const;
  string getBaseClass() const;
  const string &name() const;
  string getDocs() const;
  CcsFinfo* getDestFinfo( unsigned int i );
  unsigned int getNumDestFinfo() const;
  CcsFinfo* getLookupFinfo( unsigned int i );
  unsigned int getNumLookupFinfo() const;
  CcsFinfo* getFieldElementFinfo( unsigned int i );
  unsigned int getNumFieldElementFinfo() const;

  public:
  static const CcsCinfo *find(const string &name);
  static map< string, CcsCinfo * > &cinfoMap();  

  private:
  static DummyCcsFinfo dummy_;

};

#endif // CCS_CINFO_H
