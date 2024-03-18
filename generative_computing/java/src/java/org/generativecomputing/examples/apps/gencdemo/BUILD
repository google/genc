package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

package_group(
    name = "genc_demo_packages",
    packages = ["//generative_computing/java/src/java/org/generativecomputing/examples/apps/gencdemo/..."],
)

android_library(
    name = "lib",
    srcs = [
        "Computations.java",
        "DefaultAndroidExecutor.java",
        "GencDemo.java",
    ],
    custom_package = "org.generativecomputing.examples.apps.gencdemo",
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**/*"]),
    visibility = ["//visibility:public"],
    deps = [
        "//generative_computing/java/src/java/org/generativecomputing/authoring:constructor",
        "//generative_computing/java/src/java/org/generativecomputing/examples/apps/gencdemo/lib:jni",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/googleai:google_ai_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/mediapipe:llm_inference_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/openai:open_ai_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/wolframalpha:wolfram_alpha_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:cronet_engine_provider",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:http_client",
        "//generative_computing/java/src/java/org/generativecomputing/runtime:runner",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "//generative_computing/proto/v0:executor_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_annotation_annotation",
        "@maven//:androidx_appcompat_appcompat",
        "@maven//:androidx_legacy_legacy_support_v4",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_guava_guava",
        "@maven//:org_chromium_net_cronet_api",
        "@maven//:org_chromium_net_cronet_embedded",
    ],
)

android_binary(
    name = "app",
    manifest = "AndroidManifest.xml",
    multidex = "native",
    deps = [
        ":lib",
    ],
)
