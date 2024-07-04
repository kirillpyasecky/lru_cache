# Light weitght LRU Cache lib
## Usage
In WORKSPACE file:
```
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "lru_cache",
    commit = "f5c43d083695e77eb5c0607b76412ebd7db22302", # choose commit you needed
    remote = "https://github.com/kirillpyasecky/lru_cache.git",
)
```
In BUILD file:
```
...
deps = [
  "@lru_cache//:lru_cache",
],
...
```
