load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "tptp_parser",
  strip_prefix = "tptp_parser-master",
  urls = ["https://github.com/pompon0/tptp_parser/archive/master.zip"],
)

http_archive(
	name = "com_google_protobuf",
	strip_prefix = "protobuf-3.6.1.3",
	urls = ["https://github.com/google/protobuf/archive/v3.6.1.3.zip"],
)

http_archive(
  name = "gtest",
  strip_prefix = "googletest-release-1.8.1",
  urls = ["https://github.com/google/googletest/archive/release-1.8.1.zip"],
)

http_archive(
  name = "abseil",
  strip_prefix = "abseil-cpp-master",
  urls = ["https://github.com/abseil/abseil-cpp/archive/master.zip"],
)

http_archive(
  name = "tptp_test_cnf_proto",
  urls = ["https://storage.googleapis.com/tptp/tptp_test_cnf_proto.tgz"],
  build_file = "tptp_test_cnf_proto.BUILD",
)

