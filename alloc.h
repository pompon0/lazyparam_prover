#ifndef ALLOC_H_
#define ALLOC_H_

#include "types.h"
#include "log.h"

struct Snapshot {
  size_t blocks_used;
  u8 *begin,*end;
};

Snapshot stack{0,0,0};

inline u8* alloc_bytes(size_t s) {
  enum { BLOCK = 1<<20 };
  DEBUG if(s>BLOCK) error("% = s > BLOCK = %",s,BLOCK);
  static vec<u8*> blocks;
  if(stack.begin+s>stack.end){
    if(blocks.size()==stack.blocks_used) blocks.push_back(new u8[BLOCK]);
    stack.begin = blocks[stack.blocks_used++];
    stack.end = stack.begin+BLOCK;
  }
  auto ptr = stack.begin; stack.begin += s; return ptr;
}

inline u64* alloc(size_t s) {
  return (u64*)alloc_bytes(s*sizeof(u64));
}

template<typename T> inline T* alloc_init(const T &v) {
  return new(alloc_bytes(sizeof(T)))T(v);
}


#endif // ALLOC_H_
