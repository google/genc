# Testing libraries that are shared across modules.
package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_test(
    name = "testing_libs_test",
    srcs = ["testing_libs_test.cc"],
    deps = [
        ":testing_libs",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "testing_libs",
    testonly = True,
    srcs = ["testing_libs.cc"],
    hdrs = ["testing_libs.h"],
    deps = [
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
