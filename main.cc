#include <cstdint>
#include <vector>
#include <memory>

using u64 = uint64_t;
template<typename T> using vec = std::vector<T>;
template<typename T> using ptr = std::unique_ptr<T>;

inline u64* alloc(size_t s) {
  enum { BLOCK = 1<<20 };
  static u64 *begin = 0, *end = 0;
  if(begin+s>end){ begin = new u64[BLOCK]; end = begin+BLOCK; }
  auto ptr = begin; begin += s; return ptr;
}

enum Type {
  TVAR = 0,
  TFUN = 1,
  ATOM = 2,
};

struct Node {
  Node(u64 *_ptr) : ptr(_ptr) {}
  u64 *ptr;
  enum { TYPE, SIZE };
  u64 type(){ return ptr[TYPE]; }
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
  u64* args(){ return ptr+ARGS; }

  static TVar make(u64 _fun, u64 _arg_count) {
    auto ptr = alloc(ARGS+_arg_count);
    ptr[TYPE] = TFUN;
    ptr[FUN] = _fun;
    ptr[ARG_COUNT] = _arg_count;
    return ptr;
  }
};

struct Atom : Node {
  using Node::Node;
  enum { SIGN = Node::SIZE, PRED, ARG_COUNT, ARGS };
  bool sign(){ return ptr[SIGN]; }
  u64 pred(){ return ptr[PRED]; }
  u64 arg_count(){ return ptr[ARG_COUNT]; }
  u64* args(){ return ptr+ARGS; }

  static Atom make(bool _sign, u64 _pred, u64 _arg_count) {
    auto ptr = alloc(ARGS+_arg_count);
    ptr[TYPE] = ATOM;
    ptr[SIGN] = _sign;
    ptr[PRED] = _pred;
    ptr[ARG_COUNT] = _arg_count;
    return ptr;
  }
};

struct OrForm {};

struct Proof {};

struct State {
  State(OrForm _form, int _nodes_limit) : 
    form(_form),
    nodes_limit(_nodes_limit),
    nodes_used(0),
    next_var(0) {}
  
  //reductionOrder;
  //NotAndForm clauses; 
  OrForm form;
  int nodes_limit;
  int nodes_used;
  int next_var;
  // set<Term,Term> ineq
  // mguState
  ptr<Proof> expand(){
    nodes_used++;
    // select clause
    //allButOne (pushAndCont weak) (pushAndCont strong);
    return 0;
  }
  ptr<Proof> strong() {
    return 0;
  }
  ptr<Proof> weak() {
    return 0;
  }
};

ptr<Proof> prove_loop(OrForm form, int limit) {
  for(int i=0; i<limit; ++i) {
    State s(form,limit);
    auto mproof = s.expand();
    if(mproof) return mproof;
  }
  return 0;
}

int main() {

}
