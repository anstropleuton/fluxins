# Fluxins Expression Parser/Evaluator
[![C++](https://img.shields.io/badge/C++-23-blue.svg)](https://en.cppreference.com/w/cpp)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](license.md)
![Build](https://github.com/anstropleuton/fluxins/actions/workflows/ci.yml/badge.svg)

**Fluxins** is a lightweight expression parser and evaluator written in modern C++. It has **no dependencies\*, simple integration, and is highly customizable**.

Fluxins provides a flexible way to **parse and evaluate expressions** from strings. It is designed to be easily integrated into multiple systems, including user-facing applications, which allows them to input an expression that can be automatically evaluated. This allows users to perform quick operation directly a text field, without having to pull up a calculator.

Fluxins can be useful in scenarios where **flexible value evaluation** is needed, but a simple lambda won't cut it. A prime example for this is in a GUI framework, where programmer can enter an expression that queries related data (e.g., parent container's width) within the expression, and apply transformations (like calculating a child container's max width).

The **integration possibilities are endless**, basically. You can also customize it to include your own operators too! You can create a REPL-like expression parsing prompt, or build another calculator, because you feel like that's what the world needs right now.

\*: it does have a dependency for testing, but it is not enabled by default when Fluxins is added as a subdirectory.

# Prerequisite

- A C++ compiler that supports C++23 (GCC 12+, Clang 15+, MSVC 19.30+)
- CMake 3.15 or higher
- [doctest](https://github.com/doctest/doctest) (for testing)

# Building

- Clone the repository
```bash
git clone https://github.com/anstropleuton/fluxins.git
```
- Create a build directory
```bash
mkdir build
cd build
```
- Build the project
```bash
cmake ..
cmake --build . --config Release
```
- Install the project (optional; UNIX-like systems)
```bash
sudo cmake --install . --config Release
```

# Quick-Start Example

Add your project as a subdirectory in your CMakeLists.txt:
```cmake
add_subdirectory(fluxins)
```

If your project does not use CMake for your project, you can use CMake to build Fluxins as a static library, set up include paths, and link the library to your project.

```cpp
#include "fluxins/fluxins.hpp"

int main()
{
    // This is just a quick-start example. Also see the other examples from the
    // [example](example) directory to learn more.

    // Create a configuration for parser and evaluator
    auto cfg = std::make_shared<fluxins::config>();

    // Create an expression
    fluxins::expression expr("a + 2", cfg);

    // Add variable to the expression's context
    expr.set_variable("a", 3);

    try
    {
        // Parse and evaluate the expression
        expr.parse();
        expr.evaluate();
    }
    catch (const fluxins::code_error &e)
    {
        // Report error
        std::println("Error when parsing expression: {}", e.what());
        return 1;
    }

    // Get the result
    float result = expr;
    std::println("Result of {}: {}", (std::string)expr, result); // 5
}
```

Or check out this micro-example if you want ultra-quick expression parsing.

```cpp
#include "fluxins/fluxins.hpp"

int main()
{
    float value = fluxins::express("1 + 1"); // Since v1.0.1
    std::println("Result of 1 + 1: {}", value);
}
```

For other usage examples, including customizing functions or operators, see [example](example) directory.

# Features

Expression features:
- Numbers (duh)
  - Both integers and floating-point numbers are interpreted as float type.
  - Separate digits using ```'``` or ```_``` character.
  - Don't use the 'f' suffix for floats, as you would do in C++.
  - Conversion to integral value in specific contexts is done by flooring, not truncating.
- Parenthesis: `(` and `)`
- Basic operators: `+`, `-`, `*`, `/`, `%` (modulo/remainder), `%%` (wrapping modulo), `**` (exponent/power), `//` (flooring division)
- Relational operators: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Boolean conditional operators: `&&`, `||`, `!`
- Integral bitwise operators: `&`, `|`, `^`, `~`, `<<`, `>>`
- Absolute difference operator: `!!`
- Min and Max operator: `<?` and `>?`.
- Zero-coalescing operator: `value ?? value_if_zero`
- Conditional operator: `condition ? true_value : false_value`
- Variables: `var` \*
- Functions: `fn(args)`

\*: Technically, an expression cannot modify a variable, and the term "constant" may have made more sense. But the term "variable" is chosen due to the common use of identifiers as a variable in the code.

Customizations:
- Custom variables: Append your custom variable in list of variables.
- Custom functions: Append your custom function in list of functions.
- Multiple contexts for storing global symbols.
- Inherit symbols from other contexts.
- Custom operators: Append your custom operator in the parser config.

Other features:
- **Thread Safety**: Fluxins is thread safe, as long as you do not mutate configurations or contexts from multiple threads at the same time. Thread safety is on your hand.
- **Error Reporting**: Parsing and evaluating can throw `code_error` exception which contains information about the error, along with location of the error within the expression, such as syntax error or missing function.
- **Caching**: Expression's AST and evaluated value is cached. If you change the expression, you would need to re-parse and re-evaluate the expression. If you change the context (symbols), you would only need to re-evaluat the expression.
- **Built-in Variables and Functions**: There are several built-in variables and functions that expressions can access. **Variables** include `e`, `pi`, `phi`, `sqrt2`, `inv_sqrt3`, `inv_pi`, etc. while **Functions** include `abs(x)`, `sin(x)`, `pow(x, y)`, `min(...)`, `clamp(x,min,max)`, `avg(...)`, etc.

# Anti-features

- A programming language: **It's not**.
- Variables or function calls outside the ones defined: **It cannot**.
- Automatic vulnerability checks to prevent unauthorized defined function call: **It does not have it**.

## Anti-features cheese (skip this section if you are uninterested)

You can technically turn this into a programming language if you add basic control structure as function, and function that only return 0.
```
print_hello() + print_number(if(value) + equals(0) + then(10) + else(-10))
```
Here,
- `print_hello()`, `print_number()`, `if()` and `equals()` always returns 0.
- `if()`, `then()` and `else()` act as basic control structure.
- `then()` and `else()` return if condition matched, otherwise 0.
Note: none of these function are part of the fluxins.

# Future plans/considerations

- Comments support in the expression.

# Local vs. Global

Fluxins uses context to obtain list of variables or functions (collectively: symbols). This allows flexibility of providing symbols only to specific expressions or allowing them to inherit from other contexts. This allows global sharing of symbols.

# Parsing and Grammar

The expression consists of tokens, i.e., identifiers, numbers, operators and punctuations. The whitespace is ignored by the parser, so feel free to use it to make your expression readable.

The grammar is similar to that of most programming language. Operators have precedence and parentheses override operator precedence and are evaluated first. They can be also nested.

# List of all default operators

Unary prefix operators:
- `+` - Unary addition operator, one operand (at the right), has no effect (same as 0 + x).
- `-` - Unary subtraction operator, one operand (at the right), negates the value (same as 0 - x).
- `*` - Unary multiplication operator, one operand (at the right), has no effect (same as 1 * x).
- `/` - Unary division operator, one operand (at the right), inverts the value (same as 1 / x).
- `!` - NOT operator, one operand (at the right).
- `~` - Bitwise NOT operator, one operand (at the right).

Unary suffix operators:
- `!` - Factorial operator, one operand (at the left, not to be confused with `!` NOT operator which requires operand at the right).

Binary operators:
- `+` - Addition operator.
- `-` - Subtraction operator.
- `*` - Multiplication operator.
- `/` - Division operator.
- `%` - Modulo operator.
- `%%` - Wrapping modulo operator. `-2 %% 5` yields 3 (Wraps negative remainder into [0,5) range).
- `**` - Exponent operator.
- `//` - Flooring division operator. `7 // 2` yields 3 (3.5 floored down).
- `==` - Equality comparison. `2 == 2` yields 1 (true is 1 and false is 0).
- `!=` - Opposite of equality comparison.
- `<` - Less-than operator.
- `>` - More-than operator (greater-than operator).
- `<=` - Less-than or equal to operator.
- `>=` - More-than or equal to operator.
- `&&` - AND operator. Considers operands to be true if they are non-zero.
- `||` - OR operator.
- `&` - Bitwise AND operator.
- `|` - Bitwise OR operator.
- `^` - Bitwise XOR operator.
- `<<` - Bitwise left-shift operator.
- `>>` - Bitwise right-shift operator.
- `!!` - Absolute difference operator. `3 !! 5` yields 2 (same as `abs(3 - 5)`).
- `??` - Zero-coalescing operator. `4 ?? 5` yields 4 and `0 ?? 5` yields 5 (if the first operand is non-zero, it is yielded, otherwise the second operand is yielded)
- `<?` - Min operator.
- `>?` - Max operator.
- `?` and `:` - Conditional operator. `1 ? 2 : 3` yields 2 and `0 ? 2 : 3` yields 3 (syntax is `condition ? true_value : false_value`).

# Default binary operator precedence table (highest to lowest)

| Precedence | Operator                         |
|-----------:|:---------------------------------|
|          0 | `<<`, `>>`                       |
|          1 | `^`                              |
|          2 | `&`, `\|`                        |
|          3 | `!!`                             |
|          4 | `<?`, `>?`                       |
|          5 | `??`                             |
|          6 | `**`                             |
|          7 | `//`                             |
|          8 | `%`, `%%`                        |
|          9 | `*`, `/`                         |
|         10 | `+`, `-`                         |
|         11 | `==`, `!=`, `<`, `>`, `<=`, `>=` |
|         12 | `&&`, `\|\|`                     |

# List of all the variables and functions

There are **17 variables** and **70 functions** defined as of release 1.0.0. For the full list of all the variables and functions, see [builtins.cpp](source/builtins.cpp).

More functions are to be added when C++26 is released and compilers support are ready (namely [saturated arithmetic](https://en.cppreference.com/w/cpp/numeric#Saturation_arithmetic) such as `add_sat`, `sub_sat`).

# More examples

There are a few examples that I have created to demonstrate Fluxins.
- [usage.cpp](example/usage.cpp): Demonstrates **general usage** of Fluxins. This is a larger example and covers wider areas compared to the [Quick-Start Example](#quick-start-example).
- [customize.cpp](example/customize.cpp): Showcases customizability of Fluxins by creating custom variables, functions and several operators (prefix, suffix, binary).
- [repl.cpp](example/repl.cpp): Implements a REPL console that allows users to parse and evaluate expressions. It even supports expression evaluation from the command-line arguments, and more features for ease of demo.

# Contributing

Feel free to contribute to this project by code (see [contributing.md](contributing.md)), by suggesting your ideas, or even by reporting a bugfix.

# Credits

Thanks to [jothepro](https://github.com/jothepro) for the stylesheet [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css) for Doxygen.

Thanks to Nuno Pinheiro (can't find link) for the background Elarun. But I did find a [KDE store link](https://store.kde.org/p/1162360/) to the background image. I think you could consider that. If KDE developers are here, I would appreciate your help to find a proper link to the author.

Thanks to [patorjk](https://www.patorjk.com) for [Text to ASCII Art Generator](https://www.patorjk.com/software/taag).

# License

This project is licensed under the terms of MIT License. See [license.md](license.md) for more info.
