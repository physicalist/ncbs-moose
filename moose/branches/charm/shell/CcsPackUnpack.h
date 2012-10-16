#ifndef MOOSE_CCS_PACK_UNPACK_H
#define MOOSE_CCS_PACK_UNPACK_H

template<typename T>
class CcsPackUnpack {
  public:
    static char *pack(T &t, unsigned int &size){
      // get total size of arguments
      PUP::sizer psz;
      psz | t;

      // allocate enough memory for serialized arguments
      char *serialized = new char[psz.size()];

      // serialize arguments into allocated buffer
      PUP::toMem pmem(serialized);
      pmem | t;

      return serialized;
    }

    static void unpack(char *msg, T &out){
      T *t = CcsPackUnpack<T>::cast(msg);
      PUP::fromMem pmem(t);
      pmem | out;
    }

    // utility function that extracts from a raw Converse 
    // message the data embedded within it. 
    static T *cast(const char *msg){
      CcsImplHeader *header = (CcsImplHeader *)(msg + CmiReservedHeaderSize);
      CkAssert(ChMessageInt(header->len) == sizeof(T));
      return (T *)(header + 1);
    }

};



#endif // MOOSE_CCS_PACK_UNPACK_H
