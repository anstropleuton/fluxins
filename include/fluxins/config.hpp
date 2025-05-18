/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides parser and evaluator configurations.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/error.hpp"

/// For convenience, use `FLUXINS_UOP_PARAMS` instead of
/// `const fluxins::code &expr, fluxins::code_location location, float x`.
#define FLUXINS_UOP_PARAMS const fluxins::code &expr, fluxins::code_location location, float x

/// For convenience, use `FLUXINS_BOP_PARAMS` instead of
/// `const fluxins::code &expr, fluxins::code_location location, float x, float y`.
#define FLUXINS_BOP_PARAMS const fluxins::code &expr, fluxins::code_location location, float x, float y

namespace fluxins {

/// Associativity (left or right) of binary operator.
enum class associativity {
    left,  ///< Associativity is from left to right (`a + b + c = (a + c) + c`).
    right, ///< Associativity is from right to left (`a ** b ** c = a ** (b ** c)`).
    max    ///< Default value for associativity (invalid).
};

/// Converts associativity to string for debugging.
std::string associativity_to_string(associativity assoc);

/// Unary operator type.
struct unary_operator {
    std::string symbol; ///< Unary operator symbol.

    /// Function to call when operator "operates" or performs its thing on a value.
    std::function<float(const code &expr, code_location location, float x)> operate;
};

/// Binary operator type.
struct binary_operator {
    std::string symbol; ///< Binary operator symbol.

    associativity assoc = associativity::max; ///< Associativity of binary operator.

    /// Function to call when operator "operates" or performs its thing on two values.
    std::function<float(const code &expr, code_location location, float x, float y)> operate;
};

/// Parser and evaluator configuration.
///
/// Contains parser and evaluator configuration (mainly custom operator
/// configuration) when parsing and evaluating expression. This allows
/// customizing the parser and evaluator for your needs.
///
/// @note The operator must be a valid operator token. See `parser.hpp` for more
///       information.
struct config {
    std::vector<unary_operator> unary_prefix_operators; ///< List of all unary prefix operators.
    std::vector<unary_operator> unary_suffix_operators; ///< List of all unary suffix operators.

    /// Appends a new unary prefix operator to the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    /// @exception std::logic_error Thrown when operator already exists in the
    ///            list of operators.
    void add_unary_prefix_op(const unary_operator &op);

    /// Removes a unary prefix operator from the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    void remove_unary_prefix_op(std::string_view symbol);

    /// Find the index of the unary prefix operator in the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    std::size_t find_unary_prefix_op(std::string_view symbol) const;

    /// Returns true when unary prefix operator of the symbol exists.
    bool unary_prefix_op_exists(std::string_view symbol) const;

    /// Get unary prefix operator from symbol.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    unary_operator &get_unary_prefix_op(std::string_view symbol);

    /// Appends a new unary suffix operator to the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    /// @exception std::logic_error Thrown when operator already exists in the
    ///            list of operators.
    void add_unary_suffix_op(const unary_operator &op);

    /// Removes a unary suffix operator from the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    void remove_unary_suffix_op(std::string_view symbol);

    /// Find the index of the unary suffix operator in the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    std::size_t find_unary_suffix_op(std::string_view symbol) const;

    /// Returns true when unary suffix operator of the symbol exists.
    bool unary_suffix_op_exists(std::string_view symbol) const;

    /// Get unary suffix operator from symbol.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    unary_operator &get_unary_suffix_op(std::string_view symbol);

    std::vector<binary_operator> binary_operators; ///< List of all binary operators.

    /// Binary operator precedence table.
    ///
    /// The outer vector is the list of precedences, where the first precedence
    /// row is the most precedent.
    ///
    /// The inner vector is the list of operators in that precedence level.
    ///
    /// The `std::size_t` is an index to the operator in `binary_operators` list.
    ///
    /// @note Operators without any defined precedence are not parsed. They are
    ///       effectively inexistent for the parser.
    std::vector<std::vector<std::size_t>> binary_op_precedence;

    /// Appends a new binary operator to the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    /// @exception std::logic_error Thrown when operator already exists in the
    ///            list of operators.
    /// @note Remember to assign precedence to the operator after adding it.
    void add_binary_op(const binary_operator &op);

    /// Removes a binary operator from the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    void remove_binary_op(std::string_view symbol);

    /// Find the index of the binary operator in the list of operators.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    /// @note Even if the binary operator exists, it does not imply that it has
    ///       a precedence level assigned.
    std::size_t find_binary_op(std::string_view symbol) const;

    /// Returns true when binary operator of the symbol exists.
    /// @note Even if the binary operator exists, it does not imply that it has
    ///       a precedence level assigned.
    bool binary_op_exists(std::string_view symbol) const;

    /// Get binary operator from symbol.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    binary_operator &get_binary_op(std::string_view symbol);

    /// Assign precedence level to binary operator.
    ///
    /// When `insert_row` is true, the operator will be inserted into the new
    /// precedence level and the rest of the operators will be shifted down.
    ///
    /// When `override` is true, the operator will be removed from its current
    /// precedence level and assigned to the new one.
    ///
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    /// @exception std::logic_error Thrown when operator already exists in the
    ///            precedence table and `override` is false.
    /// @exception std::out_of_range Thrown when precedence level is out of range.
    void assign_precedence(
        std::string_view symbol,
        std::size_t      precedence,
        bool             insert_row = false,
        bool             override   = false);

    /// Assign the least precedence to binary operator.
    ///
    /// When `insert_row` is true, the operator will be inserted into the new
    /// lowest precedence level.
    ///
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    /// @exception std::logic_error Thrown when operator already exists in the
    ///            precedence table and `override` is false.
    void assign_precedence(
        std::string_view symbol,
        bool             insert_row = false,
        bool             override   = false);

    /// Unassign precedence level from binary operator.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    void unassign_precedence(std::string_view symbol);

    /// Get the precedence level of a binary operator.
    /// @exception std::invalid_argument Thrown when invalid symbol is specified.
    std::size_t get_precedence(std::string_view symbol) const;

    /// Default constructor creates a default configuration with pre-defined
    /// operators.
    config();
};

} // namespace fluxins
