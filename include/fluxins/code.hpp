/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// Provides a way to store code instead of it just being a string.
/// It stores reporting-related utilities along with the code, such as newline
/// locations.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <string>
#include <vector>

namespace fluxins {

/// Stores code and provides utilities.
struct code {
    std::string expr; ///< The code itself.
    std::string name; ///< Name of the code (initially randomly generated).

    // Utilities

    code() : expr(""), name("")
    {
        randomize_name();
    }

    code(std::string_view expr) : expr(expr), name("")
    {
        randomize_name();
        split_lines();
    }

    code(std::string_view expr, std::string name) : expr(expr), name(name)
    {
        split_lines();
    }

    code(const std::string &expr) : expr(expr), name("")
    {
        randomize_name();
        split_lines();
    }

    code(const std::string &expr, std::string name) : expr(expr), name(name)
    {
        split_lines();
    }

    code(const char *expr) : expr(expr), name("")
    {
        randomize_name();
        split_lines();
    }

    code(const char *expr, std::string name) : expr(expr), name(name)
    {
        split_lines();
    }

    template <typename type>
        requires requires(const type &value) { std::to_string(value); }
    code(const type &value) : expr(std::to_string(value)), name("")
    {}

    /// Randomize the name of the code.
    void randomize_name();

    /// Lines of the code.
    ///
    /// For each pair in this vector, the first element is the beginning index
    /// of the line, and the second element is the length of the line.
    ///
    /// @note Assuming the line breaks are `\n` characters. `\r\n` is not supported.
    std::vector<std::pair<std::size_t, std::size_t>> lines;

    /// Split the code into lines.
    void split_lines();

    /// Get line number and column number from the position.
    ///
    /// @note Line number starts from 1, column number starts from 0.
    ///       Do not just plug the first element from the pair into `lines[]`.
    ///
    /// @exception std::out_of_range Thrown when position is out of range.
    std::pair<std::size_t, std::size_t> get_line_col(std::size_t pos) const;

    /// Get line as string from the line number.
    ///
    /// @note Line number starts from 1.
    ///
    /// @exception std::out_of_range Thrown when line number is out of range.
    std::string get_line(std::size_t line_number) const;

    /// Get lines as string from a range of line numbers.
    ///
    /// @note Line numbers starts from 1.
    ///
    /// @exception std::out_of_range Thrown when line number is out of range.
    std::vector<std::string> get_lines(std::size_t begin_ln, std::size_t end_ln) const;

    operator std::string()
    {
        return expr;
    }

    operator const char *()
    {
        return expr.c_str();
    }
};

} // namespace fluxins
