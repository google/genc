# C API for Oak.

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "oak_api",
    srcs = [],
    hdrs = ["oak_api.h"],
    deps = ["//genc/rust/interop/oak:oak_api"],
)

cc_test(
    name = "oak_api_test",
    srcs = ["oak_api_test.cc"],
    deps = [
        ":oak_api",
        "@com_google_googletest//:gtest_main",
    ],
)
