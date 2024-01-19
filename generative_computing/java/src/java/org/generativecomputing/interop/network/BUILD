package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "cronet_engine_provider",
    srcs = ["CronetEngineProvider.java"],
    deps = [
        "@maven//:org_checkerframework_checker_qual",
        "@maven//:org_chromium_net_cronet_api",
        "@maven//:org_chromium_net_cronet_embedded",
    ],
)

android_library(
    name = "http_client",
    srcs = [
        "HttpClient.java",
        "HttpClientImpl.java",
        "HttpException.java",
    ],
    deps = [
        "//generative_computing/java/src/java/org/generativecomputing/interop/network/api/proto:http_request_response_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_flogger_flogger_system_backend",
        "@maven//:com_google_guava_guava",
        "@maven//:org_chromium_net_cronet_api",
        "@maven//:org_chromium_net_cronet_embedded",
    ],
)
