/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 0025 Anstro Pleuton
///
/// This source file contains list of all the built-in default global variables,
/// functions and operators defined in Fluxins.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <numbers>
#include <numeric>

#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"

#define ARITY_ZERO_OR_MORE (std::size_t) -1
#define ARITY_ONE_OR_MORE (std::size_t) -2

#define REGISTER_VARIABLE(name, value) \
    do                                 \
    {                                  \
        set_variable(name, value);     \
    }                                  \
    while (false)

#define REGISTER_FUNCTION(name, arity, ...)                                                                     \
    do                                                                                                          \
    {                                                                                                           \
        set_function(name, [](FLUXINS_FN_PARAMS) -> float {                                                     \
            if ((arity) == ARITY_ONE_OR_MORE)                                                                   \
            {                                                                                                   \
                if (params.size() == 0) throw fluxins::invalid_arity((name), params.size(), 1, expr, location); \
            }                                                                                                   \
            else if ((arity) != ARITY_ZERO_OR_MORE)                                                             \
            {                                                                                                   \
                FLUXINS_FN_ARITY((name), (arity));                                                              \
            }                                                                                                   \
            __VA_ARGS__                                                                                         \
        });                                                                                                     \
    }                                                                                                           \
    while (false)

// Utilities

static float factorial(float x)
{
    if (x < 0.0f)
        return 0.0f;

    if (x == 0.0f || x == 1.0f)
        return 1.0f;

    float result = 1.0f;
    for (int i = 1; i <= (int) x; i++)
    {
        result *= i;
    }

    return result;
}

static float wrapping_modulo(float x, float y)
{
    int ix = (int) x;
    int iy = (int) y;
    int r  = ix % iy;
    if (r < 0) r += iy;
    return (float) r;
}

// Implementation

