/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests all the default operators in Fluxins along with it's
/// precedence and associativity.
///
/// This project is licensed under the terms of MIT License.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cmath>
#include <format>
#include <memory>
#include <string>

#include "doctest/doctest.h"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"
#include "fluxins/expression.hpp"

#define CHECK_EXPR(expected_expr, check_expr)                 \
    do                                                        \
    {                                                         \
        CAPTURE(expected_expr);                               \
        CAPTURE(check_expr);                                  \
        float expected = 0.0f;                                \
        try                                                   \
        {                                                     \
            expected = express(expected_expr, cfg);           \
        }                                                     \
        catch (const fluxins::code_error &e)                  \
        {                                                     \
            MESSAGE("Fluxins error: ", e.what());             \
            continue;                                         \
        }                                                     \
        if (std::isnan(expected) || std::isinf(expected))     \
            continue;                                         \
        CHECK(fluxins::express(check_expr, cfg) == expected); \
    }                                                         \
    while (0)

TEST_CASE("All operators functional evaluation")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Binary operators
    CHECK(fluxins::express("1 + 2", cfg) == 3.0f);
    CHECK(fluxins::express("5 - 3", cfg) == 2.0f);
    CHECK(fluxins::express("4 * 2", cfg) == 8.0f);
    CHECK(fluxins::express("8 / 2", cfg) == 4.0f);
    CHECK(fluxins::express("7 % 4", cfg) == 3.0f);
    CHECK(fluxins::express("-2 %% 5", cfg) == 3.0f);
    CHECK(fluxins::express("2 ** 3", cfg) == 8.0f);
    CHECK(fluxins::express("7 // 2", cfg) == 3.0f);
    CHECK(fluxins::express("2 == 2", cfg) == 1.0f);
    CHECK(fluxins::express("2 != 2", cfg) == 0.0f);
    CHECK(fluxins::express("2 < 3", cfg) == 1.0f);
    CHECK(fluxins::express("3 > 2", cfg) == 1.0f);
    CHECK(fluxins::express("2 <= 2", cfg) == 1.0f);
    CHECK(fluxins::express("2 >= 3", cfg) == 0.0f);
    CHECK(fluxins::express("1 && 0", cfg) == 0.0f);
    CHECK(fluxins::express("1 || 0", cfg) == 1.0f);
    CHECK(fluxins::express("3 & 1", cfg) == 1.0f);
    CHECK(fluxins::express("2 | 1", cfg) == 3.0f);
    CHECK(fluxins::express("2 ^ 3", cfg) == 1.0f);
    CHECK(fluxins::express("1 << 2", cfg) == 4.0f);
    CHECK(fluxins::express("4 >> 1", cfg) == 2.0f);
    CHECK(fluxins::express("3 !! 5", cfg) == 2.0f);
    CHECK(fluxins::express("0 ?? 5", cfg) == 5.0f);
    CHECK(fluxins::express("4 ?? 5", cfg) == 4.0f);
    CHECK(fluxins::express("3 <? 5", cfg) == 3.0f);
    CHECK(fluxins::express("3 >? 5", cfg) == 5.0f);

    // Prefix unary operators
    CHECK(fluxins::express("+5", cfg) == 5.0f);
    CHECK(fluxins::express("-5", cfg) == -5.0f);
    CHECK(fluxins::express("*5", cfg) == 5.0f);
    CHECK(fluxins::express("/5", cfg) == 0.2f);
    CHECK(fluxins::express("!0", cfg) == 1.0f);
    CHECK(fluxins::express("!1", cfg) == 0.0f);
    CHECK(fluxins::express("~1", cfg) == ~1);

    // Suffix unary operator
    CHECK(fluxins::express("4!", cfg) == 24.0f);
    CHECK(fluxins::express("5!", cfg) == 120.0f);

    // Conditional (ternary) operator
    CHECK(fluxins::express("1 ? 2 : 3", cfg) == 2.0f);
    CHECK(fluxins::express("0 ? 2 : 3", cfg) == 3.0f);
}

