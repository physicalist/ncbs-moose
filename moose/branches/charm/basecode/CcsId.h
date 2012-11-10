#ifndef CCS_ID_H
#define CCS_ID_H

struct CcsId {
  unsigned int id_;

  CcsId(unsigned int id_);
  CcsId();
  void pup(PUP::er &p);
  void reduce(const Id &other);
};

#endif // CCS_ID_H
