package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "wolfram_alpha_client",
    srcs = [
        "WolframAlphaClient.java",
    ],
    deps = [
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:http_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network/api/proto:http_request_response_java_proto_lite",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_guava_guava",
    ],
)
