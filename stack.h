#ifndef STACK_H_
#define STACK_H_

#include "alloc.h"
#include "pred.h"

template<typename E> struct List {
private:
  struct Node { Node *tail; E head; };
  Node *ptr;
  List(Node *_ptr) : ptr(_ptr) {}
public:
  bool empty(){ return !ptr; }
  E head(){
    DEBUG if(empty()) error("<0>.head()");
    return ptr->head;
  }
  List<E> tail(){
    DEBUG if(empty()) error("<0>.tail()");
    return List(ptr->tail);
  }
  static List<E> make() { return List(0); }
  static List<E> make(E h, List<E> t) {
    return List(alloc_init(Node{t.ptr,h}));
  }
  friend List<E> operator+(E h, List<E> t){ return make(h,t); }
  List<E>& operator+=(E h){ return *this = make(h,*this); }
};

using Branch = List<Atom>;

struct Bud {
  bool strong;
  Branch branch;
};

// any clause with literal
// any subterm
// any weak strategy
// any branch term
// any strong strategy

#endif  // STACK_H_
