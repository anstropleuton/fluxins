/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests `code_error` exception class and error reporting
/// mechanisms.
///
/// This project is licensed under the terms of MIT License.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <memory>

#include "doctest/doctest.h"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"
#include "fluxins/expression.hpp"

TEST_CASE("Invalid arity")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();

    ctx->set_function("add", [](FLUXINS_FN_PARAMS) {
        FLUXINS_FN_ARITY("add", 2);
        return params[0] + params[1];
    });

    CHECK_THROWS_AS(fluxins::express("add(1)", cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("add(1, 2, 3)", cfg, ctx), fluxins::invalid_arity);
}

TEST_CASE("Tokenizer error")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();

    CHECK_THROWS_AS(fluxins::express("#", cfg, ctx), fluxins::tokenizer_error);
    CHECK_THROWS_AS(fluxins::express("1'23'", cfg, ctx), fluxins::tokenizer_error);
    CHECK_THROWS_AS(fluxins::express("1.2.3", cfg, ctx), fluxins::tokenizer_error);
}

TEST_CASE("Unexpected token")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();

    ctx->set_function("add", [](FLUXINS_FN_PARAMS) {
        FLUXINS_FN_ARITY("add", 2);
        return params[0] + params[1];
    });

    CHECK_THROWS_AS(fluxins::express("3 + 4 5", cfg, ctx), fluxins::unexpected_token);
    CHECK_THROWS_AS(fluxins::express("add(6, 7 8)", cfg, ctx), fluxins::unexpected_token);
    CHECK_THROWS_AS(fluxins::express("(9 10)", cfg, ctx), fluxins::unexpected_token);
    CHECK_THROWS_AS(fluxins::express("11 ? 12 13", cfg, ctx), fluxins::unexpected_token);
}

TEST_CASE("Unresolved reference to variable")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();

    CHECK_THROWS_AS(fluxins::express("x + 1", cfg, ctx), fluxins::unresolved_reference);
    CHECK_THROWS_AS(fluxins::express("function(x)", cfg, ctx), fluxins::unresolved_reference);
}

// Special case
TEST_CASE("Unexpected end of expression")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();

    ctx->set_function("add", [](FLUXINS_FN_PARAMS) {
        FLUXINS_FN_ARITY("add", 2);
        return params[0] + params[1];
    });

    CHECK_THROWS_AS(fluxins::express("1 +", cfg, ctx), fluxins::unexpected_token);
    CHECK_THROWS_AS(fluxins::express("add(2, 3", cfg, ctx), fluxins::unexpected_token);
    CHECK_THROWS_AS(fluxins::express("(4 + 5", cfg, ctx), fluxins::unexpected_token);
    CHECK_THROWS_AS(fluxins::express("6 ? 7", cfg, ctx), fluxins::unexpected_token);
}

// Special case
TEST_CASE("Configuration changed after parsing")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();

    cfg->add_unary_prefix_op({ "++", [](FLUXINS_UOP_PARAMS) { return x + 1.0f; } });
    cfg->add_unary_suffix_op({ "--", [](FLUXINS_UOP_PARAMS) { return x - 1.0f; } });
    cfg->add_binary_op({ "+++", fluxins::associativity::right, [](FLUXINS_BOP_PARAMS) { return 2.0f * x * y; } });
    cfg->assign_precedence("+++", 0zu);

    fluxins::expression expr1("++2", cfg, ctx);
    fluxins::expression expr2("2--", cfg, ctx);
    fluxins::expression expr3("2 +++ 3", cfg, ctx);

    expr1.parse();
    expr2.parse();
    expr3.parse();

    cfg->remove_unary_prefix_op("++");
    cfg->remove_unary_suffix_op("--");
    cfg->remove_binary_op("+++");

    CHECK_THROWS_AS(expr1.evaluate(), fluxins::unresolved_reference);
    CHECK_THROWS_AS(expr2.evaluate(), fluxins::unresolved_reference);
    CHECK_THROWS_AS(expr3.evaluate(), fluxins::unresolved_reference);
}
