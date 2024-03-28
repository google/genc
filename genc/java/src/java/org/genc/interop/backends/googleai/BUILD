package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "google_ai_client",
    srcs = [
        "GoogleAiClient.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/interop/network:http_client",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto_lite",
        "//genc/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_code_gson_gson",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_guava_guava",
    ],
)
