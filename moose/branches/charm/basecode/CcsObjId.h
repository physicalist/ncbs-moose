#ifndef CCS_OBJID_H
#define CCS_OBJID_H

struct CcsObjId {
  CcsId id;
  CcsDataId dataId;

  void pup(PUP::er &p);
  void reduce(const CcsObjId &other);
};

#endif // CCS_OBJID_H
