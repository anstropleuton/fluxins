/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This example file showcases general usage example for Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include <memory>
#include <print>
#include <string>

#include "fluxins/fluxins.hpp"

int main()
{
    // Create a configuration for parser and evaluator
    auto cfg = std::make_shared<fluxins::config>(); // Automatically creates default config

    // Create a context for global symbols for expression
    auto ctx = std::make_shared<fluxins::context>();

    // Add built-in symbols such as commonly used variables and functions
    ctx->populate();

    // Create an expression
    fluxins::expression my_expression("x ** 3 + 9 * x + quadruple(27)");

    // Attach config and inherit from our context
    my_expression.cfg = cfg;
    my_expression.inherit_context(ctx);

    // Add a global function "quadruple" that is shared across all
    // expressions that references our context
    ctx->set_function("quadruple", [](FLUXINS_FN_PARAMS) {
        if (params.size() != 1)
        {
            throw fluxins::code_error("quadruple() takes 1 argument", expr, location);
        }
        return params[0] * 4;
    });

    // Add a local variable "x", only exists within this expression's context
    my_expression.set_variable("x", 5);

    // Parse and evaluate the value
    my_expression.parse();    // Call this once and when modifying expression
    my_expression.evaluate(); // Call this once and when modifying expression or context
    float value = my_expression;

    // Update the local variable "x"
    my_expression.ctx->variables["x"] = 25;

    // Evaluate the expression to recalculate the result
    my_expression.evaluate(); // We call this since we modified context
    float updated_value = my_expression;

    // Change the expression completely and obtain new value (fancy)
    my_expression.expr = "quadruple(" + (std::string) my_expression + ")"; // Quadruple the final value

    // We call these both since we modified expression
    my_expression.parse();
    my_expression.evaluate();
    float quadrupled_value = my_expression;

    // Create another expression that shares our config and context
    fluxins::expression another_expression("4 + quadruple(y)", cfg, ctx);

    // Add a local variable "y" for our new expression
    another_expression.set_variable("y", 4);

    // Get the value by automatically parsing and evaluating _once_
    // Note: This does not automatically reparse or reevaluate the expression.
    //       It only serves as a quick helper to parse and evalutate the value
    //       once.
    float another_value = another_expression.get_value();

    // Print the values
    std::println("Value: {}", value);
    std::println("Updated value: {}", updated_value);
    std::println("Quadrupled value: {}", quadrupled_value);
    std::println("Another value: {}", another_value);

    // Create another expression that contains error
    fluxins::expression errored_expression("x + triple(y)", cfg, ctx);

    // Add variables
    errored_expression.set_variable("x", 2);
    errored_expression.set_variable("y", 3);

    try
    {
        // This will throw an error for missing function 'triple'
        float errored_value = errored_expression.get_value();
    }
    catch (const fluxins::code_error &error)
    {
        // Print the exception, along with the expression and location of the error
        std::print("Error parsing and evaluating expression: {}", error.what());
    }
}
