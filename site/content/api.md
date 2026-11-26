---
title: API
---

The wrapper registers its C++ API with [CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl).
[Giac.jl](https://juliagiac.github.io/Giac.jl/) builds its Julia interface on
it; you only call these names directly when working on the wrapper or on
Giac.jl itself.

## Evaluation

- `giac_eval(expr)` parses and evaluates a string in a thread-local default
  context, and returns a `Gen`.
- `giac_eval(expr, ctx)` does the same in a `GiacContext`. Each context keeps
  its own `:=` bindings and settings, so two contexts are isolated
  ([#3](https://github.com/JuliaGiac/libgiac-julia-wrapper/issues/3)).
- `Gen` methods evaluate an already-built expression: `gen_eval`, `simplify`,
  `expand`, `factor`.

## Function dispatch

- **Direct wrappers** for about 25 common operations, which skip the lookup
  of the function by name: `giac_sin/cos/tan/asin/acos/atan`,
  `giac_exp/ln/log10/sqrt`, `giac_abs/sign/floor/ceil`, `giac_re/im/conj`,
  `giac_normal/evalf`, `giac_diff/integrate/subst/solve/limit/series`,
  `giac_gcd/lcm/pow`.
- **Generic dispatch** by Giac function name: `apply_func0/1/2/3/N` call any
  Giac built-in or user-defined function with 0, 1, 2, 3 or N arguments,
  without going through a string.

## `Gen`, an opaque `giac::gen`

- **Construction:** `Gen(string)`, `Gen(Int32)`, `Gen(Float64)`,
  `make_identifier`, `make_complex`, `make_fraction`, `make_vect`,
  `make_zint_from_bytes`, `make_symbolic_unevaluated`.
- **Typed accessors:** `to_int64/int32/double`, `zint_to_bytes/sign/string`,
  `cplx_re/im`, `frac_num/den`, `vect_size/at`, `symb_sommet_name/feuille`,
  `idnt_name`, `strng_value`, `map_size/keys/values`, `type/subtype/type_name`.
- **Predicates:** `is_zero`, `is_one`, `is_integer`, `is_approx`;
  `is_numeric`, `is_vector`, `is_symbolic`, `is_identifier`, `is_fraction`,
  `is_complex`, `is_string`.
- **Operators:** `+ - * /` and unary `-`, also mixed with Julia `Int64` and
  `Float64`; `==` and `!=`.
- **Pointers**, for zero-copy exchange with Julia: `gen_to_heap_ptr`,
  `gen_from_heap_ptr`, `free_gen_ptr`, `gen_ptr_to_string`, `gen_ptr_type`.

From Julia, build small integers with `Gen(Int32(n))`: CxxWrap does not tell
the `int64_t` and `double` constructors apart, so `Gen(Int64(n))` gives a
floating-point `Gen`.

## Log capture

GIAC writes informational messages to its log stream (stderr by default),
for instance `// ∫ ~= 0.272198261288` when a definite integral is returned
unevaluated. The capture API redirects that stream to an in-memory buffer,
so the text can be retrieved separately from the result:

- `log_capture_begin()`, `log_capture_end()` and `log_capture_depth()` act on
  the default context; the same three names taking a `GiacContext` act on
  that context.
- The capture is installed on the context, so every evaluation path that uses
  it is covered: `giac_eval`, `apply_func0/1/2/3/N`, the direct wrappers and
  `Gen` methods.
- Captures nest; the text of an inner capture is also appended to the
  enclosing one. `log_capture_end()` without a matching begin throws.
- Messages GIAC writes straight to `std::cerr`/`std::cout`, bypassing its log
  stream, are not captured.

```julia
log_capture_begin()
value = giac_eval("integrate(ln(1+x)/(1+x^2),x,0,1)")
log = log_capture_end()      # "// ∫ ~= 0.272198261288\n"
to_string(value)             # "integrate(ln(1+x)/(1+x^2),x,0,1)"
```

Giac.jl wraps this as `evaluate_with_diagnostics`.

## Help and introspection

- `init_help(path_to_aide_cas)` loads Giac's command database up front, so
  Giac never searches the file system for it.
- `list_commands()`, `help_count()`, `list_builtin_functions()`,
  `builtin_function_count()`, `list_all_functions()` list the command tables.

## Contexts and configuration

- `set_xcasroot`, `get_xcasroot`: where Giac finds its data files.
- Per `GiacContext`: `set_variable/get_variable`, `set_precision/get_precision`
  and `set_complex_mode/is_complex_mode`. Precision and complex mode use
  Giac's own per-context settings (`decimal_digits`, `complex_mode`), so they
  change later results: `factor(x^2+1)` gives `(x+i)*(x-i)` only in complex
  mode, and `evalf(pi)` at precision 50 gives about 50 significant digits.
- `set_timeout/get_timeout` store a per-context timeout (30 s by default),
  but it does **not** interrupt evaluations yet.
- The C++ class also has `set_warning_handler` and `clear_warning_handler`.
  They are not exposed to Julia: CxxWrap does not pass the `std::function`
  argument.

## Calling the wrapper from Julia directly

```julia
# From the repository root, after `meson compile -C builddir`.
using CxxWrap
const libgiac = joinpath(pwd(), "builddir", "src", "libgiac_wrapper")
@wrapmodule(() -> libgiac)
@initcxx

r = giac_eval("sin(x)+1")
to_string(r)                       # "sin(x)+1"

ctx1 = GiacContext()
ctx2 = GiacContext()
giac_eval("a := 5", ctx1)
to_string(giac_eval("a", ctx1))    # "5"
to_string(giac_eval("a", ctx2))    # "a": ctx2 never saw the binding
```
