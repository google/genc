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
        "//generative_computing/java/src/java/org/generativecomputing/authoring/jni:constructor_jni",
        "//generative_computing/java/src/java/org/generativecomputing/runtime:to_from_value_proto",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_annotation_annotation",
        "@maven//:com_google_guava_guava",
    ],
)
