/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides definitions for parser-/evaluator-related classes
/// such as tokens and AST.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"

namespace fluxins {

/// A single token of the expression.
struct token {
    /// The type of a token.
    enum class token_type {
        /// Identifier starts with a-z, A-Z and '_', and can be followed by any
        /// number of a-z, A-Z, 0-9 or '_'.
        identifier,
        /// Number starts with 0-9 and can be followed by any number of 0-9 and `'`
        /// and can contain at most one '.' (must be in between).
        number,
        /// Operator can contain any number of `+`, `-`, `*`, `/`, `%`, `^`, `=`,
        /// `!`, `~`, `&`, `|`, `<`, `>`, `?`, `:`, `[` and `]`.
        symbol,
        /// Punctuation can contain one of `(`, `)` and `,`.
        punctuation,
        max
    };

    token_type type = token::token_type::max; ///< Token type.

    std::string   value;    ///< Value of the token (the token itself).
    code_location location; ///< Token location.
};

/// Tokenize the given expression string.
/// @exception code_error Thrown when invalid token was provided.
std::vector<token> tokenize(const code &expr);

/// Get the string representation of the token type for debugging.
std::string token_type_to_string(token::token_type type);

/// Get the string representation of the token for debugging.
std::string token_to_string(const code &expr, const token &tok);

/// Get the string representation of the tokens for debugging.
std::string tokens_to_string(const code &expr, const std::vector<token> &tokens);

/// Abstract Syntax Tree's base node structure.
struct ast_node {
    code_location           location; ///< Location of the AST.
    std::weak_ptr<ast_node> parent;   ///< Reference to parent of this node (if any).

    ast_node()          = default;
    virtual ~ast_node() = default;

    /// Evaluate this node (and children, if it contains any) for value.
    /// @exception code_error Thrown when invalid expression was provided.
    virtual float evaluate(
        const code              &expr,
        std::shared_ptr<config>  cfg,
        std::shared_ptr<context> ctx) const = 0;

    /// Get the string representation of this node and children for debugging.
    virtual std::string to_string(const code &expr, int indent = 0) const = 0;
};

/// Parse primary expression (initiates parsing of number, variable, function, etc.).
std::shared_ptr<ast_node> parse_primary(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse numeric expression.
std::shared_ptr<ast_node> parse_number(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse identifier expression (initiates parsing of variable and function).
std::shared_ptr<ast_node> parse_identifier(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse variable expression.
std::shared_ptr<ast_node> parse_variable(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse functional expression.
std::shared_ptr<ast_node> parse_function(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse expression with parenthesis.
std::shared_ptr<ast_node> parse_parenthesis(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse binary operator at precedence.
std::shared_ptr<ast_node> parse_binary_op(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos,
    std::size_t               prec);

/// Parse conditional operator.
std::shared_ptr<ast_node> parse_condition(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

std::shared_ptr<ast_node> parse_all(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos);

/// Parse the tokens into AST.
std::shared_ptr<ast_node> parse(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg);

/// AST node representing a number.
struct number_ast : ast_node {
    float value; ///< Value of the number.

    float evaluate(
        const code              &expr,
        std::shared_ptr<config>  cfg,
        std::shared_ptr<context> ctx) const override;

    std::string to_string(const code &expr, int indent = 0) const override;
};

/// AST node representing a variable.
struct variable_ast : ast_node {
    std::string name; ///< Name of the variable.

    float evaluate(
        const code              &expr,
        std::shared_ptr<config>  cfg,
        std::shared_ptr<context> ctx) const override;

    std::string to_string(const code &expr, int indent = 0) const override;
};

/// AST node representing a function call.
struct function_ast : ast_node {
    std::string                            name; ///< Name of the function.
    std::vector<std::shared_ptr<ast_node>> args; ///< Function arguments.

    float evaluate(
        const code              &expr,
        std::shared_ptr<config>  cfg,
        std::shared_ptr<context> ctx) const override;

    std::string to_string(const code &expr, int indent = 0) const override;
};

/// AST node representing an operator.
struct operator_ast : ast_node {
    std::string symbol; ///< Operator string.

    // The operator is binary if it has both left and right, unary if it has one
    // (prefix vs. suffix is determined whether it has operand on right or left)
    // and invalid node if it has none

    std::shared_ptr<ast_node> left;  ///< Left operand.
    std::shared_ptr<ast_node> right; ///< Right operand.

    float evaluate(
        const code              &expr,
        std::shared_ptr<config>  cfg,
        std::shared_ptr<context> ctx) const override;

    std::string to_string(const code &expr, int indent = 0) const override;
};

/// AST node representing a conditional operator.
struct conditional_ast : ast_node {
    std::shared_ptr<ast_node> condition;   ///< Condition expression.
    std::shared_ptr<ast_node> true_value;  ///< Expression if condition is true.
    std::shared_ptr<ast_node> false_value; ///< Expression if condition is false.

    float evaluate(
        const code              &expr,
        std::shared_ptr<config>  cfg,
        std::shared_ptr<context> ctx) const override;

    std::string to_string(const code &expr, int indent = 0) const override;
};

} // namespace fluxins