TEST_CASE("All operator precedence with all other operators")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Assumption: parenthesis works
    REQUIRE(fluxins::express("1 + (2 * 3)", cfg) == 7.0f);

    float a = 1, b = 2, c = 3, d = 4;

    for (const auto &op_main : cfg->binary_operators)
    {
        for (const auto &op_other : cfg->binary_operators)
        {
            if (op_main.symbol == op_other.symbol) continue;

            if (cfg->get_precedence(op_main.symbol) <= cfg->get_precedence(op_other.symbol)) continue;

            SUBCASE(std::format("Precedence: {} vs. {}", op_main.symbol, op_other.symbol).c_str())
            {
                // 1. Other on left: (a op_other b) op_main c
                {
                    std::string expr_paren = std::format("({} {} {}) {} {}", a, op_other.symbol, b, op_main.symbol, c);
                    std::string expr_flat  = std::format("{} {} {} {} {}", a, op_other.symbol, b, op_main.symbol, c);

                    CHECK_EXPR(expr_paren, expr_flat);
                }

                // 2. Other on right: a op_main (b op_other c)
                {
                    std::string expr_paren = std::format("{} {} ({} {} {})", a, op_main.symbol, b, op_other.symbol, c);
                    std::string expr_flat  = std::format("{} {} {} {} {}", a, op_main.symbol, b, op_other.symbol, c);

                    CHECK_EXPR(expr_paren, expr_flat);
                }

                // 3. Other on both sides: (a op_other b) op_main (c op_other a)
                {
                    std::string expr_paren = std::format("({} {} {}) {} ({} {} {})", a, op_other.symbol, b, op_main.symbol, c, op_other.symbol, d);
                    std::string expr_flat  = std::format("{} {} {} {} {} {} {}", a, op_other.symbol, b, op_main.symbol, c, op_other.symbol, d);

                    CHECK_EXPR(expr_paren, expr_flat);
                }
            }
        }
    }
}

TEST_CASE("All operator associativity")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Assumption: parenthesis works
    REQUIRE(fluxins::express("1 * (2 * 3)", cfg) == 6.0f);
    REQUIRE(fluxins::express("(1 * 2) * 3", cfg) == 6.0f);

    float a = 1, b = 2, c = 3;

    for (const auto &op : cfg->binary_operators)
    {
        SUBCASE(std::format("Associativity: {} ({})", op.symbol, op.assoc == fluxins::associativity::left ? "left" : op.assoc == fluxins::associativity::right ? "right"
                                                                                                                                                               : "unknown")
                    .c_str())
        {
            // Left associativity: a op b op c == (a op b) op c
            if (op.assoc == fluxins::associativity::left)
            {
                std::string expr_paren = std::format("({} {} {}) {} {}", a, op.symbol, b, op.symbol, c);
                std::string expr_flat  = std::format("{} {} {} {} {}", a, op.symbol, b, op.symbol, c);

                CHECK_EXPR(expr_paren, expr_flat);
            }

            // Right associativity: a op b op c == a op (b op c)
            else if (op.assoc == fluxins::associativity::right)
            {
                std::string expr_paren = std::format("{} {} ({} {} {})", a, op.symbol, b, op.symbol, c);
                std::string expr_flat  = std::format("{} {} {} {} {}", a, op.symbol, b, op.symbol, c);

                CHECK_EXPR(expr_paren, expr_flat);
            }
        }
    }
}

