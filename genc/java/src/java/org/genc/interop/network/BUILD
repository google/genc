package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_library(
    name = "http_client",
    srcs = [
        "HttpClient.java",
        "HttpException.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto",
        "@maven//:com_google_flogger_flogger_system_backend",
        "@maven//:com_google_guava_guava",
    ],
)

java_library(
    name = "java_http_client",
    srcs = [
        "JavaHttpClientImpl.java",
    ],
    deps = [
        ":http_client",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_flogger_flogger_system_backend",
        "@maven//:com_google_guava_guava",
    ],
)
