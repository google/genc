workspace(name = "org_genc")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "1.5.0",
)

git_repository(
    name = "rules_license",
    remote = "https://github.com/bazelbuild/rules_license.git",
    tag = "0.0.4",
)

git_repository(
    name = "rules_cc",
    remote = "https://github.com/bazelbuild/rules_cc.git",
    tag = "0.0.9",
)

git_repository(
    name = "rules_foreign_cc",
    remote = "https://github.com/bazelbuild/rules_foreign_cc.git",
    tag = "0.10.1",
)

git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    tag = "0.31.0",
)

git_repository(
    name = "rules_rust",
    remote = "https://github.com/bazelbuild/rules_rust.git",
    tag = "0.42.1",
)

git_repository(
    name = "rules_pkg",
    remote = "https://github.com/bazelbuild/rules_pkg.git",
    tag = "0.10.1",
)

git_repository(
    name = "platforms",
    remote = "https://github.com/bazelbuild/platforms.git",
    tag = "0.0.9",
)

git_repository(
    name = "build_bazel_rules_swift",
    remote = "https://github.com/bazelbuild/rules_swift.git",
    tag = "1.18.0",
)

git_repository(
    name = "com_google_absl",
    tag = "20240116.2",
    remote = "https://github.com/abseil/abseil-cpp.git",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    tag = "2024-04-01",
    repo_mapping = {"@abseil-cpp": "@com_google_absl"},
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    tag = "v26.1",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_github_grpc_grpc",
    remote = "https://github.com/grpc/grpc.git",
    tag = "v1.62.1",
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    tag = "release-1.11.0",
)

http_archive(
    name = "pybind11_bazel",
    strip_prefix = "pybind11_bazel-203508e14aab7309892a1c5f7dd05debda22d9a5",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/203508e14aab7309892a1c5f7dd05debda22d9a5.zip"],
    sha256 = "75922da3a1bdb417d820398eb03d4e9bd067c4905a4246d35a44c01d62154d91",
)

http_archive(
  name = "curl",
  urls = ["https://curl.se/download/curl-8.4.0.tar.gz"],
  sha256 = "816e41809c043ff285e8c0f06a75a1fa250211bbfb2dc0a037eeef39f1a9e427",
  strip_prefix = "curl-8.4.0",
  build_file = "//bazel:curl.BUILD",
)

new_git_repository(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    remote = "https://github.com/pybind/pybind11.git",
    tag = "v2.9.2",
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(name = "local_config_python")

git_repository(
    name = "pybind11_abseil",
    commit = "38111ef06d426f75bb335a3b58aa0342f6ce0ce3",
    remote = "https://github.com/pybind/pybind11_abseil.git",
)

load("@com_google_protobuf//bazel:system_python.bzl", "system_python")

system_python(
    name = "system_python",
    minimum_python_version = "3.7",
)

git_repository(
    name = "pybind11_protobuf",
    branch = "main",
    remote = "https://github.com/pybind/pybind11_protobuf.git",
)

http_archive(
    name = "rules_jvm_external",
    sha256 = "cd1a77b7b02e8e008439ca76fd34f5b07aecb8c752961f9640dea15e9e5ba1ca",
    strip_prefix = "rules_jvm_external-4.2",
    url = "https://github.com/bazelbuild/rules_jvm_external/archive/4.2.zip",
)

load("@rules_jvm_external//:repositories.bzl", "rules_jvm_external_deps")

rules_jvm_external_deps()

load("@rules_jvm_external//:setup.bzl", "rules_jvm_external_setup")

rules_jvm_external_setup()

load("@rules_jvm_external//:defs.bzl", "maven_install")

maven_install(
    artifacts = [
        # Implementation dependencies
        "androidx.activity:activity:1.2.2",
        "androidx.annotation:annotation:1.5.0",
        "androidx.appcompat:appcompat:aar:1.1.0-rc01",
        "androidx.concurrent:concurrent-futures:1.0.0-alpha03",
        "androidx.core:core:1.6.0",
        "androidx.fragment:fragment:1.3.4",
        "androidx.legacy:legacy-support-v4:aar:1.0.0",
        "androidx.lifecycle:lifecycle-common:2.3.1",
        "androidx.test:core:1.5.0",
        "androidx.test:runner:1.4.0",
        "androidx.test.ext:junit:1.1.4",
        "androidx.test:monitor:1.6.0",
        "androidx.test.espresso:espresso-core:3.5.0",
        "androidx.test.espresso:espresso-intents:3.5.0",
        "com.android.support:appcompat-v7:28.0.0",
        "com.google.code.findbugs:jsr305:3.0.2",
        "com.google.code.gson:gson:2.10.1",
        "com.google.guava:guava:31.1-android",
        "com.google.guava:listenablefuture:1.0",
        "com.google.flogger:flogger-system-backend:0.6",
        "com.google.flogger:flogger:0.6",
        "com.google.mediapipe:tasks-genai:0.10.11",
        "com.google.truth:truth:1.1.3",
        "javax.annotation:javax.annotation-api:1.3.2",
        "junit:junit:4.13.2",
        "org.mockito:mockito-core:2.28.1",
        "org.robolectric:robolectric:4.9.2",
        "org.robolectric:shadows-framework:4.9.2",
        # Cronet
        "org.chromium.net:cronet-api:98.4758.101",
        "org.chromium.net:cronet-embedded:98.4758.101",
    ],
    fetch_sources = True,
    generate_compat_repositories = True,
    repositories = [
        "https://maven.google.com",
        "https://repo1.maven.org/maven2",
    ],
)

http_archive(
    name = "build_bazel_rules_android",
    sha256 = "cd06d15dd8bb59926e4d65f9003bfc20f9da4b2519985c27e190cddc8b7a7806",
    strip_prefix = "rules_android-0.1.1",
    urls = ["https://github.com/bazelbuild/rules_android/archive/v0.1.1.zip"],
)

# Edge TPU
http_archive(
    name = "libedgetpu",
    sha256 = "14d5527a943a25bc648c28a9961f954f70ba4d79c0a9ca5ae226e1831d72fe80",
    strip_prefix = "libedgetpu-3164995622300286ef2bb14d7fdc2792dae045b7",
    urls = [
        "https://github.com/google-coral/libedgetpu/archive/3164995622300286ef2bb14d7fdc2792dae045b7.tar.gz",
    ],
)

load("@libedgetpu//:workspace.bzl", "libedgetpu_dependencies")

libedgetpu_dependencies()

load("@coral_crosstool//:configure.bzl", "cc_crosstool")

cc_crosstool(name = "crosstool")

http_archive(
    name = "com_google_protobuf_javalite",
    sha256 = "87407cd28e7a9c95d9f61a098a53cf031109d451a7763e7dd1253abf8b4df422",
    strip_prefix = "protobuf-3.19.1",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v3.19.1.tar.gz"],
)

# gflags needed by glog
http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "19713a36c9f32b33df59d1c79b4958434cb005b5b47dc5400a7a4b078111d9b5",
    strip_prefix = "gflags-2.2.2",
    url = "https://github.com/gflags/gflags/archive/v2.2.2.zip",
)