void fluxins::context::populate()
{
    // clang-format off

    // NOTE: BE SURE TO MODIFY TEST FILES WHEN REGISTERING NEW VARIABLES OR
    // MODIFYING EXISTING ONES.
    REGISTER_VARIABLE("e",              std::numbers::e);
    REGISTER_VARIABLE("log2e",          std::numbers::log2e);
    REGISTER_VARIABLE("log10e",         std::numbers::log10e);
    REGISTER_VARIABLE("pi",             std::numbers::pi);
    REGISTER_VARIABLE("inv_pi",         std::numbers::inv_pi);
    REGISTER_VARIABLE("inv_sqrtpi",     std::numbers::inv_sqrtpi);
    REGISTER_VARIABLE("ln2",            std::numbers::ln2);
    REGISTER_VARIABLE("ln10",           std::numbers::ln10);
    REGISTER_VARIABLE("sqrt2",          std::numbers::sqrt2);
    REGISTER_VARIABLE("sqrt3",          std::numbers::sqrt3);
    REGISTER_VARIABLE("inv_sqrt3",      std::numbers::inv_sqrt3);
    REGISTER_VARIABLE("egamma",         std::numbers::egamma);
    REGISTER_VARIABLE("phi",            std::numbers::phi);
    REGISTER_VARIABLE("FE_DOWNWARD",    FE_DOWNWARD);
    REGISTER_VARIABLE("FE_TONEAREST",   FE_TONEAREST);
    REGISTER_VARIABLE("FE_TOWARDZERO",  FE_TOWARDZERO);
    REGISTER_VARIABLE("FE_UPWARD",      FE_UPWARD);

    // Commented-out functions are part of C++26 standard proposal. They are not
    // yet available for use.

    // NOTE: BE SURE TO MODIFY TEST FILES WHEN REGISTERING NEW FUNCTIONS OR
    // CHANGING EXISTING ONES.
    REGISTER_FUNCTION("abs",            1, return std::abs(params[0]););
    REGISTER_FUNCTION("acos",           1, return std::acos(params[0]););
    REGISTER_FUNCTION("acosh",          1, return std::acosh(params[0]););
    // REGISTER_FUNCTION("add_sat",        2, return std::add_sat(params[0], params[1]););
    REGISTER_FUNCTION("assoc_laguerre", 3, return std::assoc_laguerre(params[0], (int)std::round(params[1]), (int)std::round(params[2])););
    REGISTER_FUNCTION("assoc_legendre", 3, return std::assoc_legendre((int)std::round(params[0]), (int)std::round(params[1]), params[2]););
    REGISTER_FUNCTION("asin",           1, return std::asin(params[0]););
    REGISTER_FUNCTION("asinh",          1, return std::asinh(params[0]););
    REGISTER_FUNCTION("atan",           1, return std::atan(params[0]););
    REGISTER_FUNCTION("atan2",          2, return std::atan2(params[0], params[1]););
    REGISTER_FUNCTION("atanh",          1, return std::atanh(params[0]););
    REGISTER_FUNCTION("beta",           2, return std::beta(params[0], params[1]););
    REGISTER_FUNCTION("ceil",           1, return std::ceil(params[0]););
    REGISTER_FUNCTION("cbrt",           1, return std::cbrt(params[0]););
    REGISTER_FUNCTION("clamp",          3, return std::clamp(params[0], params[1], params[2]););
    REGISTER_FUNCTION("comp_ellint_1",  1, return std::comp_ellint_1(params[0]););
    REGISTER_FUNCTION("comp_ellint_2",  1, return std::comp_ellint_2(params[0]););
    REGISTER_FUNCTION("comp_ellint_3",  2, return std::comp_ellint_3(params[0], params[1]););
    REGISTER_FUNCTION("cos",            1, return std::cos(params[0]););
    REGISTER_FUNCTION("cosh",           1, return std::cosh(params[0]););
    REGISTER_FUNCTION("cyl_bessel_i",   2, return std::cyl_bessel_i(params[0], params[1]););
    REGISTER_FUNCTION("cyl_bessel_j",   2, return std::cyl_bessel_j(params[0], params[1]););
    REGISTER_FUNCTION("cyl_bessel_k",   2, return std::cyl_bessel_k(params[0], params[1]););
    REGISTER_FUNCTION("cyl_neumann",    2, return std::cyl_neumann(params[0], params[1]););
    REGISTER_FUNCTION("dim",            2, return std::fdim(params[0], params[1]););
    // REGISTER_FUNCTION("div_sat",        2, return std::div_sat(params[0], params[1]););
    REGISTER_FUNCTION("ellint_1",       2, return std::ellint_1(params[0], params[1]););
    REGISTER_FUNCTION("ellint_2",       2, return std::ellint_2(params[0], params[1]););
    REGISTER_FUNCTION("ellint_3",       3, return std::ellint_3(params[0], params[1], params[2]););
    REGISTER_FUNCTION("erf",            1, return std::erf(params[0]););
    REGISTER_FUNCTION("erfc",           1, return std::erfc(params[0]););
    REGISTER_FUNCTION("exp",            1, return std::exp(params[0]););
    REGISTER_FUNCTION("exp2",           1, return std::exp2(params[0]););
    REGISTER_FUNCTION("expint",         1, return std::expint(params[0]););
    REGISTER_FUNCTION("expm1",          1, return std::expm1(params[0]););
    REGISTER_FUNCTION("fegetround",     2, return (float)std::fegetround(););
    REGISTER_FUNCTION("fesetround",     1, return (float)std::fesetround(params[0]););
    REGISTER_FUNCTION("fma",            3, return std::fma(params[0], params[1], params[2]););
    REGISTER_FUNCTION("floor",          1, return std::floor(params[0]););
    REGISTER_FUNCTION("gcd",            2, return (float)std::gcd((int)std::round(params[0]), (int)std::round(params[1])););
    REGISTER_FUNCTION("hermite",        2, return std::hermite((int)std::round(params[0]), params[1]););
    REGISTER_FUNCTION("hypot",          2, return std::hypot(params[0], params[1]););
    REGISTER_FUNCTION("laguerre",       2, return std::laguerre((int)std::round(params[0]), params[1]););
    REGISTER_FUNCTION("legendre",       2, return std::legendre((int)std::round(params[0]), params[1]););
    REGISTER_FUNCTION("lgamma",         1, return std::lgamma(params[0]););
    REGISTER_FUNCTION("lcm",            2, return (float)std::lcm((int)std::round(params[0]), (int)std::round(params[1])););
    REGISTER_FUNCTION("lerp",           3, return std::lerp(params[0], params[1], params[2]););
    REGISTER_FUNCTION("log",            1, return std::log(params[0]););
    REGISTER_FUNCTION("log1p",          1, return std::log1p(params[0]););
    REGISTER_FUNCTION("log10",          1, return std::log10(params[0]););
    REGISTER_FUNCTION("log2",           1, return std::log2(params[0]););
    REGISTER_FUNCTION("max",            ARITY_ONE_OR_MORE, return std::ranges::max(params););
    REGISTER_FUNCTION("midpoint",       2, return std::midpoint(params[0], params[1]););
    REGISTER_FUNCTION("min",            ARITY_ONE_OR_MORE, return std::ranges::min(params););
    REGISTER_FUNCTION("mod",            2, return std::fmod(params[0], params[1]););
    // REGISTER_FUNCTION("mul_sat",        2, return std::mul_sat(params[0], params[1]););
    REGISTER_FUNCTION("nearbyint",      1, return std::nearbyint(params[0]););
    REGISTER_FUNCTION("pow",            2, return std::pow(params[0], params[1]););
    REGISTER_FUNCTION("remainder",      2, return std::remainder(params[0], params[1]););
    REGISTER_FUNCTION("riemann_zeta",   1, return std::riemann_zeta(params[0]););
    REGISTER_FUNCTION("rint",           1, return std::rint(params[0]););
    REGISTER_FUNCTION("round",          1, return std::round(params[0]););
    // REGISTER_FUNCTION("sub_sat",        2, return std::sub_sat(params[0], params[1]););
    REGISTER_FUNCTION("sin",            1, return std::sin(params[0]););
    REGISTER_FUNCTION("sinh",           1, return std::sinh(params[0]););
    REGISTER_FUNCTION("sph_bessel",     2, return std::sph_bessel((int)std::round(params[0]), params[1]););
    REGISTER_FUNCTION("sph_legendre",   3, return std::sph_legendre((int)std::round(params[0]), (int)std::round(params[1]), params[2]););
    REGISTER_FUNCTION("sph_neumann",    2, return std::sph_neumann((int)std::round(params[0]), params[1]););
    REGISTER_FUNCTION("sqrt",           1, return std::sqrt(params[0]););
    REGISTER_FUNCTION("tan",            1, return std::tan(params[0]););
    REGISTER_FUNCTION("tanh",           1, return std::tanh(params[0]););
    REGISTER_FUNCTION("tgamma",         1, return std::tgamma(params[0]););
    REGISTER_FUNCTION("trunc",          1, return std::trunc(params[0]););

    // A few custom functions
    REGISTER_FUNCTION("avg",            ARITY_ONE_OR_MORE, return std::accumulate(params.begin(), params.end(), 0.0f) / params.size(););
    REGISTER_FUNCTION("rand",           0, return (float)std::rand() / (float)RAND_MAX;);
    REGISTER_FUNCTION("srand",          1, std::srand((unsigned int)params[0]); return 0.0f;);
    REGISTER_FUNCTION("time",           0, return (float)std::time(nullptr););

    // clang-format on
}

