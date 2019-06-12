#define DEBUG if(1)

#include <cstdio>
#include <iostream>
#include "util/read_file.h"
#include "tableau.h"
#include "pred.h"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(uint64_t,proof_size_limit,200,"maximal size of a proof to search for");

StdLogger _;
int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);

  auto file_raw_bytes = util::read_file(stdin);
  str file_raw(file_raw_bytes.begin(),file_raw_bytes.end());
  ParseCtx ctx;
  OrForm f(ctx.parse_notAndForm(file_raw));

  auto proof = prove_loop(f,absl::GetFlag(FLAGS_proof_size_limit));
  if(!proof) return 1;
  ProtoCtx pctx(ctx);
  std::cout << pctx.proto_notAndForm(NotAndForm(*proof)).DebugString() << std::endl;
  return 0;
}
