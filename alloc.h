#ifndef ALLOC_H_
#define ALLOC_H_

#include "types.h"
#include "log.h"

struct Snapshot {
  size_t blocks_used;
  u8 *begin,*end;
};

//TODO: zero the memory, when rewinding stack in DEBUG mode.
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

template<typename T> struct Maybe {
private:
  u8 data[sizeof(T)];
  bool present;
public:
  Maybe() : present(0) {}
  Maybe(const T &v) : present(1) { new(data)T(v); }
  explicit operator bool(){ return present; } 
  T get() {
    DEBUG if(!present) error("Maybe::get(): not present");
    return *(T*)data;
  }
};

template<typename E> struct List {
private:
  struct Node { Node *tail; E head; };
  Node *ptr;
  List(Node *_ptr) : ptr(_ptr) {}
public:
  List() : List(0) {}
  List(E h, List<E> t) : List(alloc_init(Node{t.ptr,h})) {}
  explicit List(E h) : List(h,List()) {}
  
  bool empty(){ return !ptr; }
  E head(){
    DEBUG if(empty()) error("<0>.head()");
    return ptr->head;
  }
  List<E> tail(){
    DEBUG if(empty()) error("<0>.tail()");
    return List(ptr->tail);
  }
  
  friend List<E> operator+(E h, List<E> t){ return List(h,t); }
  List<E>& operator+=(E h){ return *this = List(h,*this); }
};

#endif // ALLOC_H_
