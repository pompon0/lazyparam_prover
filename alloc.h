#ifndef ALLOC_H_
#define ALLOC_H_

#include "types.h"
#include "log.h"

struct Snapshot {
  size_t blocks_used;
  u64 *begin,*end;
};

Snapshot stack{0,0,0};

inline u64* alloc(size_t s) {
  enum { BLOCK = 1<<20 };
  DEBUG if(s>BLOCK) error("% = s > BLOCK = %",s,BLOCK);
  static vec<u64*> blocks;
  if(stack.begin+s>stack.end){
    if(blocks.size()==stack.blocks_used) blocks.push_back(new u64[BLOCK]);
    stack.begin = blocks[stack.blocks_used++];
    stack.end = stack.begin+BLOCK;
  }
  auto ptr = stack.begin; stack.begin += s; return ptr;
}



#endif // ALLOC_H_
