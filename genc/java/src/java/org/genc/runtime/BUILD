# Java API for GenC.

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

java_library(
    name = "runner",
    srcs = ["Runner.java"],
    deps = [
        ":executor",
        ":owned_value_id",
        ":to_from_value_proto",
        "//genc/proto/v0:computation_java_proto_lite",
        "//genc/proto/v0:executor_java_proto_lite",
    ],
)

java_library(
    name = "executor",
    srcs = ["Executor.java"],
    deps = [
        ":owned_value_id",
        ":value_id",
        "//genc/java/src/java/org/genc/runtime/jni:executor_jni",
        "//genc/java/src/java/org/genc/runtime/jni:owned_value_id_jni",
        "//genc/proto/v0:computation_java_proto_lite",
        "//genc/proto/v0:executor_java_proto_lite",
    ],
)

java_library(
    name = "owned_value_id",
    srcs = ["OwnedValueId.java"],
    deps = [
        ":value_id",
        "//genc/java/src/java/org/genc/runtime/jni:executor_jni",
        "//genc/java/src/java/org/genc/runtime/jni:owned_value_id_jni",
        "//genc/proto/v0:computation_java_proto_lite",
        "//genc/proto/v0:executor_java_proto_lite",
    ],
)

java_library(
    name = "value_id",
    srcs = ["ValueId.java"],
)

java_library(
    name = "to_from_value_proto",
    srcs = ["ToFromValueProto.java"],
    deps = ["//genc/proto/v0:computation_java_proto_lite"],
)
