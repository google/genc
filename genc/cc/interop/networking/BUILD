package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "http_client_interface",
    srcs = [],
    hdrs = ["http_client_interface.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "cronet_based_android_http_client",
    srcs = ["cronet_based_android_http_client.cc"],
    hdrs = ["cronet_based_android_http_client.h"],
    deps = [
        ":http_client_interface",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "curl_based_http_client",
    srcs = ["curl_based_http_client.cc"],
    hdrs = ["curl_based_http_client.h"],
    deps = [
        ":http_client_interface",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@curl",
    ],
)
