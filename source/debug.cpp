/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source file provides implementation to obtain debug info for Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include <format>
#include <string>
#include <string_view>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/config.hpp"
#include "fluxins/parser.hpp"

std::string repeat_string(std::string_view str, int count)
{
    std::string result;
    for (int i = 0; i < count; ++i)
    {
        result += str;
    }
    return result;
}

std::string fluxins::associativity_to_string(associativity assoc)
{
    switch (assoc)
    {
        case associativity::left:  return "left";
        case associativity::right: return "right";
        default:                   return "unknown";
    }
}

std::string fluxins::token_type_to_string(token::token_type type)
{
    switch (type)
    {
        case token::token_type::identifier:
            return "identifier";
        case token::token_type::number:
            return "number";
        case token::token_type::symbol:
            return "symbol";
        case token::token_type::punctuation:
            return "punctuation";
        default:
            return "unknown";
    }
}

std::string fluxins::token_to_string(const code &expr, const token &tok)
{
    std::string str = std::format(
        "Token: Type: {}, Value: {}, Location: {}:{}\n{}",
        token_type_to_string(tok.type),
        tok.value,
        tok.location.begin,
        tok.location.length,
        tok.location.preview_text(expr));

    return str;
}

std::string fluxins::tokens_to_string(const code &expr, const std::vector<token> &tokens)
{
    std::string str;
    for (const auto &tok : tokens)
    {
        str += token_to_string(expr, tok);
    }
    return str;
}

std::string fluxins::number_ast::to_string(const code &expr, int indent) const
{
    std::string padding = repeat_string("  ", indent);
    std::string str     = padding;
    str += std::format("Number: {}, Location: {}:{}\n{}", value, location.begin, location.length, location.preview_text(expr, indent * 2));
    return str;
}

std::string fluxins::variable_ast::to_string(const code &expr, int indent) const
{
    std::string padding = repeat_string("  ", indent);
    std::string str     = padding;
    str += std::format("Variable: {}, Location: {}:{}\n{}", name, location.begin, location.length, location.preview_text(expr, indent * 2));
    return str;
}

std::string fluxins::function_ast::to_string(const code &expr, int indent) const
{
    std::string padding = repeat_string("  ", indent);
    std::string str     = padding;
    str += std::format("Function: {}, Location: {}:{}\n{}", name, location.begin, location.length, location.preview_text(expr, indent * 2));
    str += padding;
    str += "Arguments:\n";
    for (const auto &arg : args)
    {
        str += arg->to_string(expr, indent + 1);
    }
    return str;
}

std::string fluxins::operator_ast::to_string(const code &expr, int indent) const
{
    std::string padding = repeat_string("  ", indent);
    std::string str     = padding;
    str += std::format("Operator: {}, Location: {}:{}\n{}", symbol, location.begin, location.length, location.preview_text(expr, indent * 2));
    str += padding;
    str += std::format("Has left: {}, has right: {}\n", left != nullptr, right != nullptr);
    if (left)
    {
        str += padding;
        str += "Left:\n";
        str += left->to_string(expr, indent + 1);
    }
    if (right)
    {
        str += padding;
        str += "Right:\n";
        str += right->to_string(expr, indent + 1);
    }
    return str;
}

std::string fluxins::conditional_ast::to_string(const code &expr, int indent) const
{
    std::string padding = repeat_string("  ", indent);
    std::string str     = padding;
    str += std::format("Conditional: Location: {}:{}\n{}", location.begin, location.length, location.preview_text(expr, indent * 2));
    str += padding;
    str += "Condition:\n";
    str += condition->to_string(expr, indent + 1);
    str += padding;
    str += "True value:\n";
    str += true_value->to_string(expr, indent + 1);
    str += padding;
    str += "False value:\n";
    str += false_value->to_string(expr, indent + 1);
    return str;
}
