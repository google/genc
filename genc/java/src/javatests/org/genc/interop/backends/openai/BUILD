load("@rules_java//java:defs.bzl", "java_test")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_test(
    name = "OpenAiClientTest",
    srcs = ["OpenAiClientTest.java"],
    deps = [
        "//genc/java/src/java/org/genc/interop/backends/openai:open_ai_client",
        "//genc/java/src/java/org/genc/interop/network:http_client",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto",
        "//genc/proto/v0:computation_java_proto",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_guava_guava",
        "@maven//:com_google_truth_truth",
        "@maven//:junit_junit",
        "@maven//:org_mockito_mockito_core",
    ],
)
