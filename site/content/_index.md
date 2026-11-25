---
title: libgiac-julia-wrapper
---

libgiac-julia-wrapper is a C++ library that exposes
[Giac](https://www-fourier.univ-grenoble-alpes.fr/~parisse/giac.html), the
computer algebra system by Bernard Parisse and Renée De Graeve, to Julia
through [CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl).

**To use Giac from Julia, you do not need this repository.** Install
[Giac.jl](https://juliagiac.github.io/Giac.jl/); it loads a pre-built copy of
this wrapper:

```julia
using Pkg; Pkg.add("Giac")
```

## How it is distributed

[BinaryBuilder](https://binarybuilder.org/) compiles the wrapper for every
Julia platform against the libgiac binaries of
[GIAC_jll](https://github.com/JuliaBinaryWrappers/GIAC_jll.jl), and publishes
the result as
[libgiac_julia_jll](https://github.com/JuliaBinaryWrappers/libgiac_julia_jll.jl)
([recipe](https://github.com/JuliaPackaging/Yggdrasil/tree/master/L/libgiac_julia)).
Giac.jl depends on it.

## What it provides

- **Evaluation**: `giac_eval(expr)` in a thread-local default context, and
  `giac_eval(expr, ctx)` in a `GiacContext` that keeps its own variable
  bindings and settings.
- **Function dispatch**: direct wrappers for about 25 common operations, and
  `apply_func0/1/2/3/N` to call any Giac function by name without going
  through a string.
- **`Gen`**, an opaque `giac::gen`: constructors, typed accessors (integers,
  big integers, rationals, complex numbers, vectors, symbolic trees, maps),
  predicates and arithmetic operators.
- **Help and introspection**: Giac's command database and function tables.

The [README](https://github.com/JuliaGiac/libgiac-julia-wrapper#readme) lists
the full API and the known platform issues.

## Building it yourself

Only needed to work on the wrapper itself. It builds with
[Meson](https://mesonbuild.com/) against CxxWrap's `libcxxwrap-julia` and
libgiac:

```sh
just setup   # meson setup builddir
just build   # meson compile -C builddir
just test    # meson test -C builddir
```

Issues go to the
[issue tracker](https://github.com/JuliaGiac/libgiac-julia-wrapper/issues).

## Part of JuliaGiac

This wrapper is one layer of [JuliaGiac](https://juliagiac.github.io/): it
sits between [Giac with its Meson build](https://juliagiac.github.io/giac/)
and [Giac.jl](https://juliagiac.github.io/Giac.jl/).
