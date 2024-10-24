load("@rules_java//java:defs.bzl", "java_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "open_ai_client_android",
    srcs = [
        "OpenAiClient.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/interop/network:http_client_impl",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto_lite",
        "//genc/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_code_gson_gson",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_guava_guava",
    ],
)

java_library(
    name = "open_ai_client",
    srcs = [
        "OpenAiClient.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/interop/network:http_client",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto",
        "//genc/proto/v0:computation_java_proto",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_code_gson_gson",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_flogger_flogger_system_backend",
        "@maven//:com_google_guava_guava",
    ],
)