fluxins::config::config()
{
    // clang-format off

    // NOTE: BE SURE TO MODIFY TEST FILES AND readme.md FILE WHEN ADDING
    // NEW OPERATORS OR MODIFYING EXISTING ONES.
    unary_prefix_operators = {
        unary_operator{ "+", [](FLUXINS_UOP_PARAMS) -> float { return 0.0f + x; } },
        unary_operator{ "-", [](FLUXINS_UOP_PARAMS) -> float { return 0.0f - x; } },
        unary_operator{ "*", [](FLUXINS_UOP_PARAMS) -> float { return 1.0f * x; } },
        unary_operator{ "/", [](FLUXINS_UOP_PARAMS) -> float { if (x == 0.0f) throw code_error("Division by zero", expr, location); return 1.0f / x; } },
        unary_operator{ "!", [](FLUXINS_UOP_PARAMS) -> float { return x == 0.0f; } },
        unary_operator{ "~", [](FLUXINS_UOP_PARAMS) -> float { return ~(int)(x); } },
    };

    unary_suffix_operators = {
        unary_operator{ "!", [](FLUXINS_UOP_PARAMS) -> float { return factorial(x); } },
    };

    binary_operators = {
        { "+",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return x + y; } },
        { "-",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return x - y; } },
        { "*",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return x * y; } },
        { "/",  associativity::left,  [](FLUXINS_BOP_PARAMS) { if (y == 0.0f) throw code_error("Division by zero", expr, location); return x / y; } },
        { "%",  associativity::left,  [](FLUXINS_BOP_PARAMS) { if (y == 0.0f) throw code_error("Modulo by zero", expr, location); return std::fmod(x, y); } },
        { "%%", associativity::left,  [](FLUXINS_BOP_PARAMS) { if (y == 0.0f) throw code_error("Wrapping modulo by zero", expr, location); return wrapping_modulo(x, y); } },
        { "**", associativity::right, [](FLUXINS_BOP_PARAMS) { return std::pow(x, y); } },
        { "//", associativity::left,  [](FLUXINS_BOP_PARAMS) { if (y == 0.0f) throw code_error("Flooring division by zero", expr, location); return std::floor(x / y); } },
        { "==", associativity::left,  [](FLUXINS_BOP_PARAMS) { return x == y; } },
        { "!=", associativity::left,  [](FLUXINS_BOP_PARAMS) { return x != y; } },
        { "<",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return x < y; } },
        { ">",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return x > y; } },
        { "<=", associativity::left,  [](FLUXINS_BOP_PARAMS) { return x <= y; } },
        { ">=", associativity::left,  [](FLUXINS_BOP_PARAMS) { return x >= y; } },
        { "&&", associativity::left,  [](FLUXINS_BOP_PARAMS) { return (x != 0.0f && y != 0.0f); } },
        { "||", associativity::left,  [](FLUXINS_BOP_PARAMS) { return (x != 0.0f || y != 0.0f); } },
        { "&",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return (float)((int) x & (int) y); } },
        { "|",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return (float)((int) x | (int) y); } },
        { "^",  associativity::left,  [](FLUXINS_BOP_PARAMS) { return (float)((int) x ^ (int) y); } },
        { "<<", associativity::left,  [](FLUXINS_BOP_PARAMS) { return (float)((int) x << (int) y); } },
        { ">>", associativity::left,  [](FLUXINS_BOP_PARAMS) { return (float)((int) x >> (int) y); } },
        { "!!", associativity::left,  [](FLUXINS_BOP_PARAMS) { return std::fabs(x - y); } },
        { "??", associativity::right, [](FLUXINS_BOP_PARAMS) { return x != 0.0f ? x : y; } },
        { "<?", associativity::left,  [](FLUXINS_BOP_PARAMS) { return std::fmin(x, y); } },
        { ">?", associativity::left,  [](FLUXINS_BOP_PARAMS) { return std::fmax(x, y); } },
    };

    // Precedence in order of highest to lowest

    assign_precedence("<<", true);
    assign_precedence(">>", false);

    assign_precedence("^",  true);

    assign_precedence("&",  true);
    assign_precedence("|",  false);

    assign_precedence("!!", true);

    assign_precedence("<?", true);
    assign_precedence(">?", false);

    assign_precedence("??", true);

    assign_precedence("**", true);

    assign_precedence("//", true);

    assign_precedence("%",  true);
    assign_precedence("%%", false);

    assign_precedence("*",  true);
    assign_precedence("/",  false);

    assign_precedence("+",  true);
    assign_precedence("-",  false);

    assign_precedence("==", true);
    assign_precedence("!=", false);
    assign_precedence("<",  false);
    assign_precedence(">",  false);
    assign_precedence("<=", false);
    assign_precedence(">=", false);

    assign_precedence("&&", true);
    assign_precedence("||", false);

    // clang-format on
}
