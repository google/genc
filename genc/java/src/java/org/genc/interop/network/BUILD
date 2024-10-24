load("@rules_java//java:defs.bzl", "java_library")

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
    name = "http_client_impl",
    srcs = [
        "HttpClient.java",
        "HttpClientImpl.java",
        "HttpException.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_flogger_flogger_system_backend",
        "@maven//:com_google_guava_guava",
        "@maven//:org_chromium_net_cronet_api",
        "@maven//:org_chromium_net_cronet_embedded",
    ],
)

android_library(
    name = "simple_synchronous_http_client_interface",
    srcs = [
        "SimpleSynchronousHttpClientInterface.java",
    ],
    deps = [
        ":http_client_impl",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto_lite",
        "@maven//:com_google_guava_guava",
    ],
)

java_library(
    name = "http_client",
    srcs = [
        "HttpClient.java",
        "HttpException.java",
    ],
    deps = [
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto",
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