http_archive(
    name = "com_github_glog_glog",
    sha256 = "58c9b3b6aaa4dd8b836c0fd8f65d0f941441fb95e27212c5eeb9979cfd3592ab",
    strip_prefix = "glog-0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
    urls = [
        "https://github.com/google/glog/archive/0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6.zip",
    ],
)

http_archive(
    name = "robolectric",
    sha256 = "7e007fcfdca7b7228cb4de72707e8b317026ea95000f963e91d5ae365be52d0d",
    urls = ["https://github.com/robolectric/robolectric-bazel/archive/4.9.2.tar.gz"],
    strip_prefix = "robolectric-bazel-4.9.2",
)

load("@robolectric//bazel:robolectric.bzl", "robolectric_repositories")

robolectric_repositories()

http_archive(
  name = "llama_cpp",
  sha256 = "edd7ba04ffa779325bbdcd4c017f3e528d2de84fdd7b687749923342ea0db67a",
  strip_prefix = "llama.cpp-b2223",
  urls = ["https://github.com/ggerganov/llama.cpp/archive/refs/tags/b2223.tar.gz"],
  patches = ["//bazel:llama_cpp.PATCH"],
  patch_args = ["-p1"],
)

http_archive(
    name = "fmeum_rules_jni",
    sha256 = "9a387a066f683a8aac4d165917dc7fe15ec2a20931894a97e153a9caab6123ca",
    strip_prefix = "rules_jni-0.4.0",
    url = "https://github.com/fmeum/rules_jni/archive/refs/tags/v0.4.0.tar.gz",
)

load("@fmeum_rules_jni//jni:repositories.bzl", "rules_jni_dependencies")

rules_jni_dependencies()

http_archive(
    name = "com_google_googleapis",
    sha256 = "38701e513aff81c89f0f727e925bf04ac4883913d03a60cdebb2c2a5f10beb40",
    strip_prefix = "googleapis-86fa44cc5ee2136e87c312f153113d4dd8e9c4de",
    urls = [
        "https://github.com/googleapis/googleapis/archive/86fa44cc5ee2136e87c312f153113d4dd8e9c4de.tar.gz",
    ],
)

load("@com_google_googleapis//:repository_rules.bzl", "switched_rules_by_language")

switched_rules_by_language(
    name = "com_google_googleapis_imports",
    cc = True,
    grpc = True,
    python = True,
)

git_repository(
    name = "pantor_inja",
    remote = "https://github.com/pantor/inja.git",
    tag = "v3.4.0",
    build_file = "//bazel:inja.BUILD",
)

git_repository(
    name = "nlohmann_json",
    remote = "https://github.com/nlohmann/json.git",
    tag = "v3.11.3",
)

git_repository(
    name = "io_grpc_grpc_java",
    remote = "https://github.com/grpc/grpc-java.git",
    tag = "v1.63.0",
)

git_repository(
    name = "oak",
    remote = "https://github.com/project-oak/oak.git",
    branch = "main",
)

http_archive(
  name = "emsdk",
  strip_prefix = "emsdk-3.1.15/bazel",
  url = "https://github.com/emscripten-core/emsdk/archive/refs/tags/3.1.15.zip",
  sha256 = "bc06feb66e376f0184929e603d7c02dcd07646ed6f89281bf1478bb8947fbb0f",
)

load("@emsdk//:deps.bzl", emsdk_deps = "deps")

emsdk_deps()

load("@emsdk//:emscripten_deps.bzl", emsdk_emscripten_deps = "emscripten_deps")

emsdk_emscripten_deps()

git_repository(
  name = "tink_cc",
  remote = "https://github.com/tink-crypto/tink-cc.git",
  branch = "main",
)

load("@tink_cc//:tink_cc_deps.bzl", "tink_cc_deps")

tink_cc_deps()

load("@tink_cc//:tink_cc_deps_init.bzl", "tink_cc_deps_init")

tink_cc_deps_init()

load("@rules_rust//rust:repositories.bzl", "rules_rust_dependencies", "rust_register_toolchains")

rules_rust_dependencies()

rust_register_toolchains(
    edition = "2021",
    versions = [
        "1.76.0",
        "nightly/2024-02-01",
    ],
)
