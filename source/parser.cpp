/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source file provides implementation for parser in Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include "fluxins/parser.hpp"

#include <memory>
#include <vector>

#include "fluxins/config.hpp"
#include "fluxins/error.hpp"
#include "fluxins/fluxins.hpp"

std::vector<fluxins::token> fluxins::tokenize(const code &expr)
{
    std::vector<fluxins::token> tokens;

    const std::string identifier_start =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";

    const std::string number_start        = "0123456789";
    const std::string number_separator    = "'_";
    const std::string identifier_continue = identifier_start + number_start;
    const std::string number_continue     = number_start + ".";

    const std::string operator_chars    = "+-*/%^=!~&|<>?:[]";
    const std::string punctuation_chars = "(),";

    std::size_t index = 0;
    while (index < expr.expr.size())
    {
        // Identifier
        if (identifier_start.contains(expr.expr[index]))
        {
            std::size_t begin = index;

            while (index < expr.expr.size() && identifier_continue.contains(expr.expr[index]))
            {
                index++;
            }

            code_location location = {
                .begin   = begin,
                .length  = index - begin,
                .pointer = 0,
            };

            std::string value = std::string(expr.expr.substr(location.begin, location.length));

            tokens.emplace_back(token{
                .type     = token::token_type::identifier,
                .value    = value,
                .location = location,
            });
        }

        // Number
        else if (number_start.contains(expr.expr[index]))
        {
            std::size_t begin                         = index;
            bool        found_decimal_point           = false;
            bool        found_multiple_decimal_points = false;

            while (index < expr.expr.size() && number_continue.contains(expr.expr[index]))
            {
                // Must have only one '.' character
                if (expr.expr[index] == '.')
                {
                    if (found_decimal_point)
                    {
                        found_multiple_decimal_points = true;
                    }
                    found_decimal_point = true;
                }
                index++;
            }

            code_location location = {
                .begin   = begin,
                .length  = index - begin,
                .pointer = 0,
            };

            std::string raw = std::string(expr.expr.substr(location.begin, location.length));

            for (char separator : number_separator)
            {
                if (raw.ends_with(separator))
                {
                    throw tokenizer_error("Number cannot end with separator characters", expr, location);
                }
            }

            std::string value;

            // clang-format off

            // Remove 
            std::copy_if(raw.begin(), raw.end(), std::back_inserter(value),
                [&](char c) { return !number_separator.contains(c); });

            // clang-format on

            if (found_multiple_decimal_points)
            {
                throw tokenizer_error("Number cannot contain multiple decimal points", expr, location);
            }

            tokens.emplace_back(token{
                .type     = token::token_type::number,
                .value    = value,
                .location = location,
            });
        }

        // Operator, grouped (allows custom operators)
        else if (operator_chars.contains(expr.expr[index]))
        {
            std::size_t begin = index;

            while (index < expr.expr.size() && operator_chars.contains(expr.expr[index]))
            {
                index++;
            }

            code_location location = {
                .begin   = begin,
                .length  = index - begin,
                .pointer = 0,
            };

            std::string value = std::string(expr.expr.substr(location.begin, location.length));

            tokens.emplace_back(token{
                .type     = token::token_type::symbol,
                .value    = value,
                .location = location,
            });
        }

        // Punctuation, ungrouped, only one character for a valid punctuation
        else if (punctuation_chars.contains(expr.expr[index]))
        {
            code_location location = {
                .begin   = index,
                .length  = 1,
                .pointer = 0,
            };

            std::string value = std::string(1, expr.expr[index]);

            tokens.emplace_back(token{
                .type     = token::token_type::punctuation,
                .value    = value,
                .location = location,
            });

            index++;
        }

        // Whitespace
        else if (std::isspace(expr.expr[index]))
        {
            index++;
            continue;
        }

        // Invalid character
        else
        {
            code_location location = {
                .begin   = index,
                .length  = 1,
                .pointer = 0,
            };

            throw tokenizer_error("Invalid character", expr, location);
        }
    }

    return tokens;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_primary(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    std::shared_ptr<ast_node> node;

    bool prefix_op_found = false;

    if (pos >= tokens.size())
    {
        throw unexpected_token("Unexpected end of expression", expr, tokens[pos - 1]);
    }

    // Parse all prefix operators
    if (tokens[pos].type == token::token_type::symbol)
    {
        for (const auto &op_info : cfg->unary_prefix_operators)
        {
            if (tokens[pos].value == op_info.symbol)
            {
                const token &tok = tokens[pos++];

                auto operand = parse_primary(expr, tokens, cfg, pos);

                auto new_node      = std::make_shared<operator_ast>();
                new_node->symbol   = tok.value;
                new_node->right    = operand;
                new_node->location = tok.location;

                node            = new_node;
                prefix_op_found = true;

                break;
            }
        }
    }

    // No prefix operators, parse core primary expression
    if (!prefix_op_found)
    {
        if (tokens[pos].type == token::token_type::number)
        {
            node = parse_number(expr, tokens, cfg, pos);
        }
        else if (tokens[pos].type == token::token_type::identifier)
        {
            node = parse_identifier(expr, tokens, cfg, pos);
        }
        else if (tokens[pos].type == token::token_type::punctuation && tokens[pos].value == "(")
        {
            node = parse_parenthesis(expr, tokens, cfg, pos);
        }
        else
        {
            // Possibly unreachable code
            // Because the only other token type is symbol, and it is already parsed and consumed
            throw unexpected_token("Expected number, identifier or punctuation", expr, pos >= tokens.size() ? tokens[pos - 1] : tokens[pos]);
        }
    }

    // Parse suffix (or more prefix) operators
    bool more = true;

    while (more && pos < tokens.size() && tokens[pos].type == token::token_type::symbol)
    {
        more = false;
        for (const auto &op_info : cfg->unary_suffix_operators)
        {
            if (tokens[pos].value == op_info.symbol)
            {
                const token &tok      = tokens[pos++];
                auto         new_node = std::make_shared<operator_ast>();
                new_node->symbol      = tok.value;
                new_node->left        = node;
                new_node->location    = tok.location;

                node = new_node;
                more = true;

                break;
            }
        }
    }
    return node;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_number(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    const token &tok = tokens[pos++];

    if (tok.type != token::token_type::number)
    {
        // Possibly unreachable code
        // Because the caller explicitly checks for number token type
        throw unexpected_token("Expected number", expr, tok);
    }

    auto node      = std::make_shared<number_ast>();
    node->value    = std::stof(tok.value);
    node->location = tok.location;
    return node;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_identifier(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    if (tokens[pos].type != token::token_type::identifier)
    {
        // Possibly unreachable code
        // Because the caller explicitly checks for identifier token type
        throw unexpected_token("Expected identifier", expr, tokens[pos]);
    }

    // Check if identifier is followed by '(' for function
    if (pos + 1 < tokens.size() &&
        tokens[pos + 1].type == token::token_type::punctuation &&
        tokens[pos + 1].value == "(")
    {
        return parse_function(expr, tokens, cfg, pos);
    }

    return parse_variable(expr, tokens, cfg, pos);
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_variable(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    const token &tok = tokens[pos++];

    auto node      = std::make_shared<variable_ast>();
    node->name     = tok.value;
    node->location = tok.location;

    return node;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_function(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    auto node      = std::make_shared<function_ast>();
    node->name     = tokens[pos].value;
    node->location = tokens[pos].location;

    pos++;

    if (pos >= tokens.size() ||
        tokens[pos].type != token::token_type::punctuation ||
        tokens[pos].value != "(")
    {
        // Possibly unreachable code
        // Because the caller explicitly checks for '(' token type
        throw unexpected_token("Expected '(' after function name", expr, pos >= tokens.size() ? tokens[pos - 1] : tokens[pos]);
    }

    pos++; // Consume '('

    // Zero arguments?
    if (pos < tokens.size() && tokens[pos].type == token::token_type::punctuation && tokens[pos].value == ")")
    {
        pos++;
        return node;
    }

    // Parse arguments
    while (true)
    {
        node->args.push_back(parse_all(expr, tokens, cfg, pos));

        // Separate arguments based on ','
        if (pos < tokens.size() && tokens[pos].type == token::token_type::punctuation && tokens[pos].value == ",")
        {
            pos++;
            continue;
        }

        // Closing parenthesis ends the argument collection
        if (pos < tokens.size() && tokens[pos].type == token::token_type::punctuation && tokens[pos].value == ")")
        {
            pos++;
            break;
        }

        throw unexpected_token("Expected ',' or ')' in function arguments", expr, pos >= tokens.size() ? tokens[pos - 1] : tokens[pos]);
    }

    return node;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_parenthesis(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    if (tokens[pos].type != token::token_type::punctuation || tokens[pos].value != "(")
    {
        // Possibly unreachable code
        // Because the caller explicitly checks for '(' token type
        throw unexpected_token("Expected '('", expr, tokens[pos]);
    }

    pos++; // Consume '('
    auto node = parse_all(expr, tokens, cfg, pos);

    if (pos >= tokens.size() || tokens[pos].type != token::token_type::punctuation || tokens[pos].value != ")")
    {
        throw unexpected_token("Expected ')'", expr, pos >= tokens.size() ? tokens[pos - 1] : tokens[pos]);
    }

    pos++; // Consume ')'

    return node;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_binary_op(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos,
    std::size_t               prec)
{
    std::shared_ptr<ast_node> left;

    if (prec == 0)
    {
        left = parse_primary(expr, tokens, cfg, pos);
    }
    else
    {
        left = parse_binary_op(expr, tokens, cfg, pos, prec - 1);
    }

    // Match operators in the same level of precedence
    bool matched = true;
    while (matched && pos < tokens.size())
    {
        matched = false;
        for (std::size_t i : cfg->binary_op_precedence[prec])
        {
            const auto &op_info = cfg->binary_operators[i];

            // Operator did not match
            if (tokens[pos].type != token::token_type::symbol || tokens[pos].value != op_info.symbol)
            {
                continue;
            }

            matched          = true;
            const token &tok = tokens[pos++];

            std::shared_ptr<ast_node> right;
            if (prec == 0)
            {
                right = parse_primary(expr, tokens, cfg, pos);
            }
            else if (op_info.assoc == associativity::right)
            {
                right = parse_binary_op(expr, tokens, cfg, pos, prec);
            }
            else if (op_info.assoc == associativity::left)
            {
                right = parse_binary_op(expr, tokens, cfg, pos, prec - 1);
            }

            auto new_node      = std::make_shared<operator_ast>();
            new_node->symbol   = tok.value;
            new_node->left     = left;
            new_node->right    = right;
            new_node->location = tok.location;
            left               = new_node;

            break;
        }
    }

    return left;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_condition(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    std::shared_ptr<ast_node> condition;

    // No binary operators specified, parse only the primary expressions
    if (cfg->binary_op_precedence.size() == 0)
    {
        condition = parse_primary(expr, tokens, cfg, pos);
    }
    else
    {
        condition = parse_binary_op(expr, tokens, cfg, pos, cfg->binary_op_precedence.size() - 1);
    }

    // No '?' operator found, not a conditional operator
    if (pos >= tokens.size() || tokens[pos].type != token::token_type::symbol || tokens[pos].value != "?")
    {
        return condition;
    }

    code_location location = tokens[pos].location; // Location to the '?'
    pos++;

    auto true_value = parse_all(expr, tokens, cfg, pos);

    if (pos >= tokens.size() || tokens[pos].value != ":")
    {
        throw unexpected_token("Expected ':' in conditional expression", expr, pos >= tokens.size() ? tokens[pos - 1] : tokens[pos]);
    }

    pos++;
    auto false_value = parse_all(expr, tokens, cfg, pos);

    auto node         = std::make_shared<conditional_ast>();
    node->condition   = condition;
    node->true_value  = true_value;
    node->false_value = false_value;
    node->location    = location;

    return node;
}

std::shared_ptr<fluxins::ast_node> fluxins::parse_all(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg,
    std::size_t              &pos)
{
    return parse_condition(expr, tokens, cfg, pos);
}

std::shared_ptr<fluxins::ast_node> fluxins::parse(
    const code               &expr,
    const std::vector<token> &tokens,
    std::shared_ptr<config>   cfg)
{
    if (tokens.empty())
    {
        auto zero      = std::make_shared<number_ast>();
        zero->value    = 0.0f;
        zero->location = { 0, 0, 0 };
        return zero;
    }

    std::size_t pos  = 0;
    auto        node = parse_all(expr, tokens, cfg, pos);

    if (pos != tokens.size())
    {
        throw unexpected_token("Unexpected tokens after expression", expr, tokens[pos]);
    }

    return node;
}
