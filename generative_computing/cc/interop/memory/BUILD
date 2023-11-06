package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "local_cache",
    hdrs = ["local_cache.h"],
    deps = [
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "local_cache_test",
    srcs = ["local_cache_test.cc"],
    deps = [
        ":local_cache",
        "@com_google_googletest//:gtest_main",
    ],
)
