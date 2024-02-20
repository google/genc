package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_library(
    name = "llm_inference_client",
    srcs = [
        "LlmInferenceClient.java",
    ],
    deps = [
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_flogger_flogger",
    ],
)
