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

Evaluation in a default context or in isolated `GiacContext`s, direct calls
to any Giac function by name, an opaque `Gen` type with typed accessors and
operators, and Giac's help database. See the [API](api/).

## Building it yourself

Only needed to work on the wrapper itself: see
[Building and testing](building/), and [Platform support](platforms/) for
the known Windows issue. Issues go to the
[issue tracker](https://github.com/JuliaGiac/libgiac-julia-wrapper/issues).

## Part of JuliaGiac

This wrapper is one layer of [JuliaGiac](https://juliagiac.github.io/): it
sits between [Giac with its Meson build](https://juliagiac.github.io/giac/)
and [Giac.jl](https://juliagiac.github.io/Giac.jl/).
