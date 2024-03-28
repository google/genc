package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "constructor",
    srcs = [
        "Constructor.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/authoring/jni:constructor_jni",
        "//genc/java/src/java/org/genc/runtime:to_from_value_proto",
        "//genc/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_annotation_annotation",
        "@maven//:com_google_guava_guava",
    ],
)
