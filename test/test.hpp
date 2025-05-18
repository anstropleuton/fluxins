/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides helpers for tests.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

//

#include <memory>

#include "fluxins/fluxins.hpp"

#define CHECK_EXPR(expected_expr, check_expr)             \
    do                                                    \
    {                                                     \
        CAPTURE(expected_expr);                           \
        CAPTURE(check_expr);                              \
        float expected = 0.0f;                            \
        try                                               \
        {                                                 \
            expected = express(expected_expr, cfg);       \
        }                                                 \
        catch (const fluxins::code_error &e)              \
        {                                                 \
            MESSAGE("Fluxins error: ", e.what());         \
            continue;                                     \
        }                                                 \
        if (std::isnan(expected) || std::isinf(expected)) \
            continue;                                     \
        CHECK(express(check_expr, cfg) == expected);      \
    } while (0)

inline float express(std::string_view expr, std::shared_ptr<fluxins::config> cfg, std::shared_ptr<fluxins::context> ctx = nullptr)
{
    return fluxins::expression(expr, cfg, ctx).get_value();
}

inline doctest::Approx apx(float value)
{
    return doctest::Approx(value).epsilon(0.001);
}
