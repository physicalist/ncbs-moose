#ifndef POINTER_CONTAINER_H
#define POINTER_CONTAINER_H

template <typename POINTER_TYPE>
struct PointerContainer {
  POINTER_TYPE *ptr;
  void pup(PUP::er &p){
    pup(p, (char **)&ptr, sizeof(char *));
  }
};

#endif // POINTER_CONTAINER_H
