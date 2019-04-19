#ifndef MGU_H_
#define MGU_H_

#include "types.h"
#include "pred.h"
#include "alloc.h"

struct Valuation {
  // there is NO cycles in valuation, even x -> x
  vec<MaybeNode> val; // vec<Term>

  inline Node alloc_var() {
    val.emplace_back();
    return TVar::make(val.size()-1);
  }

  inline bool assign(u64 v, Node t) {
    // TODO: error, if already set
    switch(t.type()) {
    case TVAR: {
      auto tv = t.cast<TVar>();
      // break on trivial assignment
      if(tv.var()==v) return 1;
      // traverse TVar assignments
      if(auto mtv = val[tv.var()]) return assign(v,mtv.get());
      val[v] = t;
      return 1;
    }
    case TFUN: {
      if(has_var(t,v)) return 0;
      val[v] = t;
      return 1;
    }
    }
    // TODO: throw
    return 0;
  }

  inline bool mgu(Node/*Term*/ x, Node/*Term*/ y) {
    // TODO: add this iff hash consing is implemented
    // if(t1==t2) return 1;
    if(x.type()==TFUN && y.type()==TFUN) {
      auto xf = x.cast<TFun>();
      auto yf = y.cast<TFun>();
      if(xf.fun()!=yf.fun()) return 0;
      auto ac = xf.arg_count();
      for(size_t i=0; i<ac; ++i)
        if(!mgu(xf.arg(i),yf.arg(i))) return 0;
      return 1;
    }
    if(x.type()!=TVAR && y.type()==TVAR) swap(x,y);
    if(x.type()==TVAR) {
      auto xv = x.cast<TVar>();
      if(auto mx = val[xv.var()]) return mgu(mx.get(),y);
      else return assign(xv.var(),y);
    }
    // TODO: error
    return 0;
  }

  inline Node eval(Node t) {
    //TODO
    return alloc(1);
  }
};

#endif // MGU_H_
