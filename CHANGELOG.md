# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
Changes before this file was introduced are recorded in the git history.

## [Unreleased]

### Added

- **Log capture.** GIAC writes informational messages to the per-context
  `logptr` stream (stderr by default), for instance
  `// ∫ ~= 0.272198261288` when a definite integral is returned
  unevaluated. `log_capture_begin()`, `log_capture_end()` and
  `log_capture_depth()` redirect that stream to an in-memory buffer on the
  thread-local default context; the same three names taking a `GiacContext`
  act on that context. The capture is installed on the context, not around
  individual entry points, so `giac_eval`, the `apply_func0/1/2/3/N` fast
  paths, the Tier 1 wrappers and `Gen` methods are all covered. Captures
  nest, and the text of an inner capture is also appended to the enclosing
  one. `log_capture_end()` without a matching begin throws. Messages GIAC
  writes straight to `std::cerr`/`std::cout` are not captured.
- `tests/julia/load_wrapper.jl` honours `GIAC_WRAPPER_LIB`, so the Julia
  tests can run against a library built outside `builddir/`.
