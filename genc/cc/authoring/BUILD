load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "constructor",
    srcs = ["constructor.cc"],
    hdrs = ["constructor.h"],
    deps = [
        "//genc/cc/base:computation",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "smart_chain",
    srcs = ["smart_chain.cc"],
    hdrs = ["smart_chain.h"],
    deps = [
        ":constructor",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "tracing_context",
    srcs = ["tracing_context.cc"],
    hdrs = ["tracing_context.h"],
    deps = [
        ":constructor",
        "//genc/cc/base:context",
        "//genc/cc/modules/retrieval:bi_map",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "constructor_test",
    srcs = ["constructor_test.cc"],
    deps = [
        ":constructor",
        "//genc/cc/base:computation",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "smart_chain_test",
    srcs = ["smart_chain_test.cc"],
    deps = [
        ":constructor",
        ":smart_chain",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

pybind_extension(
    name = "constructor_bindings",
    srcs = ["constructor_bindings.cc"],
    tags = ["generated_py_module=genc.authoring"],
    deps = [
        ":constructor",
        "//genc/proto/v0:computation_cc_proto",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
        "@pybind11_abseil//pybind11_abseil:status_casters",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)
