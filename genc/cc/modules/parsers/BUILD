# Libraries for parsing input and output of ops.

package(
    default_visibility = [
        #"//genc/google:__subpackages__",
        "//visibility:public",
    ],
)

licenses(["notice"])

cc_library(
    name = "gemini_parser",
    srcs = ["gemini_parser.cc"],
    hdrs = ["gemini_parser.h"],
    deps = [
        "//genc/cc/intrinsics:custom_function",
        "//genc/cc/modules/templates:inja_status_or",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@nlohmann_json//:json",
    ],
)

cc_test(
    name = "gemini_parser_test",
    srcs = ["gemini_parser_test.cc"],
    deps = [
        ":gemini_parser",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_googletest//:gtest_main",
        "@nlohmann_json//:json",
    ],
)

# cc_test(
#     name = "react_test",
#     srcs = ["react_test.cc"],
#     deps = [
#         ":react",
#         "//testing/base/public:gunit_main",
#         "//genc/proto/v0:computation_cc_proto",
#     ],
# )
