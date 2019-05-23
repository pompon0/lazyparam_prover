#ifndef STACK_H_
#define STACK_H_

#include "alloc.h"
#include "pred.h"

struct Branch : Node {
  using Node::Node;
  enum { ATOM = Node::SIZE, UP, SIZE };
  Atom atom(){ return (u64*)ptr[ATOM]; }
  MaybeNode up(){ return (u64*)ptr[SIZE]; }

  static Branch make(Atom _atom, MaybeNode _up) {
    auto ptr = alloc(SIZE);
    ptr[TYPE] = BRANCH;
    ptr[ATOM] = u64(_atom.ptr);
    ptr[UP] = u64(_up.ptr);
    return ptr;
  }
};

struct Bud : Node {
  using Node::Node;
  enum { STRONG = Node::SIZE, BRANCH, NEXT, SIZE };
  bool strong(){ return ptr[STRONG]; }
  Branch branch(){ return (u64*)ptr[BRANCH]; }
  MaybeNode next(){ return (u64*)ptr[NEXT]; }

  static Bud make(bool _strong, Branch _branch, MaybeNode _next) {
    auto ptr = alloc(SIZE);
    ptr[TYPE] = BUD;
    ptr[BRANCH] = (u64)_branch.ptr;
    ptr[NEXT] = (u64)_next.ptr;
    return ptr;
  }
};

// any clause with literal
// any subterm
// any weak strategy
// any branch term
// any strong strategy

#endif  // STACK_H_
