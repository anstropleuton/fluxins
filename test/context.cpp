/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests contexts and all their behaviors.
///
/// This project is licensed under the terms of MIT License.

#include "test.hpp"

//

#include <memory>

#include "fluxins/fluxins.hpp"

TEST_CASE("Basic context")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto ctx = std::make_shared<fluxins::context>();
    ctx->set_variable("x", 5);
    ctx->set_variable("y", 10);

    CHECK(ctx->variables["x"] == 5);
    CHECK(ctx->variables["y"] == 10);

    fluxins::expression expr("x + y", cfg, ctx);
    CHECK(expr.get_value() == 15.0f);
}

TEST_CASE("Context with custom functions")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto ctx = std::make_shared<fluxins::context>();
    ctx->set_variable("x", 5);
    ctx->set_variable("y", 10);

    ctx->set_function("add", [](const auto &, auto, const auto &params) {
        return params[0] + params[1];
    });

    fluxins::expression expr("add(x, y)", cfg, ctx);
    CHECK(expr.get_value() == 15.0f);
}

TEST_CASE("Shared context")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto ctx = std::make_shared<fluxins::context>();
    ctx->set_variable("x", 5);
    ctx->set_variable("y", 10);

    fluxins::expression expr("x + y", cfg);
    expr.inherit_context(ctx);
    CHECK(expr.get_value() == 15.0f);

    fluxins::expression expr2("x * y", cfg);
    expr2.inherit_context(ctx);
    CHECK(expr2.get_value() == 50.0f);
}

TEST_CASE("Context inheritance")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto parent_ctx = std::make_shared<fluxins::context>();
    parent_ctx->set_variable("x", 5);

    auto child_ctx = std::make_shared<fluxins::context>();
    child_ctx->set_variable("y", 10);
    child_ctx->inherit_context(parent_ctx);

    fluxins::expression expr("x + y", cfg);
    expr.inherit_context(child_ctx);
    CHECK(expr.get_value() == 15.0f);
}

TEST_CASE("Symbol shadowing")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto parent_ctx = std::make_shared<fluxins::context>();
    parent_ctx->set_variable("x", 5);

    auto child_ctx = std::make_shared<fluxins::context>();
    child_ctx->set_variable("x", 10); // Shadowing parent variable
    child_ctx->inherit_context(parent_ctx);

    fluxins::expression expr("x + 1", cfg);
    expr.inherit_context(child_ctx);
    CHECK(expr.get_value() == 11.0f); // Should use child's x
}

TEST_CASE("Deep inheritance")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto grandparent_ctx = std::make_shared<fluxins::context>();
    grandparent_ctx->set_variable("x", 5);

    auto parent_ctx = std::make_shared<fluxins::context>();
    parent_ctx->set_variable("y", 10);
    parent_ctx->inherit_context(grandparent_ctx);

    auto child_ctx = std::make_shared<fluxins::context>();
    child_ctx->set_variable("z", 15);
    child_ctx->inherit_context(parent_ctx);

    fluxins::expression expr("x + y + z", cfg);
    expr.inherit_context(child_ctx);
    CHECK(expr.get_value() == 30.0f);
}
