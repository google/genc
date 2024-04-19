package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "attestation",
    srcs = ["attestation.cc"],
    hdrs = ["attestation.h"],
    deps = [
        "//genc/cc/interop/oak:attestation_provider",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@curl",
        "@oak//proto/session:messages_cc_proto",
    ],
)
