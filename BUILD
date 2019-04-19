cc_proto_library(
  name = "tptp_cc_proto",
  deps = ["@tptp_parser//proto:tptp_proto"],
)

cc_library(
  name = "prover",
  hdrs = [
    "mgu.h",
    "types.h",
    "alloc.h",
    "pred.h",
    "log.h",
    "kbo.h",
  ],
)

cc_test(
  name = "mgu_test",
  srcs = ["mgu_test.cc"],
  deps = [
    ":prover",
    "//util:util",
    "@gtest//:gtest_main",
  ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
      ":prover",
      ":tptp_cc_proto",
    ],
)
