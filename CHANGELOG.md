# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
Changes before this file was introduced are recorded in the git history.

## [Unreleased]

## [0.6.0] - 2026-11-26

Distributed as libgiac_julia_jll 0.6.0, built against GIAC_jll 200.100.0
(giac 2.1.0, upstream's 5-bit `gen` type field, PARI 2.19.1). The `gen`
layout differs from GIAC_jll 2.0.x, so this release cannot be used with
GIAC_jll 2.0.x, nor 0.5.x with GIAC_jll 200.100.0.

### Changed

- **One default context for the whole process.** The default context used by
  `giac_eval(expr)`, `apply_func*`, the Tier 1 wrappers and `Gen` methods was
  `thread_local`: a call from another OS thread silently evaluated in a
  different, empty context, without the `:=` bindings, assumptions or
  settings made from the first thread. It is now one context shared by every
  thread, and so is its log capture stack. GIAC keeps no per-thread state of
  its own, but it cannot be entered from two threads at once: callers must
  serialize their calls, as Giac.jl does with its GIAC lock.

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

[Unreleased]: https://github.com/JuliaGiac/libgiac-julia-wrapper/compare/v0.6.0...HEAD
[0.6.0]: https://github.com/JuliaGiac/libgiac-julia-wrapper/compare/v0.4.0...v0.6.0
