#ifndef CCS_FINFO_H
#define CCS_FINFO_H

#include <string>
using namespace std;

#include "pup_stl.h"

// Class that encapsulates all the components of Finfo
// that are required by the parser

class CcsFinfo {
  private:
  string name_;
  string doc_;

  public:
  CcsFinfo();
  CcsFinfo(const string &name, const string &doc);
  virtual ~CcsFinfo() {}

  /**
   * Returns the Finfo name
   */
  const string &name( ) const;
  /**
   * Returns string-ified form to handle template expectations
   * for name field
   */
  string getName() const;
  /**
   * Returns the documentation string for the Finfo
   */
  string docs() const;

  void pup(PUP::er &p);
};

class DummyCcsFinfo : public CcsFinfo {
  public:
  DummyCcsFinfo();
};

#endif // CCS_FINFO_H
