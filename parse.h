#ifndef PARSE_H_
#define PARSE_H_

#include "external/tptp_parser/proto/tptp.pb.h"
#include "google/protobuf/text_format.h"
#include "pred.h"

template<typename T> struct IntDict {
  size_t operator()(const T &v) {
    if(!m.count(v)){ auto x = m.size(); m[v] = x; }
    return m[v];
  }
  void clear(){ m.clear(); }
  size_t size(){ return m.size(); }
private:
  std::map<T,size_t> m;
};

struct ParseCtx {
  IntDict<str> pred_names;
  IntDict<str> fun_names;
  IntDict<str> var_names;

  Term parse_term(const tptp::Term &t) {
    FRAME("parse_term(%)",t.DebugString());
    switch(t.type()){
      case tptp::Term::VAR: {
        return Term(Var::make(var_names(t.name())));
      }
      case tptp::Term::EXP: {
        size_t ac = t.args().size();
        Fun::Builder b(fun_names(t.name()),ac);
        for(size_t i=0; i<ac; ++i)
          b.set_arg(i,parse_term(t.args()[i]));
        return Term(b.build());
      }
      default:
        error("unexpected t.type() = %",t.type());
    }
  }

  Atom parse_atom(const tptp::Formula &f) {
    FRAME("parse_atom(%)",f.DebugString());
    switch(f.formula_case()) {
    case tptp::Formula::kOp: {
      if(f.op().type()!=tptp::Formula::Operator::NEG)
        error("f.op().type() = %, want %",f.op().type(),tptp::Formula::Operator::NEG);
      if(f.op().args().size()!=1)
        error("f.op.args().size() = %, want %",f.op().args().size(),1);
      return parse_atom(f.op().args()[0]).neg();
    }
    case tptp::Formula::kPred: {
      switch(f.pred().type()) {
      case tptp::Formula::Pred::CUSTOM: {
        size_t ac = f.pred().args().size();
        Atom::Builder b(true,pred_names(f.pred().name()),ac,0);
        for(size_t i=0; i<ac; ++i)
          b.set_arg(i,parse_term(f.pred().args()[i]));
        return b.build();
      }
      case tptp::Formula::Pred::EQ: {
        if(f.pred().args().size()!=2)
          error("f.pred().args().size() = %, want %",f.pred().args().size(),2);
        return Atom::eq(true, parse_term(f.pred().args()[0]), parse_term(f.pred().args()[1]));
      }
      default:
        error("unexpected f.pred().type() = %",f.pred().type());
      }
    }
    default:
      error("unexpected f.formula_case() = %",f.formula_case());
    }
  }

  OrClause parse_orClause(const tptp::Formula &f) {
    FRAME("parse_orClause(%)",f.DebugString());
    var_names.clear();
    OrClause clause;
    switch(f.formula_case()) {
    case tptp::Formula::kOp: {
      switch(f.op().type()) {
        case tptp::Formula::Operator::OR: {
          for(const auto &a : f.op().args())
            clause.atoms.push_back(parse_atom(a));
          break;
        }
        case tptp::Formula::Operator::FALSE: break;
        default: error("unexpectedf f.op().type() = %",f.op().type());
      }
      break;
    }
    case tptp::Formula::kPred: {
      clause.atoms.push_back(parse_atom(f));
      break;
    }
    default:
      error("unexpected f.formula_case() = %",f.formula_case());
    }
    clause.var_count = var_names.size();
    return clause;
  }
};

NotAndForm parse_notAndForm(const tptp::File &file) {
  ParseCtx ctx;
  NotAndForm form;
  for(const tptp::Input &input : file.input()) {
    if(input.language()!=tptp::Input::CNF)
      error("input.language() = %, want CNF",input.language());
    switch(input.role()) {
    case tptp::Input::AXIOM:
    case tptp::Input::PLAIN:
    case tptp::Input::NEGATED_CONJECTURE: {
      form.or_clauses.push_back(ctx.parse_orClause(input.formula()));
      break;
    }
    default:
      error("unexpected input.role() = %",input.role());
    }
  }
  return form;
}

NotAndForm parse_notAndForm(const str &file_raw) {
  tptp::File file;
  if(!google::protobuf::TextFormat::ParseFromString(file_raw,&file)) {
    error("failed to parse input");
  }
  return parse_notAndForm(file);
}

#endif  // PARSE_H_

