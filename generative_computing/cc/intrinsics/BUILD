load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "conditional",
    srcs = ["conditional.cc"],
    hdrs = ["conditional.h"],
    deps = [
        ":intrinsic_uris",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "fallback",
    srcs = ["fallback.cc"],
    hdrs = ["fallback.h"],
    deps = [
        ":intrinsic_uris",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "handler_sets",
    srcs = ["handler_sets.cc"],
    hdrs = ["handler_sets.h"],
    deps = [
        ":conditional",
        ":fallback",
        ":model_inference",
        ":prompt_template",
        ":regex_partial_match",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "@com_google_absl//absl/strings",
    ],
)

pybind_extension(
    name = "intrinsic_bindings",
    srcs = ["intrinsic_bindings.cc"],
    tags = ["generated_py_module=generative_computing.intrinsics"],
    deps = [
        ":intrinsic_uris",
        "@com_google_absl//absl/strings",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
        "@pybind11_abseil//pybind11_abseil:status_casters",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)

cc_library(
    name = "intrinsic_uris",
    srcs = [],
    hdrs = ["intrinsic_uris.h"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_library(
    name = "model_inference",
    srcs = ["model_inference.cc"],
    hdrs = ["model_inference.h"],
    deps = [
        ":intrinsic_uris",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "prompt_template",
    srcs = ["prompt_template.cc"],
    hdrs = ["prompt_template.h"],
    deps = [
        ":intrinsic_uris",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "regex_partial_match",
    srcs = ["regex_partial_match.cc"],
    hdrs = ["regex_partial_match.h"],
    deps = [
        ":intrinsic_uris",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)
