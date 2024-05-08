package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "local_cache",
    srcs = ["local_cache.cc"],
    hdrs = ["local_cache.h"],
    deps = [
        "//genc/cc/intrinsics:custom_function",
        "//genc/proto/v0:computation_cc_proto",
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
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:inline_executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:threading",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "bi_map",
    hdrs = ["bi_map.h"],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "bi_map_test",
    srcs = ["bi_map_test.cc"],
    deps = [
        ":bi_map",
        "@com_google_googletest//:gtest_main",
    ],
)
