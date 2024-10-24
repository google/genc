load("@rules_java//java:defs.bzl", "java_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_library(
    name = "default_executor",
    srcs = ["DefaultExecutor.java"],
    deps = [
        "//genc/java/src/java/org/genc/interop/backends/googleai:google_ai_client",
        "//genc/java/src/java/org/genc/interop/backends/openai:open_ai_client",
        "//genc/java/src/java/org/genc/interop/backends/wolframalpha:wolfram_alpha_client",
        "//genc/java/src/java/org/genc/interop/network:java_http_client",
        "//genc/java/src/native/org/genc/examples/executors:libdefault_executor_jni_lib",
    ],
)
