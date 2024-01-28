workspace(name = "org_generative_computing")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "1.3.0",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    tag = "v3.21.9",
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
    name = "rules_license",
    remote = "https://github.com/bazelbuild/rules_license.git",
    tag = "0.0.4",
)

git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    tag = "0.5.0",
)

http_archive(
    name = "rules_pkg",
    sha256 = "8a298e832762eda1830597d64fe7db58178aa84cd5926d76d5b744d6558941c2",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.7.0/rules_pkg-0.7.0.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.7.0/rules_pkg-0.7.0.tar.gz",
    ],
)

git_repository(
    name = "com_github_grpc_grpc",
    remote = "https://github.com/grpc/grpc.git",
    tag = "v1.43.0",
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    tag = "release-1.11.0",
)

http_archive(
    name = "com_google_absl",
    sha256 = "3ea49a7d97421b88a8c48a0de16c16048e17725c7ec0f1d3ea2683a2a75adc21",
    strip_prefix = "abseil-cpp-20230125.0",
    url = "https://github.com/abseil/abseil-cpp/archive/refs/tags/20230125.0.tar.gz",
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

git_repository(
    name = "pybind11_protobuf",
    commit = "80f3440cd8fee124e077e2e47a8a17b78b451363",
    remote = "https://github.com/pybind/pybind11_protobuf.git",
)

http_archive(
    name = "com_github_google_re2",
    sha256 = "8b4a8175da7205df2ad02e405a950a02eaa3e3e0840947cd598e92dca453199b",
    strip_prefix = "re2-2023-06-01",
    urls = [
        "https://github.com/google/re2/archive/refs/tags/2023-06-01.tar.gz",
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/google/re2/archive/refs/tags/2023-06-01.tar.gz",
    ],
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
        "com.google.code.gson:gson:2.8.9",
        "com.google.guava:guava:31.1-android",
        "com.google.guava:listenablefuture:1.0",
        "com.google.flogger:flogger-system-backend:0.6",
        "com.google.flogger:flogger:0.6",
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
    name = "rules_cc",
    sha256 = "691a29db9c336349e48e04c5c2f4873f2890af5cbfa6e51f4de87fefe6169294",
    strip_prefix = "rules_cc-2f8c04c04462ab83c545ab14c0da68c3b4c96191",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/2f8c04c04462ab83c545ab14c0da68c3b4c96191.zip"],
)

http_archive(
    name = "rules_foreign_cc",
    sha256 = "c2cdcf55ffaf49366725639e45dedd449b8c3fe22b54e31625eb80ce3a240f1e",
    strip_prefix = "rules_foreign_cc-0.1.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.1.0.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

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

load("@rules_python//python:pip.bzl","pip_parse")
pip_parse(
    name = "notebooks",
    requirements_lock = "//generative_computing/python/examples/colabs:requirements.txt",
)
load("@notebooks//:requirements.bzl", "install_deps")
install_deps()


