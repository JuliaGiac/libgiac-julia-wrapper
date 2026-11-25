---
title: Building and testing
---

You only need this to work on the wrapper itself. Julia users get it
pre-built as [libgiac_julia_jll](https://github.com/JuliaBinaryWrappers/libgiac_julia_jll.jl)
through [Giac.jl](https://juliagiac.github.io/Giac.jl/).

## Requirements

- **Julia 1.10** or later (CI runs 1.10).
- **Meson 1.2+** and **Ninja**.
- A **C++17 compiler**: GCC 7+ or Clang 5+ (required by libcxxwrap-julia).
- **CMake**, which Meson uses to find JlCxx (libcxxwrap-julia).
- **GMP** and **MPFR** headers: `libgmp-dev libmpfr-dev` on Debian/Ubuntu,
  `gmp mpfr` on Homebrew, `mingw-w64-x86_64-{gmp,mpfr}` on MSYS2.
- **GSL** headers: the GIAC_jll 2.0.3 headers include `gsl/gsl_vector.h`
  (`libgsl-dev`, `gsl`, `mingw-w64-x86_64-gsl`).
- **gettext/libintl** headers on macOS (`brew install gettext`); glibc
  provides them on Linux.
- **Giac**, either:
  - [GIAC_jll](https://github.com/JuliaBinaryWrappers/GIAC_jll.jl), the Julia
    binary artifact, as CI does (recommended);
  - or a system install (`libgiac-dev` on Debian/Ubuntu, or built from
    source), with headers in `/usr/include/giac` by default.

The wrapper is tested against Giac 2.0.x. `wrapper_version()` reports
`meson.project_version()`, so it cannot drift from the build
([#2](https://github.com/JuliaGiac/libgiac-julia-wrapper/issues/2)).

## Building

### Against GIAC_jll (recommended, as in CI)

```sh
julia -e 'using Pkg; Pkg.add(["CxxWrap", "GIAC_jll"])'
CXXWRAP_PREFIX=$(julia -e 'using CxxWrap; print(CxxWrap.prefix_path())')
GIAC_ARTIFACT=$(julia -e 'using GIAC_jll; print(GIAC_jll.artifact_dir)')
meson setup builddir \
  --cmake-prefix-path="$CXXWRAP_PREFIX" \
  -Dgiac_include_dir="$GIAC_ARTIFACT/include/giac"
meson compile -C builddir
```

### Against a system Giac

```sh
julia -e 'using Pkg; Pkg.add("CxxWrap")'
meson setup builddir \
  --cmake-prefix-path="$(julia -e 'using CxxWrap; print(CxxWrap.prefix_path())')"
meson compile -C builddir
```

Override the header location with `-Dgiac_include_dir=...` if needed.

### With `just`

```sh
just setup    # meson setup builddir, against a system Giac
just build    # meson compile -C builddir
```

For GIAC_jll, use the explicit `meson setup` command above.

## Testing

```sh
meson test -C builddir                                 # C++ test suites (or: just test)
julia --project=tests/julia tests/julia/runtests.jl    # Julia tests of the bindings
```

The C++ suites (`tests/cpp/`) exercise the library directly. The Julia tests
(`tests/julia/`) load `builddir/src/libgiac_wrapper` through CxxWrap and
check the bindings; CI runs both. Most of the coverage from Julia lives in
[Giac.jl](https://github.com/JuliaGiac/Giac.jl)'s own test suite.

## Releases

A release bumps `version` in `meson.build`, is tagged, and is then built for
every platform by the
[Yggdrasil recipe](https://github.com/JuliaPackaging/Yggdrasil/tree/master/L/libgiac_julia),
which publishes libgiac_julia_jll.
