/**
 * @file test_log_capture.cpp
 * @brief Tests for capturing GIAC's informational log stream (logptr)
 *
 * GIAC writes informational messages (e.g. "// ∫ ~= 0.272198261288" when a
 * definite integral has no closed form) to the per-context `logptr` stream,
 * which defaults to stderr. The capture API swaps that stream for an
 * in-memory buffer so callers can retrieve the messages separately from the
 * returned value.
 */

#include "giac_impl.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>

using namespace giac_julia;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    try { test_##name(); std::cout << "PASSED" << std::endl; } \
    catch (const std::exception& e) { std::cout << "FAILED: " << e.what() << std::endl; return 1; } \
} while(0)

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) throw std::runtime_error("Assertion failed: " #cond); \
} while(0)

static bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

// Serret's integral: GIAC finds no antiderivative it can evaluate at the
// bounds, returns the integral unevaluated and logs a numeric estimate.
static const char* SERRET = "integrate(ln(1+x)/(1+x^2),x,0,1)";
static const char* SERRET_HINT = "0.272198261288";

TEST(empty_capture_returns_empty_string) {
    log_capture_begin();
    std::string log = log_capture_end();
    ASSERT_TRUE(log.empty());
}

TEST(depth_tracks_begin_and_end) {
    ASSERT_TRUE(log_capture_depth() == 0);
    log_capture_begin();
    ASSERT_TRUE(log_capture_depth() == 1);
    log_capture_begin();
    ASSERT_TRUE(log_capture_depth() == 2);
    log_capture_end();
    ASSERT_TRUE(log_capture_depth() == 1);
    log_capture_end();
    ASSERT_TRUE(log_capture_depth() == 0);
}

TEST(end_without_begin_throws) {
    bool threw = false;
    try {
        log_capture_end();
    } catch (const std::exception&) {
        threw = true;
    }
    ASSERT_TRUE(threw);
    ASSERT_TRUE(log_capture_depth() == 0);
}

TEST(captures_message_from_giac_eval) {
    log_capture_begin();
    Gen value = giac_eval(SERRET);
    std::string log = log_capture_end();
    // The value is untouched: still the unevaluated integral.
    ASSERT_TRUE(contains(value.to_string(), "integrate"));
    ASSERT_TRUE(contains(log, "~="));
    ASSERT_TRUE(contains(log, SERRET_HINT));
}

// The fast dispatch paths (apply_func1 / apply_funcN) bypass giac_eval and
// are what Giac.jl uses for `integrate(...)`; they must be covered too.
TEST(captures_message_from_apply_funcN) {
    std::vector<Gen> args = {
        giac_eval("ln(1+x)/(1+x^2)"), make_identifier("x"),
        Gen(int64_t(0)), Gen(int64_t(1)),
    };
    log_capture_begin();
    Gen value = apply_funcN("integrate", args);
    std::string log = log_capture_end();
    ASSERT_TRUE(contains(value.to_string(), "integrate"));
    ASSERT_TRUE(contains(log, SERRET_HINT));
}

TEST(captures_message_from_apply_func1) {
    std::vector<Gen> seq = {
        giac_eval("ln(1+x)/(1+x^2)"), make_identifier("x"),
        Gen(int64_t(0)), Gen(int64_t(1)),
    };
    Gen args = make_vect(seq, 1);  // _SEQ__VECT
    log_capture_begin();
    Gen value = apply_func1("integrate", args);
    std::string log = log_capture_end();
    ASSERT_TRUE(contains(log, SERRET_HINT));
}

TEST(stream_is_restored_after_end) {
    log_capture_begin();
    giac_eval(SERRET);
    log_capture_end();
    // A later capture starts empty: nothing leaks from the previous one.
    log_capture_begin();
    std::string log = log_capture_end();
    ASSERT_TRUE(log.empty());
}

TEST(nested_capture_propagates_to_outer) {
    log_capture_begin();                 // outer
    log_capture_begin();                 // inner
    giac_eval(SERRET);
    std::string inner = log_capture_end();
    std::string outer = log_capture_end();
    ASSERT_TRUE(contains(inner, SERRET_HINT));
    ASSERT_TRUE(contains(outer, SERRET_HINT));
}

TEST(capture_survives_evaluation_error) {
    log_capture_begin();
    try {
        giac_eval("1/");  // parse error
    } catch (const std::exception&) {
    }
    log_capture_end();
    ASSERT_TRUE(log_capture_depth() == 0);
}

TEST(context_capture_is_independent) {
    GiacContext ctx;
    ASSERT_TRUE(ctx.log_capture_depth() == 0);
    ctx.log_capture_begin();
    Gen value = giac_eval(SERRET, ctx);
    std::string log = ctx.log_capture_end();
    ASSERT_TRUE(contains(value.to_string(), "integrate"));
    ASSERT_TRUE(contains(log, SERRET_HINT));
    // The default (thread-local) context was not capturing.
    ASSERT_TRUE(log_capture_depth() == 0);
}

TEST(context_end_without_begin_throws) {
    GiacContext ctx;
    bool threw = false;
    try {
        ctx.log_capture_end();
    } catch (const std::exception&) {
        threw = true;
    }
    ASSERT_TRUE(threw);
}

// A capture opened on one thread receives messages from evaluations run on
// another, since both use the one default context.
TEST(capture_spans_threads) {
    log_capture_begin();
    std::thread t([]() { giac_eval(SERRET); });
    t.join();
    std::string log = log_capture_end();
    ASSERT_TRUE(contains(log, SERRET_HINT));
    ASSERT_TRUE(log_capture_depth() == 0);
}

int main() {
    std::cout << "=== GIAC Wrapper Log Capture Tests ===" << std::endl;

    RUN_TEST(empty_capture_returns_empty_string);
    RUN_TEST(depth_tracks_begin_and_end);
    RUN_TEST(end_without_begin_throws);
    RUN_TEST(captures_message_from_giac_eval);
    RUN_TEST(captures_message_from_apply_funcN);
    RUN_TEST(captures_message_from_apply_func1);
    RUN_TEST(stream_is_restored_after_end);
    RUN_TEST(nested_capture_propagates_to_outer);
    RUN_TEST(capture_survives_evaluation_error);
    RUN_TEST(context_capture_is_independent);
    RUN_TEST(context_end_without_begin_throws);
    RUN_TEST(capture_spans_threads);

    std::cout << "=== All tests passed ===" << std::endl;
    return 0;
}
