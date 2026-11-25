---
title: Platform support
---

| Platform | CI build and tests | Through libgiac_julia_jll |
|---|---|---|
| Linux | Built and tested | Supported |
| macOS | Built and tested | Supported |
| Windows | Built; tests allowed to fail | Works, with one known issue |

## Windows

Windows has a known ABI issue. It affects Julia users, not only CI.

- **In CI,** Windows builds the wrapper under MSYS2 (currently GCC 15.2) and
  links it against GIAC_jll (built by BinaryBuilder with GCC 8). The
  mismatch with the artifact's runtime DLLs makes the test step fail, so it
  runs with `continue-on-error`.
- **For Julia users,** the wrapper comes from libgiac_julia_jll (BinaryBuilder,
  GCC 10) and Giac from GIAC_jll (BinaryBuilder, GCC 8). Although both come
  from BinaryBuilder, a difference in the bitfield layout of Giac's `gen`
  struct crosses the boundary: MPFR reals come back tagged `_DOUBLE_` instead
  of `_REAL_`. It was found while reviewing
  [Giac.jl#22](https://github.com/JuliaGiac/Giac.jl/pull/22); Giac.jl works
  around it with a heuristic on the length of the printed number. A real fix
  probably means rebuilding GIAC_jll with the same `preferred_gcc_version` as
  libgiac_julia_jll.