TEST_CASE("Nested ternary operator")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Assumption: parenthesis works
    auto ctx = std::make_shared<fluxins::context>();

    ctx->variables["a"] = 1.0f;
    ctx->variables["b"] = 2.0f;
    ctx->variables["c"] = 3.0f;
    ctx->variables["d"] = 4.0f;

    // Nested at left side
    REQUIRE(fluxins::express("0 ? (0 ? a : b) : c", cfg, ctx) == 3.0f);
    REQUIRE(fluxins::express("1 ? (0 ? a : b) : c", cfg, ctx) == 2.0f);
    REQUIRE(fluxins::express("0 ? (1 ? a : b) : c", cfg, ctx) == 3.0f);
    REQUIRE(fluxins::express("1 ? (1 ? a : b) : c", cfg, ctx) == 1.0f);

    // Nested at right side
    REQUIRE(fluxins::express("0 ? a : (0 ? b : c)", cfg, ctx) == 3.0f);
    REQUIRE(fluxins::express("1 ? a : (0 ? b : c)", cfg, ctx) == 1.0f);
    REQUIRE(fluxins::express("0 ? a : (1 ? b : c)", cfg, ctx) == 2.0f);
    REQUIRE(fluxins::express("1 ? a : (1 ? b : c)", cfg, ctx) == 1.0f);

    // Nested at both sides
    REQUIRE(fluxins::express("0 ? (0 ? a : b) : (0 ? c : d)", cfg, ctx) == 4.0f);
    REQUIRE(fluxins::express("1 ? (0 ? a : b) : (0 ? c : d)", cfg, ctx) == 2.0f);
    REQUIRE(fluxins::express("0 ? (1 ? a : b) : (0 ? c : d)", cfg, ctx) == 4.0f);
    REQUIRE(fluxins::express("1 ? (1 ? a : b) : (0 ? c : d)", cfg, ctx) == 1.0f);
    REQUIRE(fluxins::express("0 ? (0 ? a : b) : (1 ? c : d)", cfg, ctx) == 3.0f);
    REQUIRE(fluxins::express("1 ? (0 ? a : b) : (1 ? c : d)", cfg, ctx) == 2.0f);
    REQUIRE(fluxins::express("0 ? (1 ? a : b) : (1 ? c : d)", cfg, ctx) == 3.0f);
    REQUIRE(fluxins::express("1 ? (1 ? a : b) : (1 ? c : d)", cfg, ctx) == 1.0f);

    // Test nested ternary operator nested at left side
    CHECK(fluxins::express("0 ? 0 ? a : b : c", cfg, ctx) == 3.0f);
    CHECK(fluxins::express("1 ? 0 ? a : b : c", cfg, ctx) == 2.0f);
    CHECK(fluxins::express("0 ? 1 ? a : b : c", cfg, ctx) == 3.0f);
    CHECK(fluxins::express("1 ? 1 ? a : b : c", cfg, ctx) == 1.0f);

    // Test nested ternary operator nested at right side
    CHECK(fluxins::express("0 ? a : 0 ? b : c", cfg, ctx) == 3.0f);
    CHECK(fluxins::express("1 ? a : 0 ? b : c", cfg, ctx) == 1.0f);
    CHECK(fluxins::express("0 ? a : 1 ? b : c", cfg, ctx) == 2.0f);
    CHECK(fluxins::express("1 ? a : 1 ? b : c", cfg, ctx) == 1.0f);

    // Test nested ternary operator nested at both sides
    CHECK(fluxins::express("0 ? 0 ? a : b : 0 ? c : d", cfg, ctx) == 4.0f);
    CHECK(fluxins::express("1 ? 0 ? a : b : 0 ? c : d", cfg, ctx) == 2.0f);
    CHECK(fluxins::express("0 ? 1 ? a : b : 0 ? c : d", cfg, ctx) == 4.0f);
    CHECK(fluxins::express("1 ? 1 ? a : b : 0 ? c : d", cfg, ctx) == 1.0f);
    CHECK(fluxins::express("0 ? 0 ? a : b : 1 ? c : d", cfg, ctx) == 3.0f);
    CHECK(fluxins::express("1 ? 0 ? a : b : 1 ? c : d", cfg, ctx) == 2.0f);
    CHECK(fluxins::express("0 ? 1 ? a : b : 1 ? c : d", cfg, ctx) == 3.0f);
    CHECK(fluxins::express("1 ? 1 ? a : b : 1 ? c : d", cfg, ctx) == 1.0f);
}
