load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

proto_library(
    name = "http_request_response_proto",
    srcs = ["http_request_response.proto"],
)

cc_proto_library(
    name = "http_request_response_cc_proto",
    deps = [":http_request_response_proto"],
)

java_proto_library(
    name = "http_request_response_java_proto",
    deps = [":http_request_response_proto"],
)

java_lite_proto_library(
    name = "http_request_response_java_proto_lite",
    deps = [":http_request_response_proto"],
)
