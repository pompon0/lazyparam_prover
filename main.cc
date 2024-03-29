#define DEBUG if(1)

#include <iostream>
#include "tableau.h"
#include "pred.h"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(uint64_t,proof_size_limit,200,"maximal size of a proof to search for");

StreamLogger _(std::cerr);
int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);

  str file_raw((std::istreambuf_iterator<char>(std::cin)), (std::istreambuf_iterator<char>()));
  ParseCtx ctx;
  OrForm f(ctx.parse_notAndForm(file_raw));

  auto proof = prove_loop(f,absl::GetFlag(FLAGS_proof_size_limit));
  if(!proof) return 1;
  ProtoCtx pctx(ctx);
  std::cout << pctx.proto_notAndForm(NotAndForm(*proof)).DebugString() << std::endl;
  return 0;
}
