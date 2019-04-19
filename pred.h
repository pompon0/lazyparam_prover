#ifndef PRED_H_
#define PRED_H_

#include "types.h"
#include "alloc.h"

enum Type {
  TVAR,
  TFUN,
  ATOM,
};

struct Node {
  Node(u64 *_ptr) : ptr(_ptr) {}
  u64 *ptr;
  enum { TYPE, SIZE };
  u64 type(){ return ptr[TYPE]; }

  template<typename T> T cast(){ return ptr; }
};

// node which might be null
struct MaybeNode {
  u64 *ptr;
  MaybeNode(const Node &n) : ptr(n.ptr) {}
  MaybeNode() : ptr(0) {}
  operator bool(){ return ptr; }
  Node get(){ return ptr; }
};

struct TVar : Node {
  using Node::Node;
  enum { VAR = Node::SIZE, SIZE };
  u64 var(){ return ptr[VAR]; }
  
  static TVar make(u64 _var) {
    auto ptr = alloc(SIZE);
    ptr[TYPE] = TVAR;
    ptr[VAR] = _var;
    return ptr;
  }
};

struct TFun : Node {
  using Node::Node;
  enum { FUN = Node::SIZE, ARG_COUNT, ARGS };
  u64 fun(){ return ptr[FUN]; }
  u64 arg_count(){ return ptr[ARG_COUNT]; }
  inline Node arg(size_t i){ return ptr+ARGS+i; }

  struct Builder {
    u64 *ptr;
    Builder(u64 _fun, u64 _arg_count) : ptr(alloc(ARGS+_arg_count)) {
      ptr[TYPE] = TFUN;
      ptr[FUN] = _fun;
      ptr[ARG_COUNT] = _arg_count;
    }
    void set_arg(size_t i, Node a){ ptr[ARGS+i] = u64(a.ptr); }
    TFun build(){ return ptr; }
  };
};

inline bool has_var(Node t, u64 v) {
  switch(t.type()) {
  case TVAR: return t.cast<TVar>().var()==v;
  case TFUN: {
    auto tf = t.cast<TFun>();
    for(auto i=tf.arg_count(); i--;)
      if(has_var(tf.arg(i),v)) return 1;
    return 0;
  }
  default:
    //TODO: throw
    return 0; 
  }
}

struct Atom : Node {
  using Node::Node;
  enum { SIGN = Node::SIZE, PRED, ARG_COUNT, ARGS };
  bool sign(){ return ptr[SIGN]; }
  u64 pred(){ return ptr[PRED]; }
  u64 arg_count(){ return ptr[ARG_COUNT]; }
  u64* args(){ return ptr+ARGS; }
  inline Node arg(size_t i){ return ptr+ARGS+i; }

  static Atom make(bool _sign, u64 _pred, u64 _arg_count) {
    auto ptr = alloc(ARGS+_arg_count);
    ptr[TYPE] = ATOM;
    ptr[SIGN] = _sign;
    ptr[PRED] = _pred;
    ptr[ARG_COUNT] = _arg_count;
    return ptr;
  }

  Atom neg() {
    u64 *end = ptr+ARGS+arg_count();
    u64 *ptr2 = alloc(end-ptr);
    for(auto x = ptr, y = ptr2; x<end;) *x++ = *y++;
    ptr2[SIGN] = !sign();
    return ptr2;
  }
};

static_assert(sizeof(u64*)==sizeof(u64));
static_assert(sizeof(Node)==sizeof(u64*));
static_assert(sizeof(TVar)==sizeof(Node));
static_assert(sizeof(TFun)==sizeof(Node));
static_assert(sizeof(Atom)==sizeof(Node));

//TODO: replace with hash consing
inline bool operator!=(Node x, Node y);
inline bool operator==(Node x, Node y) {
  if(x.type()!=y.type()) return 0;
  switch(x.type()) {
  case TFUN: {
    auto xf = x.cast<TFun>();
    auto yf = y.cast<TFun>();
    if(xf.fun()!=yf.fun()) return 0;
    for(size_t i=xf.arg_count(); i--;)
      if(xf.arg(i)!=yf.arg(i)) return 0;
    return 1;
  }
  case TVAR:
    return x.cast<TVar>().var()==y.cast<TVar>().var();
  case ATOM: {
    auto xa = x.cast<Atom>();
    auto ya = y.cast<Atom>();
    if(xa.sign()!=ya.sign() || xa.pred()!=ya.pred()) return 0;
    for(size_t i=xa.arg_count(); i--;)
      if(xa.arg(i)!=ya.arg(i)) return 0;
    return 1;
  }
  }
  //TODO: throw error
  return 0;
}

inline bool operator!=(Node x, Node y) { return !(x==y); }

struct AndClause { vec<Atom> atoms; };
struct OrForm { vec<AndClause> and_clauses; };

struct OrClause { vec<Atom> atoms; };
struct NotAndForm { vec<OrClause> or_clauses; };

inline AndClause notOrClause(const OrClause &c) {
  AndClause d;
  for(auto a : c.atoms) d.atoms.push_back(a.neg());
  return d;
}

inline OrClause notAndClause(const AndClause &c) {
  OrClause d;
  for(auto a : c.atoms) d.atoms.push_back(a.neg());
  return d;
}

inline NotAndForm to_NotAndForm(const OrForm &f) {
  NotAndForm g;
  for(const auto &c : f.and_clauses)
      g.or_clauses.push_back(notAndClause(c));
  return g;
}

#endif // PRED_H_
