/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source file provides implementation for misc. stuff in Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <iomanip>
#include <ios>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"
#include "fluxins/expression.hpp"
#include "fluxins/parser.hpp"

auto default_config = std::make_shared<fluxins::config>();

// Implementation

void fluxins::code::randomize_name()
{
    std::ostringstream oss;
    oss << std::hex << std::setw(8) << std::setfill('0') << std::rand();
    name = oss.str() + ".flx";
}

void fluxins::code::split_lines()
{
    std::size_t begin = 0;
    std::size_t end   = 0;

    while (end < expr.size())
    {
        // Newline is the delimiter, `\r\n` is not supported
        end = expr.find('\n', begin);
        if (end == std::string::npos)
        {
            end = expr.size();
        }

        lines.emplace_back(begin, end - begin);

        begin = end + 1;
    }
}

std::pair<std::size_t, std::size_t> fluxins::code::get_line_col(std::size_t pos) const
{
    for (std::size_t i = 0; i < lines.size(); i++)
    {
        auto [begin, length] = lines[i];
        if (pos >= begin && pos < begin + length)
        {
            return { i + 1, pos - begin };
        }
    }

    throw std::out_of_range("Position is out of range");
}

std::string fluxins::code::get_line(std::size_t line_number) const
{
    if (line_number == 0 || line_number > lines.size())
    {
        throw std::out_of_range("Line number is out of range");
    }

    auto [begin, length] = lines[line_number - 1];
    return expr.substr(begin, length);
}

std::vector<std::string> fluxins::code::get_lines(std::size_t begin_ln, std::size_t end_ln) const
{
    if (begin_ln == 0 || end_ln > lines.size() || begin_ln > end_ln)
    {
        throw std::out_of_range("Line number is out of range");
    }

    std::vector<std::string> result;
    for (std::size_t i = begin_ln - 1; i < end_ln; i++)
    {
        auto [begin, length] = lines[i];
        result.emplace_back(expr.substr(begin, length));
    }

    return result;
}

void fluxins::config::add_unary_prefix_op(const unary_operator &op)
{
    if (unary_prefix_op_exists(op.symbol))
    {
        throw std::logic_error(std::format("Unary prefix operator '{}' already exists", op.symbol));
    }

    unary_prefix_operators.emplace_back(op);
}

void fluxins::config::remove_unary_prefix_op(std::string_view symbol)
{
    if (!unary_prefix_op_exists(symbol))
    {
        throw std::invalid_argument(std::format("Cannot find unary prefix operator '{}'", symbol));
    }

    std::size_t index = find_unary_prefix_op(symbol);
    unary_prefix_operators.erase(unary_prefix_operators.begin() + index);
}

std::size_t fluxins::config::find_unary_prefix_op(std::string_view symbol) const
{
    auto it = std::find_if(unary_prefix_operators.begin(), unary_prefix_operators.end(), [&](const unary_operator &op) { return op.symbol == symbol; });

    if (it == unary_prefix_operators.end())
    {
        return (std::size_t) -1;
    }

    return std::distance(unary_prefix_operators.begin(), it);
}

bool fluxins::config::unary_prefix_op_exists(std::string_view symbol) const
{
    return find_unary_prefix_op(symbol) != (std::size_t) -1;
}

fluxins::unary_operator &fluxins::config::get_unary_prefix_op(std::string_view symbol)
{
    if (!unary_prefix_op_exists(symbol))
    {
        throw std::invalid_argument(std::format("Cannot find unary prefix operator '{}'", symbol));
    }

    return unary_prefix_operators[find_unary_prefix_op(symbol)];
}

void fluxins::config::add_unary_suffix_op(const unary_operator &op)
{
    if (unary_suffix_op_exists(op.symbol))
    {
        throw std::logic_error(std::format("Unary suffix operator '{}' already exists", op.symbol));
    }

    unary_suffix_operators.emplace_back(op);
}

void fluxins::config::remove_unary_suffix_op(std::string_view symbol)
{
    if (!unary_suffix_op_exists(symbol))
    {
        throw std::invalid_argument(std::format("Cannot find unary suffix operator '{}'", symbol));
    }

    std::size_t index = find_unary_suffix_op(symbol);
    unary_suffix_operators.erase(unary_suffix_operators.begin() + index);
}

std::size_t fluxins::config::find_unary_suffix_op(std::string_view symbol) const
{
    auto it = std::find_if(unary_suffix_operators.begin(), unary_suffix_operators.end(), [&](const unary_operator &op) { return op.symbol == symbol; });

    if (it == unary_suffix_operators.end())
    {
        return (std::size_t) -1;
    }

    return std::distance(unary_suffix_operators.begin(), it);
}

