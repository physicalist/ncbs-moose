#ifndef CCS_DATA_ID_H
#define CCS_DATA_ID_H

struct DataId {
  unsigned long long index_;

  DataId();

  void pup(PUP::er &p);
};

#endif // CCS_DATA_ID_H
