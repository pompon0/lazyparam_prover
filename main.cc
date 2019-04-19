#define DEBUG if(1)

#include "types.h"
#include "pred.h"
#include "mgu.h"
#include "kbo.h"
#include "log.h"

struct Proof {};

struct Ineq { Node l,r; };

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
};

struct SearchState {
  SearchState(OrForm _form, int _nodes_limit) : 
    form(to_NotAndForm(_form)),
    nodes_limit(_nodes_limit) {}
  
  //reductionOrder;
  const NotAndForm form;
  int nodes_limit;
  TabState tab;

  ptr<Proof> expand(){
    tab.nodes_used++;
    auto snapshot = tab;
    for(auto &cla : form.or_clauses) {
      tab = snapshot;
      // TODO: alloc vars
      auto snapshot = tab;
      for(size_t i=0; i<cla.atoms.size(); ++i) {
        tab = snapshot;
        // TODO: snapshot state
        tab.branch.push_back(cla.atoms[i]);
        auto strong_proof = strong();
        tab.branch.pop_back();
        if(!strong_proof) continue;

        for(size_t j=0; j<cla.atoms.size(); ++j) if(i!=j) {
          tab.branch.push_back(cla.atoms[j]);
          auto weak_proof = weak();
          tab.branch.pop_back();
          if(!weak_proof) break; //TODO: continue the external loop
        }
      }
    }
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
    SearchState s(form,limit);
    auto mproof = s.expand();
    if(mproof) return mproof;
  }
  return 0;
}

StdLogger _;
int main() {}
