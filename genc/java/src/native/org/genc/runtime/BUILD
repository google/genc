package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "executor_jni_lib",
    srcs = ["executor.cc"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/c/runtime:gc_buffer",
        "//genc/cc/runtime:executor",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@bazel_tools//tools/jdk:jni",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "libexecutor_jni.so",
    linkshared = True,
    deps = [":executor_jni_lib"],
)

cc_library(
    name = "owned_value_id_jni_lib",
    srcs = ["owned_value_id.cc"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/cc/runtime:executor",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@bazel_tools//tools/jdk:jni",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "libowned_value_id_jni.so",
    linkshared = True,
    deps = [":owned_value_id_jni_lib"],
)
