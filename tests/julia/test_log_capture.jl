# test_log_capture.jl
# Tests for capturing GIAC's informational log stream (logptr)

include(joinpath(@__DIR__, "load_wrapper.jl"))

# Serret's integral: returned unevaluated, with a numeric estimate logged.
const SERRET = "integrate(ln(1+x)/(1+x^2),x,0,1)"
const SERRET_HINT = "0.272198261288"

@testset "GIAC Wrapper Log Capture Tests" begin
    @testset "empty capture" begin
        log_capture_begin()
        @test log_capture_end() == ""
        @test log_capture_depth() == 0
    end

    @testset "captures message, value untouched (giac_eval)" begin
        log_capture_begin()
        value = giac_eval(SERRET)
        log = log_capture_end()
        @test occursin("integrate", to_string(value))
        @test occursin("~=", log)
        @test occursin(SERRET_HINT, log)
    end

    @testset "captures message on the apply_funcN fast path" begin
        # Bounds built with giac_eval: from Julia, Gen(Int64(0)) currently
        # yields a double, which would turn this into a numeric integral.
        args = CxxWrap.StdVector([giac_eval("ln(1+x)/(1+x^2)"), make_identifier("x"),
                                  giac_eval("0"), giac_eval("1")])
        log_capture_begin()
        value = apply_funcN("integrate", args)
        log = log_capture_end()
        @test occursin("integrate", to_string(value))
        @test occursin(SERRET_HINT, log)
    end

    @testset "nesting" begin
        log_capture_begin()
        log_capture_begin()
        @test log_capture_depth() == 2
        giac_eval(SERRET)
        inner = log_capture_end()
        outer = log_capture_end()
        @test occursin(SERRET_HINT, inner)
        @test occursin(SERRET_HINT, outer)
        @test log_capture_depth() == 0
    end

    @testset "end without begin throws" begin
        @test_throws Exception log_capture_end()
        @test log_capture_depth() == 0
    end

    @testset "per-context capture" begin
        ctx = GiacContext()
        log_capture_begin(ctx)
        @test log_capture_depth(ctx) == 1
        @test log_capture_depth() == 0
        value = giac_eval(SERRET, ctx)
        log = log_capture_end(ctx)
        @test occursin("integrate", to_string(value))
        @test occursin(SERRET_HINT, log)
        @test_throws Exception log_capture_end(ctx)
    end
end
