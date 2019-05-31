#define DEBUG if(1)

#include "types.h"
#include "pred.h"
#include "pred_format.h"
#include "mgu.h"
#include "kbo.h"
#include "log.h"
#include "stack.h"
#include "parse.h"
#include "util/read_file.h"
#include <cstdio>

struct Proof {};

struct Ineq { Term l,r; };

struct TabState {
  Snapshot snapshot;
  Valuation mgu_state;
  vec<Ineq> ineqs;
  int nodes_used;
  List<Bud> buds;
};

/*
struct TabState {
  TabState() : nodes_used(0), next_var(0) {}
  int nodes_used;
  int next_var;
  vec<Atom> branch;
  Valuation mgu_state;
  vec<Ineq> ineqs;

  inline bool validate_lt(Node l, Node r) {
    return !kbo(mgu_state.eval(r),mgu_state.eval(l));
  }

  inline bool validate_acyclic() {
    //TODO
    return 1;
  }

  inline bool add_eq(Node l, Node r) {
    if(!mgu_state.mgu(l,r)) return 0;
    for(auto &i : ineqs) if(!validate_lt(i.l,i.r)) return 0;
    return validate_acyclic();
  }

  inline bool add_lt(Node l, Node r) {
    ineqs.push_back({l,r});
    return validate_lt(l,r) && validate_acyclic();
  }
};
*/
struct SearchState {
  SearchState(OrForm _form, int _nodes_limit) : 
    form(to_NotAndForm(_form)),
    nodes_limit(_nodes_limit) {}
  
  //reductionOrder;
  const NotAndForm form;
  int nodes_limit;
  vec<TabState> tabs;

  void start() {
    
  }

  void expand(TabState _tab){
    for(auto cla : form.or_clauses) {
      TabState tab2 = _tab; // reset tab for variable allocation
      if(++tab2.nodes_used>nodes_limit) return;
      auto bud = tab2.buds.head();
      auto branch = bud.branch;
      // TODO: alloc variables 
      // each iteration generates an alternative
      for(size_t i=0; i<cla.atoms.size(); ++i) {
        tabs.push_back(tab2);
        TabState &tab = tabs.back();
        tab.buds = tab.buds.tail();
        // TODO: unify atom
        // Make the first bud strong
        tab.buds += Bud{1,cla.atoms[i]+branch};
        for(size_t j=0; j<cla.atoms.size(); ++j) if(i!=j) {
          tab.buds += Bud{0,cla.atoms[j]+branch};
        }
        tab.snapshot = stack;
      }
    }
  }

  bool step(){
    auto tab = tabs.back(); tabs.pop_back();
    if(tab.buds.empty()) return 1; // success
    auto bud = tab.buds.head();
    stack = tab.snapshot;
    if(bud.strong) {
      strong_pred(tab);
    } else {
      expand(tab);
      weak_pred(tab);
    }
    return 0;
  }

  void strong_pred(TabState tab) {
    auto bud = tab.buds.head();
    auto branch = bud.branch;
    auto a1 = branch.head();
    auto a2 = branch.tail().head();
    if(a1.sign()==a2.sign()) return;
    if(a1.pred()!=a2.pred()) return;
    auto ac = a1.arg_count();
    for(size_t i=0; i<ac; ++i) {
      if(!tab.mgu_state.mgu(a1.arg(i),a2.arg(i))) return;
    }
    tab.snapshot = stack;
    tab.buds = tab.buds.tail();
    tabs.push_back(tab);
  }
  
  void weak_pred(TabState _tab) {
    auto bud = _tab.buds.head();
    auto branch = bud.branch;
    auto a1 = branch.head();
    for(auto b2 = branch.tail(); !b2.empty(); b2 = b2.tail()) {
      auto tab = _tab;
      auto a2 = b2.head();
      if(a1.sign()==a2.sign()) return;
      if(a1.pred()!=a2.pred()) return;
      auto ac = a1.arg_count();
      bool ok = 1;
      for(size_t i=0; i<ac; ++i) {
        if(!tab.mgu_state.mgu(a1.arg(i),a2.arg(i))){ ok = 0; break; }
      }
      if(!ok) continue;
      tab.snapshot = stack;
      tab.buds = tab.buds.tail();
      tabs.push_back(tab);
    }
  }
};

ptr<Proof> prove_loop(OrForm form, int limit) {
  for(int i=0; i<limit; ++i) {
    SearchState s(form,limit);
    s.start();
    while(s.tabs.size()) {
      if(s.step()) return ptr<Proof>(new Proof{});
    }
  }
  return 0;
}



StdLogger _;
int main() {
  auto file_raw_bytes = util::read_file(stdin);
  str file_raw(file_raw_bytes.begin(),file_raw_bytes.end());
  auto f = parse_notAndForm(file_raw);
  info("%",show(f));
  info("ok");
  return 0;
}