bool fluxins::config::unary_suffix_op_exists(std::string_view symbol) const
{
    return find_unary_suffix_op(symbol) != (std::size_t) -1;
}

fluxins::unary_operator &fluxins::config::get_unary_suffix_op(std::string_view symbol)
{
    if (!unary_suffix_op_exists(symbol))
    {
        throw std::invalid_argument(std::format("Cannot find unary suffix operator '{}'", symbol));
    }

    return unary_suffix_operators[find_unary_suffix_op(symbol)];
}

void fluxins::config::add_binary_op(const binary_operator &op)
{
    if (binary_op_exists(op.symbol))
    {
        throw std::logic_error(std::format("Binary operator '{}' already exists", op.symbol));
    }

    if (op.assoc == associativity::max)
    {
        throw std::logic_error(std::format("Binary operator '{}' has invalid associativity '{}'", op.symbol, associativity_to_string(op.assoc)));
    }

    binary_operators.emplace_back(op);
}

void fluxins::config::remove_binary_op(std::string_view symbol)
{
    if (!binary_op_exists(symbol))
    {
        throw std::invalid_argument(std::format("Cannot find binary operator '{}'", symbol));
    }

    std::size_t index = find_binary_op(symbol);
    binary_operators.erase(binary_operators.begin() + index);
}

std::size_t fluxins::config::find_binary_op(std::string_view symbol) const
{
    auto it = std::find_if(binary_operators.begin(), binary_operators.end(), [&](const binary_operator &op) { return op.symbol == symbol; });

    if (it == binary_operators.end())
    {
        return (std::size_t) -1;
    }

    return std::distance(binary_operators.begin(), it);
}

bool fluxins::config::binary_op_exists(std::string_view symbol) const
{
    return find_binary_op(symbol) != (std::size_t) -1;
}

fluxins::binary_operator &fluxins::config::get_binary_op(std::string_view symbol)
{
    if (!binary_op_exists(symbol))
    {
        throw std::invalid_argument(std::format("Cannot find binary operator '{}'", symbol));
    }

    return binary_operators[find_binary_op(symbol)];
}

void fluxins::config::assign_precedence(
    std::string_view symbol,
    std::size_t      precedence,
    bool             insert_row,
    bool             override)
{
    std::size_t index = find_binary_op(symbol);

    if (index == (std::size_t) -1)
    {
        throw std::invalid_argument(std::format("Cannot find binary operator '{}'", symbol));
    }

    // Check if the operator is already in the precedence level
    for (std::size_t i = 0; i < binary_op_precedence.size(); i++)
    {
        auto &row          = binary_op_precedence[i];
        auto  found_in_row = std::find(row.begin(), row.end(), index);
        if (found_in_row == row.end())
        {
            continue;
        }

        if (!override)
        {
            throw std::logic_error(std::format("Operator '{}' already exists in precedence level {}", symbol, i));
        }

        row.erase(found_in_row);
        if (row.empty())
        {
            binary_op_precedence.erase(binary_op_precedence.begin() + i);

            // Adjust the precedence level if we deleted row before the current one
            if (i < precedence)
            {
                precedence--;
            }
        }

        break;
    }

    // Insert a new row if requested
    if (insert_row)
    {
        if (precedence > binary_op_precedence.size())
        {
            throw std::out_of_range(std::format("Cannot insert precedence level {}, it is out of range", precedence));
        }

        binary_op_precedence.insert(binary_op_precedence.begin() + precedence, std::vector<std::size_t>());
    }

    if (precedence >= binary_op_precedence.size())
    {
        throw std::out_of_range(std::format("Cannot assign precedence level {}, it is out of range", precedence));
    }

    binary_op_precedence[precedence].emplace_back(index);
}

void fluxins::config::assign_precedence(
    std::string_view symbol,
    bool             insert_row,
    bool             override)
{
    assign_precedence(symbol, binary_op_precedence.size() - !insert_row, insert_row, override);
}

void fluxins::config::unassign_precedence(std::string_view symbol)
{
    auto it = std::find_if(binary_operators.begin(), binary_operators.end(), [&](const binary_operator &op) { return op.symbol == symbol; });

    if (it == binary_operators.end())
    {
        throw std::invalid_argument(std::format("Cannot find binary operator '{}'", symbol));
    }

    std::size_t index = std::distance(binary_operators.begin(), it);

    for (std::size_t i = 0; i < binary_op_precedence.size(); i++)
    {
        auto &row   = binary_op_precedence[i];
        auto  found = std::find(row.begin(), row.end(), index);
        if (found == row.end())
        {
            continue;
        }

        row.erase(found);
        if (row.empty())
        {
            binary_op_precedence.erase(binary_op_precedence.begin() + i);
        }

        break; // We know the operator is only in one precedence level
    }
}

