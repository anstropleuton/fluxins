/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests config's initial state and customization functionality.
///
/// This project is licensed under the terms of MIT License.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <memory>
#include <stdexcept>

#include "doctest/doctest.h"
#include "fluxins/config.hpp"
#include "fluxins/expression.hpp"

TEST_CASE("Initial configuration state")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Check that the default configuration is valid
    REQUIRE(cfg->unary_prefix_operators.size() == 6);
    REQUIRE(cfg->unary_suffix_operators.size() == 1);
    REQUIRE(cfg->binary_operators.size() == 25);
    REQUIRE(cfg->binary_op_precedence.size() == 13);

    // Check that the default configuration has all the operators
    CHECK(cfg->unary_prefix_op_exists("+"));
    CHECK(cfg->unary_prefix_op_exists("-"));
    CHECK(cfg->unary_prefix_op_exists("*"));
    CHECK(cfg->unary_prefix_op_exists("/"));
    CHECK(cfg->unary_prefix_op_exists("!"));
    CHECK(cfg->unary_prefix_op_exists("~"));

    CHECK(cfg->unary_suffix_op_exists("!"));

    CHECK(cfg->binary_op_exists("+"));
    CHECK(cfg->binary_op_exists("-"));
    CHECK(cfg->binary_op_exists("*"));
    CHECK(cfg->binary_op_exists("/"));
    CHECK(cfg->binary_op_exists("%"));
    CHECK(cfg->binary_op_exists("%%"));
    CHECK(cfg->binary_op_exists("**"));
    CHECK(cfg->binary_op_exists("//"));
    CHECK(cfg->binary_op_exists("=="));
    CHECK(cfg->binary_op_exists("!="));
    CHECK(cfg->binary_op_exists("<"));
    CHECK(cfg->binary_op_exists(">"));
    CHECK(cfg->binary_op_exists("<="));
    CHECK(cfg->binary_op_exists(">="));
    CHECK(cfg->binary_op_exists("&&"));
    CHECK(cfg->binary_op_exists("||"));
    CHECK(cfg->binary_op_exists("&"));
    CHECK(cfg->binary_op_exists("|"));
    CHECK(cfg->binary_op_exists("^"));
    CHECK(cfg->binary_op_exists("<<"));
    CHECK(cfg->binary_op_exists(">>"));
    CHECK(cfg->binary_op_exists("!!"));
    CHECK(cfg->binary_op_exists("??"));
    CHECK(cfg->binary_op_exists("<?"));
    CHECK(cfg->binary_op_exists(">?"));

    // Check that the default configuration has all the operators in the right
    // precedence level
    int precedence = 0;

    CHECK(cfg->get_precedence("<<") == precedence);
    CHECK(cfg->get_precedence(">>") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("^") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("&") == precedence);
    CHECK(cfg->get_precedence("|") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("!!") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("<?") == precedence);
    CHECK(cfg->get_precedence(">?") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("??") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("**") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("//") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("%") == precedence);
    CHECK(cfg->get_precedence("%%") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("*") == precedence);
    CHECK(cfg->get_precedence("/") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("+") == precedence);
    CHECK(cfg->get_precedence("-") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("==") == precedence);
    CHECK(cfg->get_precedence("!=") == precedence);
    CHECK(cfg->get_precedence("<") == precedence);
    CHECK(cfg->get_precedence(">") == precedence);
    CHECK(cfg->get_precedence("<=") == precedence);
    CHECK(cfg->get_precedence(">=") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("&&") == precedence);
    CHECK(cfg->get_precedence("||") == precedence);

    // Associativity check
    CHECK(cfg->get_binary_op("+").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("-").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("*").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("/").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("%").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("%%").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("**").assoc == fluxins::associativity::right);
    CHECK(cfg->get_binary_op("//").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("==").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("!=").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("<").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op(">").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("<=").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op(">=").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("&&").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("||").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("&").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("|").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("^").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("<<").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op(">>").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("!!").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op("??").assoc == fluxins::associativity::right);
    CHECK(cfg->get_binary_op("<?").assoc == fluxins::associativity::left);
    CHECK(cfg->get_binary_op(">?").assoc == fluxins::associativity::left);
}

