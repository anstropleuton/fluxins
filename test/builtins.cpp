/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests all built-in symbols from a populated context.
///
/// This project is licensed under the terms of MIT License.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cfenv>
#include <ctime>
#include <memory>
#include <numbers>

#include "doctest/doctest.h"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"
#include "fluxins/expression.hpp"

inline doctest::Approx apx(float value)
{
    return doctest::Approx(value).epsilon(0.001);
}

TEST_CASE("Built-in symbols")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto ctx = std::make_shared<fluxins::context>();
    ctx->populate();

    // clang-format off
    CHECK(fluxins::express("e",                      cfg, ctx) == apx(std::numbers::e));
    CHECK(fluxins::express("log2e",                  cfg, ctx) == apx(std::numbers::log2e));
    CHECK(fluxins::express("log10e",                 cfg, ctx) == apx(std::numbers::log10e));
    CHECK(fluxins::express("pi",                     cfg, ctx) == apx(std::numbers::pi));
    CHECK(fluxins::express("inv_pi",                 cfg, ctx) == apx(std::numbers::inv_pi));
    CHECK(fluxins::express("inv_sqrtpi",             cfg, ctx) == apx(std::numbers::inv_sqrtpi));
    CHECK(fluxins::express("ln2",                    cfg, ctx) == apx(std::numbers::ln2));
    CHECK(fluxins::express("ln10",                   cfg, ctx) == apx(std::numbers::ln10));
    CHECK(fluxins::express("sqrt2",                  cfg, ctx) == apx(std::numbers::sqrt2));
    CHECK(fluxins::express("sqrt3",                  cfg, ctx) == apx(std::numbers::sqrt3));
    CHECK(fluxins::express("inv_sqrt3",              cfg, ctx) == apx(std::numbers::inv_sqrt3));
    CHECK(fluxins::express("egamma",                 cfg, ctx) == apx(std::numbers::egamma));
    CHECK(fluxins::express("phi",                    cfg, ctx) == apx(std::numbers::phi));
    CHECK(fluxins::express("FE_DOWNWARD",            cfg, ctx) == apx(FE_DOWNWARD));
    CHECK(fluxins::express("FE_TONEAREST",           cfg, ctx) == apx(FE_TONEAREST));
    CHECK(fluxins::express("FE_TOWARDZERO",          cfg, ctx) == apx(FE_TOWARDZERO));
    CHECK(fluxins::express("FE_UPWARD",              cfg, ctx) == apx(FE_UPWARD));

    CHECK(fluxins::express("abs(-5)",                cfg, ctx) == apx( 5.0f));
    CHECK(fluxins::express("acos(1)",                cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("acosh(1)",               cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("assoc_laguerre(3,0,0)",  cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("assoc_legendre(0,0,2)",  cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("asin(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("asinh(0)",               cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("atan(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("atan2(0,1)",             cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("atanh(0)",               cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("beta(1,1)",              cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("ceil(2.3)",              cfg, ctx) == apx( 3.0f));
    CHECK(fluxins::express("cbrt(27)",               cfg, ctx) == apx( 3.0f));
    CHECK(fluxins::express("clamp(20,5,10)",         cfg, ctx) == apx( 10.0f));
    CHECK(fluxins::express("comp_ellint_1(0)",       cfg, ctx) == apx( 1.57079f));
    CHECK(fluxins::express("comp_ellint_2(0)",       cfg, ctx) == apx( 1.57079f));
    CHECK(fluxins::express("comp_ellint_3(0,0.5)",   cfg, ctx) == apx( 2.22144f));
    CHECK(fluxins::express("cos(0)",                 cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("cosh(0)",                cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("cyl_bessel_i(0,1)",      cfg, ctx) == apx( 1.26606f));
    CHECK(fluxins::express("cyl_bessel_j(0,1)",      cfg, ctx) == apx( 0.76519f));
    CHECK(fluxins::express("cyl_bessel_k(0,1)",      cfg, ctx) == apx( 0.42102f));
    CHECK(fluxins::express("cyl_neumann(0,1)",       cfg, ctx) == apx( 0.08825f));
    CHECK(fluxins::express("dim(5,3)",               cfg, ctx) == apx( 2.0f));
    CHECK(fluxins::express("ellint_1(0,0)",          cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("ellint_2(0,0)",          cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("ellint_3(0,0,1)",        cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("erf(0)",                 cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("erfc(0)",                cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("exp(1)",                 cfg, ctx) == apx( 2.71828f));
    CHECK(fluxins::express("exp2(3)",                cfg, ctx) == apx( 8.0f));
    CHECK(fluxins::express("expint(1)",              cfg, ctx) == apx( 1.89512f));
    CHECK(fluxins::express("expm1(1)",               cfg, ctx) == apx( 1.71828f));
    CHECK(fluxins::express("fegetround(0,0)",        cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("fesetround(0)",          cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("fma(2,3,4)",             cfg, ctx) == apx(10.0f));
    CHECK(fluxins::express("floor(2.9)",             cfg, ctx) == apx( 2.0f));
    CHECK(fluxins::express("gcd(54,24)",             cfg, ctx) == apx( 6.0f));
    CHECK(fluxins::express("hermite(3,2.0)",         cfg, ctx) == apx(40.0f));
    CHECK(fluxins::express("hypot(3,4)",             cfg, ctx) == apx( 5.0f));
    CHECK(fluxins::express("laguerre(0,5)",          cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("legendre(0,5)",          cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("lgamma(1)",              cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("lcm(21,6)",              cfg, ctx) == apx(42.0f));
    CHECK(fluxins::express("lerp(0,10,0.5)",         cfg, ctx) == apx( 5.0f));
    CHECK(fluxins::express("log(1)",                 cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("log1p(1)",               cfg, ctx) == apx( 0.69314f));
    CHECK(fluxins::express("log10(100)",             cfg, ctx) == apx( 2.0f));
    CHECK(fluxins::express("log2(8)",                cfg, ctx) == apx( 3.0f));
    CHECK(fluxins::express("max(1,5,3)",             cfg, ctx) == apx( 5.0f));
    CHECK(fluxins::express("midpoint(2,4)",          cfg, ctx) == apx( 3.0f));
    CHECK(fluxins::express("min(1,5,3)",             cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("mod(7,3)",               cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("nearbyint(2.3)",         cfg, ctx) == apx( 2.0f));
    CHECK(fluxins::express("pow(2,3)",               cfg, ctx) == apx( 8.0f));
    CHECK(fluxins::express("remainder(7,3)",         cfg, ctx) == apx( 1.0f));
    CHECK(fluxins::express("riemann_zeta(0)",        cfg, ctx) == apx(-0.5f));
    CHECK(fluxins::express("rint(2.3)",              cfg, ctx) == apx( 2.0f));
    CHECK(fluxins::express("round(2.5)",             cfg, ctx) == apx( 3.0f));
    CHECK(fluxins::express("sin(0)",                 cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("sinh(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("sph_bessel(0,1)",        cfg, ctx) == apx( 0.84147f));
    CHECK(fluxins::express("sph_legendre(0,0,2)",    cfg, ctx) == apx( 0.28209f));
    CHECK(fluxins::express("sph_neumann(0,1)",       cfg, ctx) == apx(-0.54030f));
    CHECK(fluxins::express("sqrt(4)",                cfg, ctx) == apx( 2.0f));
    CHECK(fluxins::express("tan(0)",                 cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("tanh(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("tgamma(5)",              cfg, ctx) == apx(24.0f));
    CHECK(fluxins::express("trunc(2.7)",             cfg, ctx) == apx( 2.0f));

    CHECK(fluxins::express("avg(1,2,3)",             cfg, ctx) == apx( 2.0f));
    // CHECK(fluxins::express("rand()",                 cfg, ctx) == apx( 0.0f)); // Cannot test random number generator
    CHECK(fluxins::express("srand(1)",               cfg, ctx) == apx( 0.0f));
    CHECK(fluxins::express("time()",                 cfg, ctx) == apx( (float)std::time(nullptr)));
    // clang-format on
}

TEST_CASE("Invalid arity exception tests")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();
    ctx->populate();

    // clang-format off
    CHECK_THROWS_AS(fluxins::express("abs()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("acos()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("acosh()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("assoc_laguerre()", cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("assoc_legendre()", cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("asin()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("asinh()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("atan()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("atan2()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("atanh()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("beta()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("ceil()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cbrt()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("clamp()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("comp_ellint_1()",  cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("comp_ellint_2()",  cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("comp_ellint_3()",  cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cos()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cosh()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cyl_bessel_i()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cyl_bessel_j()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cyl_bessel_k()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("cyl_neumann()",    cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("dim()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("ellint_1()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("ellint_2()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("ellint_3()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("erf()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("erfc()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("exp()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("exp2()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("expint()",         cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("expm1()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("fegetround()",     cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("fesetround()",     cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("fma()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("floor()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("gcd()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("hermite()",        cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("hypot()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("laguerre()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("legendre()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("lgamma()",         cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("lcm()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("lerp()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("log()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("log1p()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("log10()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("log2()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("max()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("midpoint()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("min()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("mod()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("nearbyint()",      cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("pow()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("remainder()",      cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("riemann_zeta()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("rint()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("round()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("sin()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("sinh()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("sph_bessel()",     cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("sph_legendre()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("sph_neumann()",    cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("sqrt()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("tan()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("tanh()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("tgamma()",         cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("trunc()",          cfg, ctx), fluxins::invalid_arity);

    CHECK_THROWS_AS(fluxins::express("avg()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("rand(1)",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("srand()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(fluxins::express("time(1)",          cfg, ctx), fluxins::invalid_arity);
    // clang-format on
}
