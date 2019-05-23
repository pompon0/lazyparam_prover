#define DEBUG if(1)

#include "types.h"
#include "pred.h"
#include "mgu.h"
#include "kbo.h"
#include "log.h"
#include "stack.h"

struct Proof {};

struct Ineq { Node l,r; };

struct TabState {
  Snapshot snapshot;
  Valuation mgu_state;
  vec<Ineq> ineqs;
  int nodes_used;
  int next_var;
  MaybeNode buds;
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

  void expand(TabState _tab){
    for(auto &cla : form.or_clauses) {
      TabState tab2 = _tab; // reset tab for variable allocation
      tab2.nodes_used++;
      auto buds = tab2.buds.get().cast<Bud>();
      auto branch = buds.branch();
      // TODO: alloc vars
      // each loop generates
      for(size_t i=0; i<cla.atoms.size(); ++i) {
        tabs.push_back(tab2);
        TabState &tab = tabs.back();
        // TODO: unify atom
        // Make the first bud strong
        tab.buds = Bud::make(1,Branch::make(cla.atoms[i],branch),buds);
        for(size_t j=0; j<cla.atoms.size(); ++j) if(i!=j) {
          tab.buds = Bud::make(0,Branch::make(cla.atoms[j],branch),tab.buds);
        }
        tab.snapshot = stack;
      }
    }
  }

  void step(){
    auto tab = tabs.back(); tabs.pop_back();
    if(!tab.buds) return; // success
    auto bud = tab.buds.get().cast<Bud>();
    stack = tab.snapshot;
    if(bud.strong()) {
      strong_pred(tab);
    } else {
      weak_pred(tab);
      expand(tab);
    }
  }

  void strong_pred(TabState tab) {
    auto bud = tab.buds.get().cast<Bud>();
    auto branch = bud.branch();
    auto a1 = branch.atom();
    auto a2 = branch.up().get().cast<Branch>().atom();
    if(a1.sign()==a2.sign()) return;
    if(a1.pred()!=a2.pred()) return;
    auto ac = a1.arg_count();
    for(size_t i=0; i<ac; ++i) {
      if(!tab.mgu_state.mgu(a1.arg(i),a2.arg(i))) return;
    }
    tab.snapshot = stack;
    tab.buds = bud.next();
    tabs.push_back(tab);
  }
  
  void weak_pred(TabState tab) {

  }
};

ptr<Proof> prove_loop(OrForm form, int limit) {
  for(int i=0; i<limit; ++i) {
    SearchState s(form,limit);
    //auto mproof = s.expand();
    //if(mproof) return mproof;
  }
  return 0;
}

StdLogger _;
int main() {}
