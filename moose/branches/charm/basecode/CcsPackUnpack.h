#ifndef MOOSE_CCS_PACK_UNPACK_H
#define MOOSE_CCS_PACK_UNPACK_H

#include "converse.h"
#include "conv-ccs.h"
#include "pup.h"
#include "pup_stl.h"

#include <assert.h>

template<typename T>
class CcsPackUnpack {
  public:
    static char *pack(T &t, unsigned int &size){
      // get total size of arguments
      PUP::sizer psz;
      psz | t;

      size = psz.size();
      // allocate enough memory for serialized arguments
      char *serialized = new char[size];

      // serialize arguments into allocated buffer
      PUP::toMem pmem(serialized);
      pmem | t;

      return serialized;
    }

    // to unpack data embedded within messages 
    // received in ccs handlers
    static void unpackHandler(char *msg, T &out){
      char *data = (char *) extractHandler(msg);
      unpack(data, out);
    }

    // utility function that extracts from a ccs 
    // message the data embedded within it.
    // meant for use in handlers.
    static T *extractHandler(char *msg){
      //CcsImplHeader *header = (CcsImplHeader *)(msg + CmiReservedHeaderSize);
      //assert(ChMessageInt(header->len) == sizeof(T));
      return (T *) (msg + CmiReservedHeaderSize);
      //return (T *)(header + 1);
    }

    static void unpackReply(char *msg, T &out){
      char *data = (char *) extractReply(msg);
      unpack(data, out);
    } 

    // get data from ccs message
    static T *extractReply(char *msg){
      return (T *) msg;
    }

  private:
    static void unpack(char *data, T &out){
      PUP::fromMem pmem(data);
      pmem | out;
    }

};



#endif // MOOSE_CCS_PACK_UNPACK_H
