/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides definitions for functions and variables and
/// context that holds local or global symbols.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "fluxins/code.hpp"
#include "fluxins/error.hpp"

/// For convenience, use `FLUXINS_FN_PARAMS` instead of
/// `const fluxins::code &expr, fluxins::code_location location, const std::vector<float> &params`.
#define FLUXINS_FN_PARAMS const fluxins::code &expr, fluxins::code_location location, const std::vector<float> &params

/// For convenience, use `FLUXINS_FN_ARITY` instead of
/// `if (params.size() != arity) { throw fluxins::invalid_arity("function_name", arity, params.size(), expr, location); }`.
#define FLUXINS_FN_ARITY(name, arity)                                                     \
    do                                                                                    \
    {                                                                                     \
        if (params.size() != (arity))                                                     \
        {                                                                                 \
            throw fluxins::invalid_arity((name), params.size(), (arity), expr, location); \
        }                                                                                 \
    }                                                                                     \
    while (0)

namespace fluxins {

/// Variable (or constant) type for local and global variables.
using fluxins_variable = float;

/// Function signature for local and global functions.
///
/// Use the code and location for construction of `code_error` exception, which
/// requires the original, full-form code and location from where the function
/// was called.
using fluxins_function = std::function<float(
    const code               &expr,
    code_location             location,
    const std::vector<float> &params)>;

using fluxins_variables = std::unordered_map<std::string, fluxins_variable>;
using fluxins_functions = std::unordered_map<std::string, fluxins_function>;

/// Context for expression's list of symbols.
struct context {
    /// Variables accessible to all expressions using this context.
    fluxins_variables variables;

    /// Functions accessible to all expressions using this context.
    fluxins_functions functions;

    /// Allow inheriting symbols from another contexts.
    /// @note This context's symbols are prioritized over inherited ones when
    ///       they conflict.
    std::vector<std::shared_ptr<context>> parents;

    /// Populates the context with built-in variables and functions.
    ///
    /// Populate the context with a set of standard mathematical variables
    /// (constants*), such as `pi`, `e`, and `phi`, which are aliases to values
    /// from the C++ `<numeric>` library. It also includes over 50 commonly used
    /// functions, such as `sin()`, `cos()`, `sqrt()`, `exp()`, `abs()`,
    /// `floor()`, `ceil()`, `min()`, and `max()`, mostly wrapping C++ standard
    /// library functions from headers like `<cmath>`, `<algorithm>`, and
    /// `<numeric>`.
    ///
    /// For the list of built-in symbols, see `builtins.cpp`.
    ///
    /// @note When a symbol already exists with the same name as the populating
    ///       symbols, the existing symbols are overriden. Be sure to populate
    ///       the context before adding custom symbols.
    void populate();

    /// Get variable from this context or it's parent contexts (recursively).
    std::optional<fluxins_variable> resolve_variable(const std::string &name) const;

    /// Get function from this context or it's parent contexts (recursively).
    std::optional<fluxins_function> resolve_function(const std::string &name) const;

    /// Assigns or inserts a variable to this context.
    /// @note This will override the variable if exists.
    context &set_variable(const std::string &name, const fluxins_variable &variable)
    {
        variables[name] = variable;
        return *this;
    }

    /// Assigns or inserts a function to this context.
    /// @note This will override the function if exists.
    context &set_function(const std::string &name, const fluxins_function &function)
    {
        functions[name] = function;
        return *this;
    }

    context &inherit_context(std::shared_ptr<context> parent)
    {
        parents.emplace_back(parent);
        return *this;
    }
};

} // namespace fluxins
