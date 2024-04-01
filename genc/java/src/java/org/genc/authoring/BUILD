package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_library(
    name = "constructor",
    srcs = [
        "Constructor.java",
    ],
    data = ["//genc/java/src/native/org/genc/authoring:libconstructor_jni.so"],
    deps = [
        "//genc/java/src/java/org/genc/runtime:to_from_value_proto",
        "//genc/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:com_google_guava_guava",
    ],
)
