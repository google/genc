package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "open_ai_client",
    srcs = [
        "OpenAiClient.java",
    ],
    deps = [
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:http_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network/api/proto:http_request_response_java_proto_lite",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_code_gson_gson",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_flogger_flogger_system_backend",
        "@maven//:com_google_guava_guava",
    ],
)
