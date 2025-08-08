load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

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
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "testing_libs",
    testonly = True,
    srcs = ["testing_libs.cc"],
    hdrs = ["testing_libs.h"],
    deps = [
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
