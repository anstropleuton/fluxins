/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests all built-in symbols from a populated context.
///
/// This project is licensed under the terms of MIT License.

#include "test.hpp"

//

#include <cfenv>
#include <memory>
#include <numbers>

#include "fluxins/fluxins.hpp"

TEST_CASE("Built-in symbols")
{
    auto cfg = std::make_shared<fluxins::config>();

    auto ctx = std::make_shared<fluxins::context>();
    ctx->populate();

    // clang-format off
    CHECK(express("e",                      cfg, ctx) == apx(std::numbers::e));
    CHECK(express("log2e",                  cfg, ctx) == apx(std::numbers::log2e));
    CHECK(express("log10e",                 cfg, ctx) == apx(std::numbers::log10e));
    CHECK(express("pi",                     cfg, ctx) == apx(std::numbers::pi));
    CHECK(express("inv_pi",                 cfg, ctx) == apx(std::numbers::inv_pi));
    CHECK(express("inv_sqrtpi",             cfg, ctx) == apx(std::numbers::inv_sqrtpi));
    CHECK(express("ln2",                    cfg, ctx) == apx(std::numbers::ln2));
    CHECK(express("ln10",                   cfg, ctx) == apx(std::numbers::ln10));
    CHECK(express("sqrt2",                  cfg, ctx) == apx(std::numbers::sqrt2));
    CHECK(express("sqrt3",                  cfg, ctx) == apx(std::numbers::sqrt3));
    CHECK(express("inv_sqrt3",              cfg, ctx) == apx(std::numbers::inv_sqrt3));
    CHECK(express("egamma",                 cfg, ctx) == apx(std::numbers::egamma));
    CHECK(express("phi",                    cfg, ctx) == apx(std::numbers::phi));
    CHECK(express("FE_DOWNWARD",            cfg, ctx) == apx(FE_DOWNWARD));
    CHECK(express("FE_TONEAREST",           cfg, ctx) == apx(FE_TONEAREST));
    CHECK(express("FE_TOWARDZERO",          cfg, ctx) == apx(FE_TOWARDZERO));
    CHECK(express("FE_UPWARD",              cfg, ctx) == apx(FE_UPWARD));

    CHECK(express("abs(-5)",                cfg, ctx) == apx( 5.0f));
    CHECK(express("acos(1)",                cfg, ctx) == apx( 0.0f));
    CHECK(express("acosh(1)",               cfg, ctx) == apx( 0.0f));
    CHECK(express("assoc_laguerre(3,0,0)",  cfg, ctx) == apx( 1.0f));
    CHECK(express("assoc_legendre(0,0,2)",  cfg, ctx) == apx( 1.0f));
    CHECK(express("asin(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(express("asinh(0)",               cfg, ctx) == apx( 0.0f));
    CHECK(express("atan(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(express("atan2(0,1)",             cfg, ctx) == apx( 0.0f));
    CHECK(express("atanh(0)",               cfg, ctx) == apx( 0.0f));
    CHECK(express("beta(1,1)",              cfg, ctx) == apx( 1.0f));
    CHECK(express("ceil(2.3)",              cfg, ctx) == apx( 3.0f));
    CHECK(express("cbrt(27)",               cfg, ctx) == apx( 3.0f));
    CHECK(express("clamp(20,5,10)",         cfg, ctx) == apx( 10.0f));
    CHECK(express("comp_ellint_1(0)",       cfg, ctx) == apx( 1.57079f));
    CHECK(express("comp_ellint_2(0)",       cfg, ctx) == apx( 1.57079f));
    CHECK(express("comp_ellint_3(0,0.5)",   cfg, ctx) == apx( 2.22144f));
    CHECK(express("cos(0)",                 cfg, ctx) == apx( 1.0f));
    CHECK(express("cosh(0)",                cfg, ctx) == apx( 1.0f));
    CHECK(express("cyl_bessel_i(0,1)",      cfg, ctx) == apx( 1.26606f));
    CHECK(express("cyl_bessel_j(0,1)",      cfg, ctx) == apx( 0.76519f));
    CHECK(express("cyl_bessel_k(0,1)",      cfg, ctx) == apx( 0.42102f));
    CHECK(express("cyl_neumann(0,1)",       cfg, ctx) == apx( 0.08825f));
    CHECK(express("dim(5,3)",               cfg, ctx) == apx( 2.0f));
    CHECK(express("ellint_1(0,0)",          cfg, ctx) == apx( 0.0f));
    CHECK(express("ellint_2(0,0)",          cfg, ctx) == apx( 0.0f));
    CHECK(express("ellint_3(0,0,1)",        cfg, ctx) == apx( 1.0f));
    CHECK(express("erf(0)",                 cfg, ctx) == apx( 0.0f));
    CHECK(express("erfc(0)",                cfg, ctx) == apx( 1.0f));
    CHECK(express("exp(1)",                 cfg, ctx) == apx( 2.71828f));
    CHECK(express("exp2(3)",                cfg, ctx) == apx( 8.0f));
    CHECK(express("expint(1)",              cfg, ctx) == apx( 1.89512f));
    CHECK(express("expm1(1)",               cfg, ctx) == apx( 1.71828f));
    CHECK(express("fegetround(0,0)",        cfg, ctx) == apx( 0.0f));
    CHECK(express("fesetround(0)",          cfg, ctx) == apx( 0.0f));
    CHECK(express("fma(2,3,4)",             cfg, ctx) == apx(10.0f));
    CHECK(express("floor(2.9)",             cfg, ctx) == apx( 2.0f));
    CHECK(express("gcd(54,24)",             cfg, ctx) == apx( 6.0f));
    CHECK(express("hermite(3,2.0)",         cfg, ctx) == apx(40.0f));
    CHECK(express("hypot(3,4)",             cfg, ctx) == apx( 5.0f));
    CHECK(express("laguerre(0,5)",          cfg, ctx) == apx( 1.0f));
    CHECK(express("legendre(0,5)",          cfg, ctx) == apx( 1.0f));
    CHECK(express("lgamma(1)",              cfg, ctx) == apx( 0.0f));
    CHECK(express("lcm(21,6)",              cfg, ctx) == apx(42.0f));
    CHECK(express("lerp(0,10,0.5)",         cfg, ctx) == apx( 5.0f));
    CHECK(express("log(1)",                 cfg, ctx) == apx( 0.0f));
    CHECK(express("log1p(1)",               cfg, ctx) == apx( 0.69314f));
    CHECK(express("log10(100)",             cfg, ctx) == apx( 2.0f));
    CHECK(express("log2(8)",                cfg, ctx) == apx( 3.0f));
    CHECK(express("max(1,5,3)",             cfg, ctx) == apx( 5.0f));
    CHECK(express("midpoint(2,4)",          cfg, ctx) == apx( 3.0f));
    CHECK(express("min(1,5,3)",             cfg, ctx) == apx( 1.0f));
    CHECK(express("mod(7,3)",               cfg, ctx) == apx( 1.0f));
    CHECK(express("nearbyint(2.3)",         cfg, ctx) == apx( 2.0f));
    CHECK(express("pow(2,3)",               cfg, ctx) == apx( 8.0f));
    CHECK(express("remainder(7,3)",         cfg, ctx) == apx( 1.0f));
    CHECK(express("riemann_zeta(0)",        cfg, ctx) == apx(-0.5f));
    CHECK(express("rint(2.3)",              cfg, ctx) == apx( 2.0f));
    CHECK(express("round(2.5)",             cfg, ctx) == apx( 3.0f));
    CHECK(express("sin(0)",                 cfg, ctx) == apx( 0.0f));
    CHECK(express("sinh(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(express("sph_bessel(0,1)",        cfg, ctx) == apx( 0.84147f));
    CHECK(express("sph_legendre(0,0,2)",    cfg, ctx) == apx( 0.28209f));
    CHECK(express("sph_neumann(0,1)",       cfg, ctx) == apx(-0.54030f));
    CHECK(express("sqrt(4)",                cfg, ctx) == apx( 2.0f));
    CHECK(express("tan(0)",                 cfg, ctx) == apx( 0.0f));
    CHECK(express("tanh(0)",                cfg, ctx) == apx( 0.0f));
    CHECK(express("tgamma(5)",              cfg, ctx) == apx(24.0f));
    CHECK(express("trunc(2.7)",             cfg, ctx) == apx( 2.0f));

    CHECK(express("avg(1,2,3)",             cfg, ctx) == apx( 2.0f));
    // CHECK(express("rand()",                 cfg, ctx) == apx( 0.0f)); // Cannot test random number generator
    CHECK(express("srand(1)",               cfg, ctx) == apx( 0.0f));
    CHECK(express("time()",                 cfg, ctx) == apx( (float)std::time(nullptr)));
    // clang-format on
}

TEST_CASE("Invalid arity exception tests")
{
    auto cfg = std::make_shared<fluxins::config>();
    auto ctx = std::make_shared<fluxins::context>();
    ctx->populate();

    // clang-format off
    CHECK_THROWS_AS(express("abs()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("acos()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("acosh()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("assoc_laguerre()", cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("assoc_legendre()", cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("asin()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("asinh()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("atan()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("atan2()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("atanh()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("beta()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("ceil()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cbrt()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("clamp()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("comp_ellint_1()",  cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("comp_ellint_2()",  cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("comp_ellint_3()",  cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cos()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cosh()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cyl_bessel_i()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cyl_bessel_j()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cyl_bessel_k()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("cyl_neumann()",    cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("dim()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("ellint_1()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("ellint_2()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("ellint_3()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("erf()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("erfc()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("exp()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("exp2()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("expint()",         cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("expm1()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("fegetround()",     cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("fesetround()",     cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("fma()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("floor()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("gcd()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("hermite()",        cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("hypot()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("laguerre()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("legendre()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("lgamma()",         cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("lcm()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("lerp()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("log()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("log1p()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("log10()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("log2()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("max()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("midpoint()",       cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("min()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("mod()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("nearbyint()",      cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("pow()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("remainder()",      cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("riemann_zeta()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("rint()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("round()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("sin()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("sinh()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("sph_bessel()",     cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("sph_legendre()",   cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("sph_neumann()",    cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("sqrt()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("tan()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("tanh()",           cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("tgamma()",         cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("trunc()",          cfg, ctx), fluxins::invalid_arity);

    CHECK_THROWS_AS(express("avg()",            cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("rand(1)",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("srand()",          cfg, ctx), fluxins::invalid_arity);
    CHECK_THROWS_AS(express("time(1)",          cfg, ctx), fluxins::invalid_arity);
    // clang-format on
}
