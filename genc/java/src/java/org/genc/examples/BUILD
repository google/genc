package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_binary(
    name = "genc_demo",
    srcs = ["GencDemo.java"],
    jvm_flags = [
        "-Djava.library.path=" +
        "genc/java/src/native/org/genc/authoring:" +
        "genc/java/src/native/org/genc/examples/executors:" +
        "genc/java/src/native/org/genc/runtime",
    ],
    main_class = "org.genc.examples.GencDemo",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto",
    ],
)

java_binary(
    name = "open_ai_demo",
    srcs = [
        "OpenAiDemo.java",
    ],
    jvm_flags = [
        "-Djava.library.path=" +
        "genc/java/src/native/org/genc/authoring:" +
        "genc/java/src/native/org/genc/examples/executors:" +
        "genc/java/src/native/org/genc/runtime",
    ],
    main_class = "org.genc.examples.OpenAiDemo",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:com_google_guava_guava",
    ],
)

java_binary(
    name = "gemini_on_ai_studio",
    srcs = [
        "GeminiOnAiStudioDemo.java",
    ],
    jvm_flags = [
        "-Djava.library.path=" +
        "genc/java/src/native/org/genc/authoring:" +
        "genc/java/src/native/org/genc/examples/executors:" +
        "genc/java/src/native/org/genc/runtime",
    ],
    # Enable/disable following jvm flags to view logs at FINE level
    main_class = "org.genc.examples.GeminiOnAiStudioDemo",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:com_google_guava_guava",
    ],
)

java_binary(
    name = "gemini_on_vertex_ai",
    srcs = [
        "GeminiOnVertexAiDemo.java",
    ],
    jvm_flags = [
        "-Djava.library.path=" +
        "genc/java/src/native/org/genc/authoring:" +
        "genc/java/src/native/org/genc/examples/executors:" +
        "genc/java/src/native/org/genc/runtime",
    ],
    main_class = "org.genc.examples.GeminiOnVertexAiDemo",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:com_google_guava_guava",
    ],
)

java_binary(
    name = "wolfram_alpha_demo",
    srcs = [
        "WolframAlphaDemo.java",
    ],
    jvm_flags = [
        "-Djava.library.path=" +
        "genc/java/src/native/org/genc/authoring:" +
        "genc/java/src/native/org/genc/examples/executors:" +
        "genc/java/src/native/org/genc/runtime",
    ],
    main_class = "org.genc.examples.WolframAlphaDemo",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:com_google_guava_guava",
    ],
)

java_binary(
    name = "local_model_with_llamacpp",
    srcs = [
        "LocalModelWithLlamacpp.java",
    ],
    jvm_flags = [
        "-Djava.library.path=" +
        "genc/java/src/native/org/genc/authoring:" +
        "genc/java/src/native/org/genc/examples/executors:" +
        "genc/java/src/native/org/genc/runtime",
    ],
    main_class = "org.genc.examples.LocalModelWithLlamacpp",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto",
        "@maven//:com_google_code_findbugs_jsr305",
        "@maven//:com_google_guava_guava",
    ],
)
