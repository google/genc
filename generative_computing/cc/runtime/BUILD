load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "executor",
    srcs = ["executor.cc"],
    hdrs = ["executor.h"],
    deps = [
        ":status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "executor_test",
    timeout = "short",
    srcs = ["executor_test.cc"],
    deps = [
        ":executor",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)

pybind_extension(
    name = "executor_bindings",
    srcs = ["executor_bindings.cc"],
    tags = ["generated_py_module=generative_computing.runtime"],
    deps = [
        ":executor",
        ":executor_stacks",
        "//generative_computing/cc/runtime/intrinsics",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
        "@pybind11_abseil//pybind11_abseil:status_casters",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)

cc_library(
    name = "executor_stacks",
    srcs = ["executor_stacks.cc"],
    hdrs = ["executor_stacks.h"],
    deps = [
        ":executor",
        ":lambda_executor",
        ":model_executor",
        ":status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "executor_stacks_test",
    timeout = "short",
    srcs = ["executor_stacks_test.cc"],
    deps = [
        ":executor",
        ":executor_stacks",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "intrinsic_handler",
    srcs = ["intrinsic_handler.cc"],
    hdrs = ["intrinsic_handler.h"],
    deps = [
        ":status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "lambda_executor",
    srcs = ["lambda_executor.cc"],
    hdrs = ["lambda_executor.h"],
    deps = [
        ":executor",
        ":status_macros",
        "//generative_computing/cc/runtime/intrinsics",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "lambda_executor_test",
    timeout = "short",
    srcs = ["lambda_executor_test.cc"],
    deps = [
        ":executor",
        ":lambda_executor",
        ":model_executor",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "model_executor",
    srcs = ["model_executor.cc"],
    hdrs = ["model_executor.h"],
    deps = [
        ":executor",
        ":intrinsic_handler",
        ":status_macros",
        ":threading",
        "//generative_computing/cc/runtime/intrinsics",
        "//generative_computing/cc/runtime/intrinsics:prompt_template",
        "//generative_computing/cc/runtime/intrinsics:regex_partial_match",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "model_executor_test",
    timeout = "short",
    srcs = ["model_executor_test.cc"],
    deps = [
        ":executor",
        ":model_executor",
        "//generative_computing/cc/runtime/intrinsics",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "status_macros",
    hdrs = ["status_macros.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "threading",
    srcs = ["threading.cc"],
    hdrs = ["threading.h"],
    deps = [
        ":status_macros",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ],
)
