package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "executor_jni",
    srcs = ["executor_jni.cc"],
    hdrs = ["executor_jni.h"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/c/runtime:gc_buffer",
        "//genc/cc/runtime:executor",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@bazel_tools//tools/jdk:jni",
    ],
    alwayslink = 1,
)

cc_library(
    name = "owned_value_id_jni",
    srcs = ["owned_value_id_jni.cc"],
    hdrs = ["owned_value_id_jni.h"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/cc/runtime:executor",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@bazel_tools//tools/jdk:jni",
    ],
    alwayslink = 1,
)
