package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "local_cache",
    srcs = ["local_cache.cc"],
    hdrs = ["local_cache.h"],
    deps = [
        "//generative_computing/cc/intrinsics:custom_function",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "local_cache_test",
    srcs = ["local_cache_test.cc"],
    deps = [
        ":local_cache",
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/intrinsics:handler_sets",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:inline_executor",
        "//generative_computing/cc/runtime:runner",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)
