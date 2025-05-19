/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides the core of Fluxins: the expression class. It
/// contains pointers to parser configuration and global symbols context.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/parser.hpp"

namespace fluxins {

/// Fluxins expression.
///
/// Provided an expression, this class stores expression as string as is, and
/// parses as well as evaluates the expression.
///
/// @see `readme.md` for more information about expression grammar, syntax and
/// other relevant information.
struct expression {
    /// Expression string to parse and evaluate.
    ///
    /// Remember to call `parse()` when modifying the expression, and
    /// `evaluate()` to update the cached value.
    /// @note Empty expression string implies '0'.
    code expr;

    /// Configuration for parser. If `nullptr`, it will use default
    /// configuration.
    ///
    /// Remember to call `parse()` when modifying the config as well, and
    /// `evaluate()` to update the cached value.
    std::shared_ptr<config> cfg;

    /// The local list of symbols. Also includes links to other contexts to
    /// allow global sharing.
    ///
    /// Remember to call `evaluate()` when modifying the symbols in the
    /// expression or parent contexts.
    std::shared_ptr<context> ctx;

    /// Cached tokens after parsing.
    ///
    /// This is just here for debugging purposes.
    std::vector<token> tokens;

    /// Cached AST after parsing. This helps avoid re-parsing the expression
    /// when nothing has changed.
    ///
    /// Remember to call `parse()` when modifying the expression, and
    /// `evaluate()` to update the cached value.
    std::shared_ptr<ast_node> ast;

    /// Cached value after evaluation. This helps avoid re-evaluating the
    /// expression when nothing has changed.
    ///
    /// Remember to call `evaluate()` when modifying the symbols in the
    /// expression or parent contexts.
    float value = 0.0f;

    /// Parse the expression into cached AST.
    ///
    /// @exception code_error Thrown when syntactical error occurs during parsing.
    void parse();

    /// Evaluate the cached AST into cached value.
    ///
    /// @exception code_error Thrown when a referenced symbol is missing.
    void evaluate();

    /// Conversion operator to `float` type for ease of use.
    operator float()
    {
        return value;
    }

    /// Conversion operator to `std::string` type for ease of use.
    operator std::string()
    {
        return expr;
    }

    /// Obtain the value of the expression.
    ///
    /// This function will call parse() and evaluate() once.
    float get_value()
    {
        if (!ast)
        {
            parse();
            evaluate();
        }
        return value;
    }

    /// Set a variable to this expression's context.
    ///
    /// This will also create a context if it is absent (nullptr).
    expression &set_variable(const std::string &name, const fluxins_variable &variable)
    {
        if (!ctx)
        {
            ctx = std::make_shared<context>();
        }
        ctx->set_variable(name, variable);
        return *this;
    }

    /// Set a function to this expression's context.
    ///
    /// This will also create a context if it is absent (nullptr).
    expression &set_function(const std::string &name, const fluxins_function &function)
    {
        if (!ctx)
        {
            ctx = std::make_shared<context>();
        }
        ctx->set_function(name, function);
        return *this;
    }

    /// Inherit a context into this expression's context.
    ///
    /// This will also create a context if it is absent (nullptr).
    expression &inherit_context(std::shared_ptr<context> parent)
    {
        if (!ctx)
        {
            ctx = std::make_shared<context>();
        }
        ctx->inherit_context(parent);
        return *this;
    }
};

/// Evaluate an expression with the given configuration and context.
inline float express(std::string_view expr, std::shared_ptr<fluxins::config> cfg = nullptr, std::shared_ptr<fluxins::context> ctx = nullptr)
{
    return fluxins::expression(expr, cfg, ctx).get_value();
}

} // namespace fluxins
