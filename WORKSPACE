workspace(name = "cache_lib")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "6dc2da7ab4cf5d7bfc7c949776b1b7c733f05e56edc4bcd9022bb249d2e2a996",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.39.1/rules_go-v0.39.1.zip",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.39.1/rules_go-v0.39.1.zip",
    ],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains(version = "1.20.3")

http_archive(
    name = "bazel_gazelle",
    sha256 = "727f3e4edd96ea20c29e8c2ca9e8d2af724d8c7778e7923a854b2c80952bc405",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-gazelle/releases/download/v0.30.0/bazel-gazelle-v0.30.0.tar.gz",
        "https://github.com/bazelbuild/bazel-gazelle/releases/download/v0.30.0/bazel-gazelle-v0.30.0.tar.gz",
    ],
)

load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")

gazelle_dependencies()

http_archive(
    name = "com_google_protobuf",
    sha256 = "3bd7828aa5af4b13b99c191e8b1e884ebfa9ad371b0ce264605d347f135d2568",
    strip_prefix = "protobuf-3.19.4",
    urls = [
        "https://github.com/protocolbuffers/protobuf/archive/v3.19.4.tar.gz",
    ],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

http_archive(
    name = "com_github_bazelbuild_buildtools",
    sha256 = "ae34c344514e08c23e90da0e2d6cb700fcd28e80c02e23e4d5715dddcb42f7b3",
    strip_prefix = "buildtools-4.2.2",
    urls = [
        "https://github.com/bazelbuild/buildtools/archive/refs/tags/4.2.2.tar.gz",
    ],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "66ffd9315665bfaafc96b52278f57c7e2dd09f5ede279ea6d39b2be471e7e3aa",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.4.2/bazel-skylib-1.4.2.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.4.2/bazel-skylib-1.4.2.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

http_archive(
    name = "catch2",
    sha256 = "122928b814b75717316c71af69bd2b43387643ba076a6ec16e7882bfb2dfacbb",
    strip_prefix = "Catch2-3.4.0",
    urls = ["https://github.com/catchorg/Catch2/archive/refs/tags/v3.4.0.tar.gz"],
)

http_archive(
    name = "hedron_compile_commands",
    sha256 = "ed5aea1dc87856aa2029cb6940a51511557c5cac3dbbcb05a4abd989862c36b4",
    strip_prefix = "bazel-compile-commands-extractor-e16062717d9b098c3c2ac95717d2b3e661c50608",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/e16062717d9b098c3c2ac95717d2b3e661c50608.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "bazel_clang_tidy",
    commit = "11541864afa832ff6721e479c44794e9c9497ae8",
    remote = "https://github.com/erenon/bazel_clang_tidy.git",
)

# load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_repository")

BAZEL_CLANG_FORMAT_COMMIT = "ad5f2572518053b36a2d999f7b824fb5a7819ab4"

BAZEL_CLANG_FORMAT_SHA = "c31f0ea477dd0da84fc2c694a533797a9d2bf060f8cca8b5ea30402a47a2ea0a"

http_archive(
    name = "bazel_clang_format",
    sha256 = BAZEL_CLANG_FORMAT_SHA,
    strip_prefix = "bazel_clang_format-{commit}".format(
        commit = BAZEL_CLANG_FORMAT_COMMIT,
    ),
    url = "https://github.com/oliverlee/bazel_clang_format/archive/{commit}.tar.gz".format(
        commit = BAZEL_CLANG_FORMAT_COMMIT,
    ),
)
