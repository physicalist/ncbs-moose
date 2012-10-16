#ifndef MOOSE_DIRECT_QBUF_ENTRY_H
#define MOOSE_DIRECT_QBUF_ENTRY_H

struct DirectQbufEntry {
  Qinfo qinfo_;
  ObjFid ofid_;

  DirectQbufEntry(const Qinfo &qinfo, const ObjFid &ofid) : 
    qinfo_(qinfo),
    ofid_(ofid)
  {
  }

  DirectQbufEntry() {}
};


#endif // MOOSE_DIRECT_QBUF_ENTRY_H
