package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_local_test(
    name = "LlmInferenceClientTest",
    srcs = ["LlmInferenceClientTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "//genc/java/src/java/org/genc/interop/backends/mediapipe:llm_inference_client",
        "//genc/proto/v0:computation_java_proto_lite",
        "@maven//:androidx_test_ext_junit",
        "@maven//:com_google_mediapipe_tasks_genai",
        "@maven//:com_google_truth_truth",
        "@maven//:org_robolectric_annotations",
        "@maven//:org_robolectric_robolectric",
        "@maven//:org_robolectric_shadows_framework",
        "@robolectric//bazel:android-all",
    ],
)
