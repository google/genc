# Java API for generative computing.

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

exports_files(
    srcs = [
        "Executor.java",
        "OwnedValueId.java",
        "Runner.java",
        "ToFromValueProto.java",
        "ValueId.java",
    ],
    visibility = ["//visibility:public"],
)

android_library(
    name = "runner",
    srcs = ["Runner.java"],
    deps = [
        ":executor",
        ":owned_value_id",
        ":to_from_value_proto",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "//generative_computing/proto/v0:executor_java_proto_lite",
    ],
)

android_library(
    name = "executor",
    srcs = ["Executor.java"],
    deps = [
        ":native",
        ":owned_value_id",
        ":value_id",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "//generative_computing/proto/v0:executor_java_proto_lite",
    ],
)

android_library(
    name = "owned_value_id",
    srcs = ["OwnedValueId.java"],
    deps = [
        ":native",
        ":value_id",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "//generative_computing/proto/v0:executor_java_proto_lite",
    ],
)

android_library(
    name = "value_id",
    srcs = ["ValueId.java"],
)

android_library(
    name = "to_from_value_proto",
    srcs = ["ToFromValueProto.java"],
    deps = ["//generative_computing/proto/v0:computation_java_proto_lite"],
)

cc_library(
    name = "native",
    srcs = [
        "//generative_computing/java/src/native:executor_jni.cc",
        "//generative_computing/java/src/native:owned_value_id_jni.cc",
    ],
    hdrs = [
        "//generative_computing/java/src/native:executor_jni.h",
        "//generative_computing/java/src/native:owned_value_id_jni.h",
    ],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//generative_computing/c/runtime:c_api",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
    ],
    alwayslink = 1,
)