std::size_t fluxins::config::get_precedence(std::string_view symbol) const
{
    auto it = std::find_if(binary_operators.begin(), binary_operators.end(), [&](const binary_operator &op) { return op.symbol == symbol; });

    if (it == binary_operators.end())
    {
        throw std::invalid_argument(std::format("Cannot find binary operator '{}'", symbol));
    }

    std::size_t index = std::distance(binary_operators.begin(), it);

    for (std::size_t i = 0; i < binary_op_precedence.size(); i++)
    {
        auto &row   = binary_op_precedence[i];
        auto  found = std::find(row.begin(), row.end(), index);
        if (found != row.end())
        {
            return i;
        }
    }

    return (std::size_t) -1;
}

std::optional<fluxins::fluxins_variable> fluxins::context::resolve_variable(const std::string &name) const
{
    if (variables.contains(name))
    {
        return variables.at(name);
    }

    for (const auto &parent : parents)
    {
        if (auto resolved = parent->resolve_variable(name))
        {
            return resolved;
        }
    }

    return std::nullopt;
}

std::optional<fluxins::fluxins_function> fluxins::context::resolve_function(const std::string &name) const
{
    if (functions.contains(name))
    {
        return functions.at(name);
    }

    for (const auto &parent : parents)
    {
        if (auto resolved = parent->resolve_function(name))
        {
            return resolved;
        }
    }

    return std::nullopt;
}

std::string fluxins::code_location::preview_text(const code &expr, int padding) const
{
    std::size_t begin_pos   = begin;
    std::size_t end_pos     = begin + length;
    std::size_t pointer_pos = begin + pointer;

    auto [begin_line, begin_col]     = expr.get_line_col(begin_pos);
    auto [end_line, end_col_inc]     = expr.get_line_col(end_pos - 1);
    auto [pointer_line, pointer_col] = expr.get_line_col(pointer_pos);
    std::size_t end_col_exc          = end_col_inc + 1;

    // Width for the line‑number column
    std::size_t        width = std::to_string(end_line).length();
    std::ostringstream out;

    // Emit each affected line plus its marker row
    for (std::size_t ln = begin_line; ln <= end_line; ++ln)
    {
        // Padding
        out << std::string(padding, ' ');

        out << std::setw(width) << ln << " | "
            << expr.get_line(ln) << "\n";

        // Marker line
        out << std::string(padding, ' ');

        out << std::string(width, ' ') << " | ";
        auto [_, line_len] = expr.lines[ln - 1];
        std::size_t start  = (ln == begin_line ? begin_col : 0);
        std::size_t end    = (ln == end_line ? end_col_exc : line_len);

        out << std::string(start, ' ');
        for (std::size_t c = start; c < end; ++c)
        {
            if (ln == pointer_line && c == pointer_col)
                out << '^';
            else if (ln == begin_line && c == start)
                out << '<';
            else if (ln == end_line && c == end - 1)
                out << '>';
            else
                out << '~';
        }
        out << "\n";
    }

    return out.str();
}

fluxins::code_error::code_error(std::string_view message, const code &expr, code_location location)
    : message(message), source_code(expr), location(location)
{
    std::ostringstream oss;
    auto [begin_line, begin_col] = source_code.get_line_col(location.begin);
    auto [end_line, end_col]     = source_code.get_line_col(location.begin + location.length - 1);
    oss << source_code.name << ": "
        << begin_line << ":" << begin_col << "-"
        << end_line << ":" << end_col << ": "
        << message << "\n"
        << location.preview_text(source_code);
    formatted_message = oss.str();
}

fluxins::invalid_arity::invalid_arity(std::string_view function, std::size_t args_count, std::size_t arity, const code &expr, code_location location)
    : code_error(std::format("Function '{}' requires {} arguments, but got {}", function, arity, args_count), expr, location)
{
}

fluxins::tokenizer_error::tokenizer_error(std::string_view message, const code &expr, code_location location)
    : code_error(message, expr, location)
{
}

fluxins::unexpected_token::unexpected_token(std::string_view messsage, const code &expr, const token &token)
    : code_error(messsage, expr, token.location)
{
}

fluxins::unresolved_reference::unresolved_reference(std::string_view name, std::string_view type, const code &expr, code_location location)
    : code_error(std::format("Unresolved reference to {} '{}'", type, name), expr, location)
{
}

void fluxins::expression::parse()
{
    tokens = tokenize(expr);
    ast    = ::fluxins::parse(expr, tokens, cfg ? cfg : default_config);
}

void fluxins::expression::evaluate()
{
    if (!ctx)
    {
        ctx = std::make_shared<context>();
    }
    value = ast->evaluate(expr, cfg ? cfg : default_config, ctx);
}
