package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@oak//cc/client",
    ],
)

cc_library(
    name = "transport_wrapper",
    srcs = ["transport_wrapper.cc"],
    hdrs = ["transport_wrapper.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@oak//cc/transport",
    ],
)
