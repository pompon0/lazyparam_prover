cc_proto_library(
  name = "tptp_cc_proto",
  deps = ["@tptp_parser//proto:tptp_proto"],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [":tptp_cc_proto"],
)
