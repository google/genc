package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_local_test(
    name = "OpenAiClientTest",
    srcs = ["OpenAiClientTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/openai:open_ai_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:http_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network/api/proto:http_request_response_java_proto_lite",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_test_ext_junit",
        "@maven//:com_google_guava_guava",
        "@maven//:com_google_truth_truth",
        "@maven//:org_mockito_mockito_core",
        "@maven//:org_robolectric_annotations",
        "@maven//:org_robolectric_robolectric",
        "@maven//:org_robolectric_shadows_framework",
        "@robolectric//bazel:android-all",
    ],
)
