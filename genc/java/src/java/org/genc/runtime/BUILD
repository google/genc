# Java API for GenC.

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_library(
    name = "runner",
    srcs = ["Runner.java"],
    deps = [
        ":executor",
        ":owned_value_id",
        ":to_from_value_proto",
        "//genc/proto/v0:computation_java_proto",
    ],
)

android_library(
    name = "runner_android",
    srcs = ["Runner.java"],
    deps = [
        ":executor_android",
        ":owned_value_id_android",
        ":to_from_value_proto_android",
        "//genc/proto/v0:computation_java_proto_lite",
    ],
)

java_library(
    name = "executor",
    srcs = ["Executor.java"],
    deps = [
        ":owned_value_id",
        ":value_id",
        "//genc/java/src/native/org/genc/runtime:libexecutor_jni_lib",
        "//genc/java/src/native/org/genc/runtime:libowned_value_id_jni_lib",
        "//genc/proto/v0:computation_java_proto",
    ],
)

android_library(
    name = "executor_android",
    srcs = ["Executor.java"],
    deps = [
        ":owned_value_id_android",
        ":value_id_android",
        "//genc/java/src/native/org/genc/runtime:libexecutor_jni_lib",
        "//genc/java/src/native/org/genc/runtime:libowned_value_id_jni_lib",
        "//genc/proto/v0:computation_java_proto_lite",
    ],
)

java_library(
    name = "owned_value_id",
    srcs = ["OwnedValueId.java"],
    deps = [
        ":value_id",
        "//genc/java/src/native/org/genc/runtime:libexecutor_jni_lib",
        "//genc/java/src/native/org/genc/runtime:libowned_value_id_jni_lib",
    ],
)

android_library(
    name = "owned_value_id_android",
    srcs = ["OwnedValueId.java"],
    deps = [
        ":value_id_android",
        "//genc/java/src/native/org/genc/runtime:libexecutor_jni_lib",
        "//genc/java/src/native/org/genc/runtime:libowned_value_id_jni_lib",
    ],
)

java_library(
    name = "value_id",
    srcs = ["ValueId.java"],
)

android_library(
    name = "value_id_android",
    srcs = ["ValueId.java"],
)

java_library(
    name = "to_from_value_proto",
    srcs = ["ToFromValueProto.java"],
    deps = ["//genc/proto/v0:computation_java_proto"],
)

android_library(
    name = "to_from_value_proto_android",
    srcs = ["ToFromValueProto.java"],
    deps = ["//genc/proto/v0:computation_java_proto_lite"],
)
