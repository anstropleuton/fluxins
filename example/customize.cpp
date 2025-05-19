/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This example file showcases customization features of Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include <memory>
#include <print>
#include "fluxins/fluxins.hpp"

int main()
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();
    ctx->populate();

    // Customize the configuration

    // Add a custom unary prefix operator "++" that adds 1 to the value
    fluxins::unary_operator unary_op = {
        .symbol  = "++",
        .operate = [](FLUXINS_UOP_PARAMS) -> float {
            return x + 1;
        }
    };

    // Add the custom unary operator to the configuration
    cfg->add_unary_prefix_op(unary_op);

    // Add a custom unary suffix operator "--" that subtracts 1 from the value
    fluxins::unary_operator unary_op_suffix = {
        .symbol  = "--",
        .operate = [](FLUXINS_UOP_PARAMS) -> float {
            return x - 1;
        }
    };

    // Add the custom unary operator to the configuration
    cfg->add_unary_suffix_op(unary_op_suffix);

    // Add a custom binary operator "+++" that combines two values in a specific way
    fluxins::binary_operator binary_op = {
        .symbol  = "+++",
        .assoc   = fluxins::associativity::left,
        .operate = [](FLUXINS_BOP_PARAMS) -> float {
            return 2 * x * y; // Just a random operation
        }
    };

    // Add the custom binary operator to the configuration
    cfg->add_binary_op(binary_op);

    // Make sure to assign precedence to the custom binary operator for it to
    // be registered properly (the parser does not recognize operators without
    // precedence)
    cfg->assign_precedence("+++", true); // Least precedent

    // Let's use our custom operators in an expression
    fluxins::expression expr("++x + 2 * (3 + 4) - 5 + x +++ (y--)", cfg, ctx);
    expr.set_variable("x", 5);
    expr.set_variable("y", 10);

    try
    {
        expr.parse();
        expr.evaluate();
        float value = expr;

        std::println("Value: {}", value); // Should print "Value: 360"
    }
    catch (const fluxins::code_error &e)
    {
        std::println("Error: {}", e.what());
    }
}
