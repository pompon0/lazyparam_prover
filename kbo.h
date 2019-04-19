#ifndef KBO_H_
#define KBO_H_

#include "util/short.h"
#include "pred.h"
#include "types.h"
#include <algorithm>

inline size_t max_var(Node t) {
  switch(t.type()) {
  case TVAR: return t.cast<TVar>().var();
  case TFUN: {
    auto tf = t.cast<TFun>();
    size_t h = 0; for(auto i=tf.arg_count(); i--;) util::maxi(h,max_var(tf.arg(i)));
    return h;
  }
  default: DEBUG error("unhandled t.type() = %",t.type());
  }
}

struct Balance {
  Balance(size_t var_count) : var_occ(var_count,0), pos(0), neg(0), weight(0) {}
  vec<int> var_occ;
  int pos;
  int neg;
  int weight;

  enum Res { L, G, E, N };
  static inline Res cmp(u64 l, u64 r) { return l<r ? L : l>r ? G : E; }
  
  inline void accum(Node t, int f) { FRAME("Balance.accum()");
    switch(t.type()) {
    case TVAR: {
      weight += f;
      auto &x = var_occ[t.cast<TVar>().var()], x2 = x+f;
      pos += (x2>0)-(x>0);
      neg += (x2<0)-(x<0);
      x = x2;
    }
    case TFUN: {
      auto tf = t.cast<TFun>();
      weight += f;
      for(auto i=tf.arg_count(); i--;) accum(tf.arg(i),f);
    }
    default: DEBUG error("unhandled t.type() = %",t.type());
    }
  }

  inline Res cmp_accum(Res lex) { FRAME("cmp_accum");
    if(pos && neg) return N;
    if(pos && !neg) return weight>0 || (weight==0 && lex==G) ? G : N;
    if(!pos && neg) return weight<0 || (weight==0 && lex==L) ? L : N;
    return weight<0 ? L : weight>0 ? G : lex;
  }
  
  inline Res cmp(Node l, Node r) { FRAME("Balance.cmp()");
    //TODO: replace with hash cons
    if(l.type()==TFUN && r.type()==TFUN && l!=r) {
      auto lf = l.cast<TFun>();
      auto rf = r.cast<TFun>();
      if(lf.fun()!=rf.fun()) {
        for(auto i=lf.arg_count(); i--;) accum(lf.arg(i),1);
        for(auto i=rf.arg_count(); i--;) accum(rf.arg(i),-1);
        return cmp_accum(cmp(lf.fun(),rf.fun()));
      } else {
        auto ac = lf.arg_count();
        //TODO: replace with hash cons
        size_t i=0; for(;i<ac; ++i) if(lf.arg(i)!=rf.arg(i)) break;
        Res lex = cmp(lf.arg(i),rf.arg(i));
        for(;i<ac; ++i){ accum(lf.arg(i),1); accum(rf.arg(i),-1); }
        return cmp_accum(lex);
      }
    } else {
      //TODO: replace with hash cons
      accum(l,1);
      accum(r,1);
      return l==r ? E : cmp_accum(N);
    }
  }
};

inline bool kbo(Node l, Node r) { FRAME("kbo()");
  return Balance(std::max(max_var(l),max_var(r))+1).cmp(l,r)==Balance::L;
}

#endif // KBO_H_
