# libgiac-julia-wrapper

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/s-celles/libgiac-julia-wrapper)
[![CI](https://github.com/JuliaGiac/libgiac-julia-wrapper/actions/workflows/ci.yml/badge.svg)](https://github.com/JuliaGiac/libgiac-julia-wrapper/actions/workflows/ci.yml)
[![Website](https://img.shields.io/badge/website-juliagiac.github.io-4063D8)](https://juliagiac.github.io/libgiac-julia-wrapper/)

C++ library exposing the [Giac](https://www-fourier.univ-grenoble-alpes.fr/~parisse/giac.html)
computer algebra system (Bernard Parisse and Renée De Graeve) to Julia through
[CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl).

**To use Giac from Julia, install [Giac.jl](https://github.com/JuliaGiac/Giac.jl).**
It loads this wrapper pre-built, as
[libgiac_julia_jll](https://github.com/JuliaBinaryWrappers/libgiac_julia_jll.jl).
This repository is for working on the wrapper itself.

## Documentation

<https://juliagiac.github.io/libgiac-julia-wrapper/>

- [API](https://juliagiac.github.io/libgiac-julia-wrapper/api/): evaluation and contexts, function dispatch, `Gen`, help.
- [Building and testing](https://juliagiac.github.io/libgiac-julia-wrapper/building/): requirements, GIAC_jll or system Giac, tests, releases.
- [Platform support](https://juliagiac.github.io/libgiac-julia-wrapper/platforms/): the known Windows ABI issue.

## Quick start

```sh
julia -e 'using Pkg; Pkg.add(["CxxWrap", "GIAC_jll"])'
meson setup builddir \
  --cmake-prefix-path="$(julia -e 'using CxxWrap; print(CxxWrap.prefix_path())')" \
  -Dgiac_include_dir="$(julia -e 'using GIAC_jll; print(GIAC_jll.artifact_dir)')/include/giac"
meson compile -C builddir
meson test -C builddir
```

The web site's sources are in `site/` (Hugo): `just site-serve` previews it.

## License

GPL-3.0-or-later. See [LICENSE](LICENSE).
