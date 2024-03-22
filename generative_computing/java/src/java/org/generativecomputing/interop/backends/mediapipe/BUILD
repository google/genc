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
        "//java/com/google/common/annotations",
        "@com_google_protobuf//java/lite",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_guava_guava",
        "@maven//:com_google_mediapipe_tasks_genai",
    ],
)
