/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides error reporting mechanism as well as a way to
/// specify location of certain constructs in the expression, such as tokens.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <string>

#include "fluxins/code.hpp"

namespace fluxins {

/// A way to point to part of the expression.
/// Yes, location can span multiple lines.
struct code_location {
    std::size_t begin   = 0; ///< Beginning of the location.
    std::size_t length  = 0; ///< Length of the location.
    std::size_t pointer = 0; ///< Pointer within the text (relative) that is the important part.

    /// Get a preview text of the lines that the location spans.
    ///
    /// The preview text is in the following format:
    /// ```
    /// 123 | The line that caused an error within it.
    ///     |                      <~~^~~~>
    /// ```
    ///
    /// It can also span multiple lines, in which case the preview text will be:
    /// ```
    /// 122 | Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do
    ///     |                             <~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// 123 | eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut
    ///     | ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~
    /// 124 | enim ad minim veniam, quis nostrud exercitation ullamco laboris.
    ///     | ~~~~~~~~~~~~~~~~~~~>
    std::string preview_text(const code &expr, int padding = 0) const;
};

/// Code error exception, also contains details about the error and location of
/// the error in the code.
///
/// The "formatted message" is a string that contains the error message, the
/// location of the error, and the code itself as a preview text.
///
/// For example, e.what() may return:
/// ```
/// 00c872e7.flx: 123:24: The term "an error" is not defined.
/// 123 | The line that caused an error within it.
///     |                      <~~^~~~>
/// ```
///
/// @note Even with the existence of these specific subclasses of `code_error`
///       (see below), some part of the code will throw `code_error` directly
///       instead of its subclasses for uncategorized errors, such as division
///       by zero.
struct code_error : std::exception {
    std::string   message;     ///< Message of the error.
    code          source_code; ///< Reference to the code.
    code_location location;    ///< Location in the code that caused the error.

    std::string formatted_message; ///< Stores the formatted message (for `what()`).

    /// Initializes the exception. Also creates the formatted message.
    code_error(std::string_view message, const code &expr, code_location location);

    /// Obtain the error message (formatted).
    const char *what() const noexcept override
    {
        return formatted_message.c_str();
    }
};

/// Subclass of `code_error` for errors related to invalid number of arguments
/// (arity) of a function.
///
/// Throw from a function that requires a specific number of arguments but
/// got invalid number of arguments.
struct invalid_arity : code_error {
    std::string function;   ///< Name of the function that is throwing this exception.
    std::size_t args_count; ///< Number of arguments the function was called with.
    std::size_t arity;      ///< Number of arguments the function expects.

    invalid_arity(std::string_view function, std::size_t args_count, std::size_t arity, const code &expr, code_location location);
};

/// Subclass of `code_error` for errors from the tokenizer (lexer), related to
/// invalid characters for construction of a token.
///
/// Thrown when unexpected or invalid characters appear in the code in a way
/// that the tokenizer cannot handle.
struct tokenizer_error : code_error {
    tokenizer_error(std::string_view message, const code &expr, code_location location);
};

struct token; // FWD

/// Subclass of `code_error` for errors from the parser, related to unexpected
/// tokens while parsing.
///
/// Thrown when encountering invalid tokens for a specific parsing operation.
struct unexpected_token : code_error {
    /// Token that was found unexpectedly.
    /// @note This is a shared pointer because of the incomplete type `token`
    ///       with circular dependency when including `parser.hpp`.
    std::shared_ptr<token> unexpected;

    unexpected_token(std::string_view message, const code &expr, const token &unexpected);
};

/// Subclass of `code_error` for errors from the evaluator, related to
/// unresolvable references within the code.
///
/// Thrown when encountering a reference to a symbol (variable, function,
/// operator, etc.) that was not found within the code.
struct unresolved_reference : code_error {
    std::string symbol; ///< Name of the unresolved reference.
    std::string type;   ///< Type of the unresolved reference (e.g., "variable", "function", "operator").

    unresolved_reference(std::string_view symbol, std::string_view type, const code &expr, code_location location);
};

} // namespace fluxins