TEST_CASE("Throw exceptions when invalid operator is specified")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Obtaining of invalid operator
    CHECK_THROWS_AS(cfg->get_unary_prefix_op("+++"), std::invalid_argument);
    CHECK_THROWS_AS(cfg->get_unary_suffix_op("+++"), std::invalid_argument);
    CHECK_THROWS_AS(cfg->get_binary_op("+++"), std::invalid_argument);
    CHECK_THROWS_AS(cfg->get_precedence("+++"), std::invalid_argument);

    // Addition of already existing operator
    CHECK_THROWS_AS(cfg->add_unary_prefix_op(cfg->unary_prefix_operators[0]), std::logic_error);
    CHECK_THROWS_AS(cfg->add_unary_suffix_op(cfg->unary_suffix_operators[0]), std::logic_error);
    CHECK_THROWS_AS(cfg->add_binary_op(cfg->binary_operators[0]), std::logic_error);

    // Removal of invalid operator
    CHECK_THROWS_AS(cfg->remove_unary_prefix_op("+++"), std::invalid_argument);
    CHECK_THROWS_AS(cfg->remove_unary_suffix_op("+++"), std::invalid_argument);
    CHECK_THROWS_AS(cfg->remove_binary_op("+++"), std::invalid_argument);

    // Precedence assigned to invalid operator
    CHECK_THROWS_AS(cfg->assign_precedence("+++", true), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", false), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu, true), std::invalid_argument); // int literal is ambiguous to bool vs. unsigned long (std::size_t)?? wtf C++? The zu suffix shall stay then...
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu, false), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu, true, true), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu, false, true), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu, true, false), std::invalid_argument);
    CHECK_THROWS_AS(cfg->assign_precedence("+++", 0zu, false, false), std::invalid_argument);

    // Precedence assigned to valid operator but with override set to false
    CHECK_THROWS_AS(cfg->assign_precedence("+", true), std::logic_error);
    CHECK_THROWS_AS(cfg->assign_precedence("+", false), std::logic_error);
    CHECK_THROWS_AS(cfg->assign_precedence("+", 0zu, true), std::logic_error);
    CHECK_THROWS_AS(cfg->assign_precedence("+", 0zu, false), std::logic_error);

    // Precedence unassigned to invalid operator
    CHECK_THROWS_AS(cfg->unassign_precedence("+++"), std::invalid_argument);

    // Invalid precedence level (out of range)
    CHECK_THROWS_AS(cfg->assign_precedence("+", cfg->binary_op_precedence.size(), false, true), std::out_of_range);
    CHECK_THROWS_AS(cfg->assign_precedence("+", cfg->binary_op_precedence.size() + 1, true, true), std::out_of_range);
}

TEST_CASE("Custom unary prefix operator")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Existence check for invalid operator should return false
    REQUIRE_FALSE(cfg->unary_prefix_op_exists("++"));

    // Create a new custom operator
    fluxins::unary_operator custom_op{
        "++",
        [](FLUXINS_UOP_PARAMS) {
            return x + 1.0f;
        },
    };

    cfg->add_unary_prefix_op(custom_op);

    REQUIRE(cfg->unary_prefix_op_exists("++"));
    CHECK(cfg->get_unary_prefix_op("++").operate({}, {}, 2.0f) == 3.0f);

    // Test the operator
    CHECK(fluxins::express("++2", cfg) == 3.0f);

    // Delete the custom operator
    cfg->remove_unary_prefix_op("++");

    CHECK_FALSE(cfg->unary_prefix_op_exists("++"));
}

