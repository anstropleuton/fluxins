/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests basic expression parsing and evaluation of Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include "test.hpp"

//

#include <memory>

#include "fluxins/fluxins.hpp"

TEST_CASE("Basic expression parsing and evaluation")
{
    auto cfg = std::make_shared<fluxins::config>();

    CHECK(express("1 + 1", cfg) == 2.0f);
    CHECK(express("2 * (3 + 4)", cfg) == 14.0f);
    CHECK(express("2 ** 3 + 1", cfg) == 9.0f);
    CHECK(express("10 // 3 + 2 % 3", cfg) == 5.0f);
    CHECK(express("-2 %% 5 + 4", cfg) == 7.0f);
    CHECK(express("5 !! 3 + (4 <? 2)", cfg) == 4.0f);
    CHECK(express("8 >? 3 - 1", cfg) == 7.0f);
    CHECK(express("0 ?? 5 * 2", cfg) == 10.0);
    CHECK(express("1 ? 10 : 20 + 5", cfg) == 10.0f);
    CHECK(express("(1 ? 2 : 3) * (4 - 1)", cfg) == 6.0f);
    CHECK(express("(1 + 2) * 3 - 4 / 5 + 2 ** (1 + 1)", cfg) == 12.2f);
}

TEST_CASE("Basic expression with context")
{
    auto cfg = std::make_shared<fluxins::config>();

    CHECK(fluxins::expression("x + 1", cfg).set_variable("x", 5).get_value() == 6.0f);
    CHECK(fluxins::expression("y * 2 + 3", cfg).set_variable("y", 4).get_value() == 11.0f);
    CHECK(fluxins::expression("a - b / 2", cfg).set_variable("a", 10).set_variable("b", 6).get_value() == 7.0f);
    CHECK(fluxins::expression("(m + n) * 2", cfg).set_variable("m", 2).set_variable("n", 3).get_value() == 10.0f);
    CHECK(fluxins::expression("sum + offset", cfg).set_variable("sum", 8).set_variable("offset", 2).get_value() == 10.0f);
    CHECK(fluxins::expression("(u - v) ** w", cfg).set_variable("u", 5).set_variable("v", 2).set_variable("w", 3).get_value() == 27.0f);
    CHECK(fluxins::expression("flag ? a : b", cfg).set_variable("flag", 0).set_variable("a", 10).set_variable("b", 20).get_value() == 20.0f);
    CHECK(fluxins::expression("flag ? a : b", cfg).set_variable("flag", 1).set_variable("a", 10).set_variable("b", 20).get_value() == 10.0f);
    CHECK(fluxins::expression("p ?? q", cfg).set_variable("p", 0).set_variable("q", 7).get_value() == 7.0f);
    CHECK(fluxins::expression("p ?? q", cfg).set_variable("p", 5).set_variable("q", 7).get_value() == 5.0f);
    CHECK(fluxins::expression("double(3) + 1", cfg).set_function("double", [](FLUXINS_FN_PARAMS) { return params[0] * 2; }).get_value() == 7.0f);
    CHECK(fluxins::expression("quadruple(x)", cfg).set_variable("x", 2).set_function("quadruple", [](FLUXINS_FN_PARAMS) { return params[0] * 4; }).get_value() == 8.0f);
}

TEST_CASE("Basic expression with shared context")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto ctx1 = std::make_shared<fluxins::context>();
    ctx1->set_variable("a", 1);
    ctx1->set_variable("b", 2);
    ctx1->set_variable("c", 3);

    auto ctx2 = std::make_shared<fluxins::context>();
    ctx2->set_function("inc", [](FLUXINS_FN_PARAMS) { return params[0] + 1; });
    ctx2->set_function("square", [](FLUXINS_FN_PARAMS) { return params[0] * params[0]; });

    auto ctx3 = std::make_shared<fluxins::context>();
    ctx3->set_variable("x", 5);
    ctx3->set_variable("y", 10);
    ctx3->set_function("double", [](FLUXINS_FN_PARAMS) { return params[0] * 2; });

    auto ctx4 = std::make_shared<fluxins::context>();
    ctx4->set_variable("p", 3);
    ctx4->set_function("square", [](FLUXINS_FN_PARAMS) { return params[0] * params[0]; });

    CHECK(express("a + b + c", cfg, ctx1) == 6.0f);
    CHECK(express("a * b - c", cfg, ctx1) == -1.0f);
    CHECK(express("(a + b) * c", cfg, ctx1) == 9.0f);
    CHECK(express("inc(5)", cfg, ctx2) == 6.0f);
    CHECK(express("square(3)", cfg, ctx2) == 9.0f);
    CHECK(express("inc(square(2))", cfg, ctx2) == 5.0f);
    CHECK(express("double(x)", cfg, ctx3) == 10.0f);
    CHECK(express("x + double(y)", cfg, ctx3) == 25.0f);
    CHECK(express("double(x + y)", cfg, ctx3) == 30.0f);
    CHECK(express("square(p)", cfg, ctx4) == 9.0f);
    CHECK(express("p + square(p)", cfg, ctx4) == 12.0f);
    CHECK(express("square(p + 2)", cfg, ctx4) == 25.0f);
}
