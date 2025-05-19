/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source file provides implementation for evaluataion in Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include <cstddef>
#include <memory>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"
#include "fluxins/parser.hpp"

float fluxins::number_ast::evaluate(
    const code              &expr,
    std::shared_ptr<config>  cfg,
    std::shared_ptr<context> ctx) const
{
    return value;
}

float fluxins::variable_ast::evaluate(
    const code              &expr,
    std::shared_ptr<config>  cfg,
    std::shared_ptr<context> ctx) const
{
    if (auto resolved = ctx->resolve_variable(name))
    {
        return *resolved;
    }

    throw unresolved_reference(name, "variable", expr, location);
}

float fluxins::function_ast::evaluate(
    const code              &expr,
    std::shared_ptr<config>  cfg,
    std::shared_ptr<context> ctx) const
{
    fluxins_function function;

    if (auto resolved = ctx->resolve_function(name))
    {
        function = *resolved;
    }

    if (!function)
    {
        throw unresolved_reference(name, "function", expr, location);
    }

    std::vector<float> evaluated_args(args.size());
    for (std::size_t i = 0; i < args.size(); i++)
    {
        evaluated_args[i] = args[i]->evaluate(expr, cfg, ctx);
    }

    return function(expr, location, evaluated_args);
}

float fluxins::operator_ast::evaluate(
    const code              &expr,
    std::shared_ptr<config>  cfg,
    std::shared_ptr<context> ctx) const
{
    float left_value  = left ? left->evaluate(expr, cfg, ctx) : 0.0f;
    float right_value = right ? right->evaluate(expr, cfg, ctx) : 0.0f;

    if (left && right)
    {
        if (!cfg->binary_op_exists(symbol))
        {
            // Possibly unreachable code
            // Can happen if the configuration is modified after the expression is parsed
            throw unresolved_reference(symbol, "binary operator", expr, location);
        }

        const auto &op_info = cfg->get_binary_op(symbol);
        return op_info.operate(expr, location, left_value, right_value);
    }
    else if (left)
    {
        if (!cfg->unary_suffix_op_exists(symbol))
        {
            // Possibly unreachable code
            // Can happen if the configuration is modified after the expression is parsed
            throw unresolved_reference(symbol, "unary prefix operator", expr, location);
        }

        const auto &op_info = cfg->get_unary_suffix_op(symbol);
        return op_info.operate(expr, location, left_value);
    }
    else if (right)
    {
        if (!cfg->unary_prefix_op_exists(symbol))
        {
            // Possibly unreachable code
            // Can happen if the configuration is modified after the expression is parsed
            throw unresolved_reference(symbol, "unary prefix operator", expr, location);
        }

        const auto &op_info = cfg->get_unary_prefix_op(symbol);
        return op_info.operate(expr, location, right_value);
    }

    // Possibly unreachable code here
    // Can happen if the configuration is modified after the expression is parsed
    throw code_error("No operands for operator was specified", expr, location);
}

float fluxins::conditional_ast::evaluate(
    const code              &expr,
    std::shared_ptr<config>  cfg,
    std::shared_ptr<context> ctx) const
{
    float condition_value = condition->evaluate(expr, cfg, ctx);

    if (condition_value != 0.0f)
    {
        return true_value->evaluate(expr, cfg, ctx);
    }
    else
    {
        return false_value->evaluate(expr, cfg, ctx);
    }
}
