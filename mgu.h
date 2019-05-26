#ifndef MGU_H_
#define MGU_H_

#include "types.h"
#include "pred.h"
#include "alloc.h"
#include "log.h"

struct Valuation {
  // there is NO cycles in valuation, even x -> x
  vec<Maybe<Term>> val;

  inline Var alloc_var() {
    val.emplace_back();
    return Var::make(val.size()-1);
  }

  inline bool assign(u64 v, Term t) { FRAME("MGU.assign()");
    // TODO: error, if already set
    switch(t.type()) {
      case Term::VAR: {
      Var tv(t);
      // break on trivial assignment
      if(tv.id()==v) return 1;
      // traverse TVar assignments
      if(auto mtv = val[tv.id()]) return assign(v,mtv.get());
      val[v] = t;
      return 1;
    }
    case Term::FUN: {
      if(has_var(t,v)) return 0;
      val[v] = t;
      return 1;
    }
    }
    error("MGU::assignt(v,<type=%>)",t.type());
    return 0;
  }

  inline bool mgu(Term x, Term y) { FRAME("mgu()");
    // TODO: add this iff hash consing is implemented
    // if(t1==t2) return 1;
    if(x.type()==Term::FUN && y.type()==Term::FUN) {
      Fun xf(x), yf(y);
      if(xf.fun()!=yf.fun()) return 0;
      auto ac = xf.arg_count();
      for(size_t i=0; i<ac; ++i)
        if(!mgu(xf.arg(i),yf.arg(i))) return 0;
      return 1;
    }
    if(x.type()!=Term::VAR && y.type()==Term::VAR) swap(x,y);
    if(x.type()==Term::VAR) {
      Var xv(x);
      if(auto mx = val[xv.id()]) return mgu(mx.get(),y);
      else return assign(xv.id(),y);
    }
    error("unhandled case (type %, type %)",x.type(),y.type());
    return 0;
  }

  inline Term eval(Term t) { FRAME("eval");
    switch(t.type()) {
    case Term::VAR: if(auto mv = val[Var(t).id()]) return eval(mv.get()); else return t;
    case Term::FUN: {
      Fun tf(t);
      size_t ac = tf.arg_count();
      Fun::Builder b(tf.fun(),ac);
      for(size_t i=0; i<ac; ++i) b.set_arg(i,eval(tf.arg(i)));
      return Term(b.build());
    }
    default: DEBUG error("unhandled t.type() = %",t.type());
    }
  }
};

#endif // MGU_H_
