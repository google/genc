load("@rules_rust//rust:defs.bzl", "rust_static_library")

package(
    default_visibility = ["//genc/c/interop/oak:__pkg__"],
)

licenses(["notice"])

rust_static_library(
    name = "oak_api",
    srcs = ["src/lib.rs"],
)