TEST_CASE("Custom unary suffix operator")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Existence check for invalid operator should return false
    REQUIRE_FALSE(cfg->unary_suffix_op_exists("--"));

    // Create a new custom operator
    fluxins::unary_operator custom_op{
        "--",
        [](FLUXINS_UOP_PARAMS) {
            return x - 1.0f;
        },
    };

    cfg->add_unary_suffix_op(custom_op);

    REQUIRE(cfg->unary_suffix_op_exists("--"));
    CHECK(cfg->get_unary_suffix_op("--").operate({}, {}, 2.0f) == 1.0f);

    // Test the operator
    CHECK(fluxins::express("2--", cfg) == 1.0f);

    // Delete the custom operator
    cfg->remove_unary_suffix_op("--");

    CHECK_FALSE(cfg->unary_suffix_op_exists("--"));
}

TEST_CASE("Custom binary operator")
{
    auto cfg = std::make_shared<fluxins::config>();

    // Existence check for invalid operator should return false
    REQUIRE_FALSE(cfg->binary_op_exists("+++"));

    // Create a new custom operator
    fluxins::binary_operator custom_op{
        "+++",
        fluxins::associativity::right,
        [](FLUXINS_UOP_PARAMS, float y) {
            return 2.0f * x * y;
        },
    };

    cfg->add_binary_op(custom_op);
    cfg->assign_precedence("+++", true); // Least precedent

    REQUIRE(cfg->binary_op_exists("+++"));
    CHECK(cfg->get_precedence("+++") == cfg->binary_op_precedence.size() - 1);

    CHECK(cfg->get_binary_op("+++").assoc == fluxins::associativity::right);
    CHECK(cfg->get_binary_op("+++").operate({}, {}, 2.0f, 3.0f) == 2.0f * 3.0f + 2.0f * 3.0f);

    // Override the custom operator precedence to zero
    cfg->assign_precedence("+++", 0zu, false, true); // Assign to the first precedence level along with other operators in that level
    CHECK(cfg->get_precedence("+++") == 0);
    CHECK(cfg->binary_op_precedence[0zu].size() > 1);

    // Override the custom operator precedence to one with new row
    cfg->assign_precedence("+++", 1zu, true, true); // Assign to the second precedence level where the custom operator is the only one
    CHECK(cfg->get_precedence("+++") == 1);
    CHECK(cfg->binary_op_precedence[1zu].size() == 1);

    // Precedence check v2, with newly added custom operator
    int precedence = 0;

    CHECK(cfg->get_precedence("<<") == precedence);
    CHECK(cfg->get_precedence(">>") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("+++") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("^") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("&") == precedence);
    CHECK(cfg->get_precedence("|") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("!!") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("<?") == precedence);
    CHECK(cfg->get_precedence(">?") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("??") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("**") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("//") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("%") == precedence);
    CHECK(cfg->get_precedence("%%") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("*") == precedence);
    CHECK(cfg->get_precedence("/") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("+") == precedence);
    CHECK(cfg->get_precedence("-") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("==") == precedence);
    CHECK(cfg->get_precedence("!=") == precedence);
    CHECK(cfg->get_precedence("<") == precedence);
    CHECK(cfg->get_precedence(">") == precedence);
    CHECK(cfg->get_precedence("<=") == precedence);
    CHECK(cfg->get_precedence(">=") == precedence);

    precedence++;

    CHECK(cfg->get_precedence("&&") == precedence);
    CHECK(cfg->get_precedence("||") == precedence);

    // Test the operator
    CHECK(fluxins::express("2+++3", cfg) == 12.0f); // 2 * 3 + 2 * 3 = 12

    // Test the operator precedence with other operator
    CHECK(fluxins::express("2+++3*4", cfg) == 48.0f); // 12 * 4 = 48, since +++ is more precedent than *

    // Test the associativity of the operator
    CHECK(fluxins::express("2+++3+++4", cfg) == 96.0f); // 2+++(3+++4), we get 96, since +++ is right associative

    // Delete the custom operator
    cfg->unassign_precedence("+++");
    cfg->remove_binary_op("+++");

    CHECK_FALSE(cfg->binary_op_exists("+++"));
}
