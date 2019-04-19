#ifndef ALLOC_H_
#define ALLOC_H_

#include "types.h"

inline u64* alloc(size_t s) {
  enum { BLOCK = 1<<20 };
  static u64 *begin = 0, *end = 0;
  if(begin+s>end){ begin = new u64[BLOCK]; end = begin+BLOCK; }
  auto ptr = begin; begin += s; return ptr;
}

#endif // ALLOC_H_
