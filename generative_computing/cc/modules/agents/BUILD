# Libraries for building agent-based applications.

package(
    default_visibility = [
        "//generative_computing/google:__subpackages__",
    ],
)

cc_library(
    name = "react",
    srcs = ["react.cc"],
    hdrs = ["react.h"],
    deps = [
        "//generative_computing/cc/intrinsics:custom_function",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "react_test",
    srcs = ["react_test.cc"],
    deps = [
        ":react",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)
