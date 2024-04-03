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

java_library(
    name = "executor",
    srcs = ["Executor.java"],
    data = [
        "//genc/java/src/native/org/genc/runtime:libexecutor_jni.so",
        "//genc/java/src/native/org/genc/runtime:libowned_value_id_jni.so",
    ],
    deps = [
        ":owned_value_id",
        ":value_id",
        "//genc/proto/v0:computation_java_proto",
    ],
)

java_library(
    name = "owned_value_id",
    srcs = ["OwnedValueId.java"],
    data = [
        "//genc/java/src/native/org/genc/runtime:libexecutor_jni.so",
        "//genc/java/src/native/org/genc/runtime:libowned_value_id_jni.so",
    ],
    deps = [
        ":value_id",
    ],
)

java_library(
    name = "value_id",
    srcs = ["ValueId.java"],
)

java_library(
    name = "to_from_value_proto",
    srcs = ["ToFromValueProto.java"],
    deps = ["//genc/proto/v0:computation_java_proto"],
)
