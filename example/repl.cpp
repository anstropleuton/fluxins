/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This example file implements a Read-Evaluate-Print Loop (REPL) console with
/// custom features outside Fluxins.
///
/// # Features
/// - Evaluate expression:
///   ```
///   > 2 * 2
///   4
///   ```
/// - Expressions from command-line arguments:
///   ```bash
///   $ ./fluxins_repl_example "1 + 1"
///   2
///   ```
///
/// # Expression features (outside Fluxins)
/// - Evaluate multi-line expression:
///   ```
///   > 2 \
///   . + \
///   . 4
///   6
///   ```
/// - Multiple expressions in a single line:
///   ```
///   > 1 - 5; 3 - 4
///   -4; -1
///   ```
/// - Create custom variables: `> var variable = expression`
/// - Create custom functions: `> fn function(params) = expression`
/// - Create custom operators:
///   - `> unary_prefix_op symbol(x) = expression`: Unary prefix operator
///   - `> unary_suffix_op symbol(x) = expression`: Unary suffix operator
///   - `> binary_op_left symbol(x, y) = expression`: Binary operator with left associativity
///   - `> binary_op_right symbol(x, y) = expression`: Binary operator with right associativity
/// - Display information about symbols: `> info symbol` (variable, function, operator)
/// - List all symbols: `> list` (or `> list variables` for only variables, etc.)
/// - Undefine symbols: `> undef symbol` (you can pass multiple symbols)
/// - Print the tokens of previous expression: `> tokens`
/// - Print the AST of previous expression: `> ast`
/// - The command history is presestant, as they are stored in a file `repl_history.txt`.
/// - Print help message: `> help` (or `> help list` for manual for `list` command)
/// - Exit: `> exit`
///
/// # Limitations:
/// - All the symbols are stored in a single context. Function parameters are
///   stored in their own context that inherits the global context.
/// - Identifier names cannot be a command.
/// - Expression starting with '--' are treated as command line argument when
///   passed from command line.
/// - Binary operator's precedence table cannot be modified. New custom
///   operators are added to the end of the precedence table. Though, overriding
///   existing operators is possible.
/// - All the tokens are limited to the list of valid token types defined by
///   Fluxins. Implies no emoji as an operator :'(
/// - Strings are not supported by Fluxins' tokenizer. Load and save commands
///   only accept identifier and treat them as file with `.txt` extension.
/// - This is not a scripting language or a shell.
///
/// # Planned features:
/// - Load a file for expression: `> load filename` (loads from filename.txt)
/// - Save custom symbols to a file: `> save filename` (saves to filename.txt)
/// - Retrieve history of expressions by pressing up and down arrow.
/// - Retrieve the last evaluated value by the variable `_`.
/// - Customizable Syntax Highlighting in console mode.
///   Use --no-colorization to disable syntax highlighting and output
///   colorization. Customize syntax highlighting colors from `repl_config.txt`.
/// - Automatic evaluation as you type, kind of like a web browser's console.
/// - Advanced Text Editor. \*
///
/// \*: Currently, text editing is basic, and supports:
/// - Inserting text (duh) (only valid characters)
/// - Backspace to delete character before the cursor
/// - Delete to delete character after the cursor
/// - Multi-line input support (still very basic, cannot edit previous lines)
/// - Arrow keys to move the cursor (cannot move up and down in multi line)
/// - Home to move the cursor to the beginning of the line
/// - End to move the cursor to the end of the line
///
/// Implementing more features is on the roadmap:
/// - Proper multi-line input support
/// - Proper up and down arrow key support
/// - Ctrl + Arrow keys to move the cursor by words
/// - Ctrl + Backspace to delete the word before the cursor
/// - Ctrl + Delete to delete the word after the cursor
/// - Shift + Arrow keys to select text
/// - Ctrl + Shift + Arrow keys to select text by words
/// - Ctrl + A to select all text
/// - Ctrl + C to copy selected text
/// - Ctrl + V to paste text
/// - Ctrl + X to cut selected text
/// - Ctrl + Z to undo
/// - Ctrl + Shift + Z to redo
/// - Ctrl + Home to move the cursor to the beginning of the input
/// - Ctrl + End to move the cursor to the end of the input
/// - Shift + Alt + Arrow keys to edit using multiple cursors (multi line edit)
///
/// And a whole can of worms of editing using mouse cursor as input.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "fluxins/code.hpp"
#include "fluxins/config.hpp"
#include "fluxins/context.hpp"
#include "fluxins/error.hpp"
#include "fluxins/parser.hpp"

using namespace fluxins;

// clang-format off

// ===
// ANSI escape codes

#define RESET           "\x1b[0m"
#define BOLD            "\x1b[1m"
#define UNDERLINE       "\x1b[4m"

#define BLACK           "\x1b[30m"
#define RED             "\x1b[31m"
#define GREEN           "\x1b[32m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define MAGENTA         "\x1b[35m"
#define CYAN            "\x1b[36m"
#define WHITE           "\x1b[37m"

#define BRIGHT_BLACK    "\x1b[90m"
#define BRIGHT_RED      "\x1b[91m"
#define BRIGHT_GREEN    "\x1b[92m"
#define BRIGHT_YELLOW   "\x1b[93m"
#define BRIGHT_BLUE     "\x1b[94m"
#define BRIGHT_MAGENTA  "\x1b[95m"
#define BRIGHT_CYAN     "\x1b[96m"
#define BRIGHT_WHITE    "\x1b[97m"

// ===
// Configure the REPL

#define CONFIG_FILE             "repl_config.txt"
#define COMMAND_HISTORY_FILE    "repl_history.txt"
#define COMMAND_HISTORY_DELIM   ";;\n"

// ===
// Helper macros

#define COMMAND_PARAMS std::string_view expr, const std::vector<token> &args
#define REPL_FN_PARAMS const repl_function &this_fn, std::string_view expr, code_location location, const std::vector<float> &values
#define CTRL_KEY(x)    ((x) & 0x1f)

// clang-format on

// ===
// Global state

// Global configuration and context for all the symbols in the repl
auto global_config  = std::make_shared<config>();
auto global_context = std::make_shared<context>();

bool colorization = true; // Enable syntax highlighting by default

// ===
// Utils

/// Evaluate an expression from tokens.
/// @note It takes expression code only for displaying errors. It does not
///       process the expression code, and only processes the given tokens.
/// @note We are not using the new `fluxins::express` feature added in v1.0.1
///       because we are working with tokens, not a string inherently.
float eval(const code &expr, const std::vector<token> &tokens, std::shared_ptr<context> ctx = std::make_shared<context>())
{
    // Manually parse and evaluate tokens using Fluxins' parser and evaluator.
    // See `interpret()` to see how we got tokens.
    ctx->inherit_context(global_context);
    auto ast = parse(expr, tokens, global_config);
    return ast->evaluate(expr, global_config, ctx);
}

/// Formats text by replacing the shorthand ANSI escape codes with their
/// corresponding ANSI escape codes.
///
/// Shorthand AEC is a prefixed shorthand codes, prefixed with with `$`, which
/// allows for a concise way to represent the predefined simple AECs, such as
/// `$r` for Red color or `$*` to bold the text. Use `$$` to escape a dollar
/// sign.
///
/// @note Invalid shorthand AEC or trailing dollar sign will be ignored, and
///       will not be returned as is.
std::string pre_print_format(std::string_view str)
{
    std::unordered_map<char, std::string> shorthand_aecs = {
        { '0', RESET          },
        { '*', BOLD           },
        { '_', UNDERLINE      },
        { 'k', BLACK          },
        { 'r', RED            },
        { 'g', GREEN          },
        { 'y', YELLOW         },
        { 'b', BLUE           },
        { 'm', MAGENTA        },
        { 'c', CYAN           },
        { 'w', WHITE          },
        { 'K', BRIGHT_BLACK   },
        { 'R', BRIGHT_RED     },
        { 'G', BRIGHT_GREEN   },
        { 'Y', BRIGHT_YELLOW  },
        { 'B', BRIGHT_BLUE    },
        { 'M', BRIGHT_MAGENTA },
        { 'C', BRIGHT_CYAN    },
        { 'W', BRIGHT_WHITE   },
        { '$', "$$"           },
    };

    std::string result;
    for (size_t i = 0; i < str.size(); i++)
    {
        // Regular character
        if (str[i] != '$')
        {
            result += str[i];
            continue;
        }

        // Trailing dollar sign, also ignored.
        if (i + 1 >= str.size())
        {
            continue;
        }

        if (auto it = shorthand_aecs.find(str[i + 1]); it != shorthand_aecs.end())
        {
            if (colorization)
            {
                result += it->second;
            }
        }

        i++;
    }
    return result;
}

/// Sanitize user input to escape dollar sign to allow proper printing, without
/// causing user input to accidentally be treated as shorthand AEC.

/// Error with formatted message.
///
/// This exception class is solely used to distinguish with exceptions without
/// AEC formatting. It does not actually format the message directly. They are
/// formatted during the real printing.
struct formatted_error : code_error {
    formatted_error(std::string_view message, const code &expr, code_location location) : code_error(message, expr, location)
    {}
};

/// Helper to parse the command arguments.
struct token_stream {
    code               expr;    ///< The expression being evaluated.
    std::vector<token> tokens;  ///< The tokens of the expression.
    std::size_t        pos = 0; ///< Current position in the tokens.

    token_stream(const code &expr, const std::vector<token> &tokens) : expr(expr), tokens(tokens)
    {}

    /// Helper to throw an error with the current token position.
    void throw_err(std::string_view message, int token_offset = 0) const
    {
        throw formatted_error(message, expr, tokens.size() != 0 && pos + token_offset < tokens.size() ? tokens[pos + token_offset].location : code_location{});
    }

    /// Check if the end of the token stream is reached.
    bool is_eof() const
    {
        return pos >= tokens.size();
    }

    /// Expect the end of the token stream.
    void expect_eof() const
    {
        if (!is_eof())
        {
            throw_err("$rExpected end of expression$0");
        }
    }

    /// Expect not the end of the token stream.
    void expect_not_eof() const
    {
        if (is_eof())
        {
            throw_err("$rUnexpected end of expression$0", -1);
        }
    }

    /// Get the current token.
    token peek() const
    {
        expect_not_eof();
        return tokens[pos];
    }

    /// Get the current token and consume it.
    token next()
    {
        expect_not_eof();
        return tokens[pos++];
    }

    std::vector<token> slice_rem() const
    {
        return std::vector<token>(tokens.begin() + pos, tokens.end());
    }

    /// Check if the current token is of the given type.
    bool is_type(token::token_type type) const
    {
        if (is_eof())
        {
            return false;
        }
        return tokens[pos].type == type;
    }

    /// Check if the current token is of the given type and has the given value.
    bool is_type_val(token::token_type type, std::string_view value) const
    {
        if (is_eof())
        {
            return false;
        }
        return tokens[pos].type == type && tokens[pos].value == value;
    }

    /// Expect the current token to be of the given type and consume it.
    token expect_type(token::token_type type)
    {
        if (is_eof())
        {
            throw_err("$rUnexpected end of expression, expected $R$*" + token_type_to_string(type) + "$0", -1);
        }
        if (tokens[pos].type != type)
        {
            throw_err("$rExpected $R$*" + token_type_to_string(type) + "$0$r, got $R$*" + token_type_to_string(tokens[pos].type) + "$0");
        }
        return tokens[pos++];
    }

    /// Expect the current token to be of the given type and value, and consume it.
    token expect_type_val(token::token_type type, std::string_view value)
    {
        if (is_eof())
        {
            throw_err("$rUnexpected end of expression, expected $R$*" + token_type_to_string(type) + "$0$r of value $R$*" + std::string(value) + "$0", -1);
        }
        if (tokens[pos].type != type)
        {
            throw_err("$rExpected $R$*" + token_type_to_string(type) + "$0$r, got $R$*" + token_type_to_string(tokens[pos].type) + "$0");
        }
        if (tokens[pos].value != value)
        {
            throw_err("$rExpected $R$*" + std::string(value) + "$0$r, got $R$*" + std::string(tokens[pos].value) + "$0");
        }
        return tokens[pos++];
    }

    // Helpers for specific token types

    // clang-format off

    bool is_id() const { return is_type(token::token_type::identifier); }

    bool is_id(std::string_view id) const { return is_type_val(token::token_type::identifier, id); }

    token expect_id() { return expect_type(token::token_type::identifier); }

    token expect_id(std::string_view id) { return expect_type_val(token::token_type::identifier, id); }

    bool is_num() const { return is_type(token::token_type::number); }

    bool is_num(std::string_view num) const { return is_type_val(token::token_type::number, num); }

    token expect_num() { return expect_type(token::token_type::number); }

    token expect_num(std::string_view num) { return expect_type_val(token::token_type::number, num); }

    bool is_sym() const { return is_type(token::token_type::symbol); }

    bool is_sym(std::string_view sym) const { return is_type_val(token::token_type::symbol, sym); }

    token expect_sym() { return expect_type(token::token_type::symbol); }

    token expect_sym(std::string_view sym) { return expect_type_val(token::token_type::symbol, sym); }

    bool is_punc() const { return is_type(token::token_type::punctuation); }

    bool is_punc(std::string_view punc) const { return is_type_val(token::token_type::punctuation, punc); }

    token expect_punc() { return expect_type(token::token_type::punctuation); }

    token expect_punc(std::string_view punc) { return expect_type_val(token::token_type::punctuation, punc); }

    // clang-format on
};

// ===
// Command management

/// A REPL command. If an expression starts with a command, it is evaluated
/// based on the command's requirements.
struct command {
    /// Function to evaluate a command.
    std::function<std::string(COMMAND_PARAMS)> evaluate;

    /// Function to print help message of a command.
    std::function<std::string(COMMAND_PARAMS)> docs;
};

// FWD

// clang-format off
// Perhaps there can be a third column for other purposes?
std::string var_command_evaluator(COMMAND_PARAMS);              std::string var_command_docs(COMMAND_PARAMS);
std::string fn_command_evaluator(COMMAND_PARAMS);               std::string fn_command_docs(COMMAND_PARAMS);
std::string unary_prefix_op_command_evaluator(COMMAND_PARAMS);  std::string unary_prefix_op_command_docs(COMMAND_PARAMS);
std::string unary_suffix_op_command_evaluator(COMMAND_PARAMS);  std::string unary_suffix_op_command_docs(COMMAND_PARAMS);
std::string binary_op_left_command_evaluator(COMMAND_PARAMS);   std::string binary_op_left_command_docs(COMMAND_PARAMS);
std::string binary_op_right_command_evaluator(COMMAND_PARAMS);  std::string binary_op_right_command_docs(COMMAND_PARAMS);
std::string info_command_evaluator(COMMAND_PARAMS);             std::string info_command_docs(COMMAND_PARAMS);
std::string list_command_evaluator(COMMAND_PARAMS);             std::string list_command_docs(COMMAND_PARAMS);
std::string undef_command_evaluator(COMMAND_PARAMS);            std::string undef_command_docs(COMMAND_PARAMS);
std::string tokens_command_evaluator(COMMAND_PARAMS);           std::string tokens_command_docs(COMMAND_PARAMS);
std::string ast_command_evaluator(COMMAND_PARAMS);              std::string ast_command_docs(COMMAND_PARAMS);
std::string help_command_evaluator(COMMAND_PARAMS);             std::string help_command_docs(COMMAND_PARAMS);
std::string exit_command_evaluator(COMMAND_PARAMS);             std::string exit_command_docs(COMMAND_PARAMS);

/// List of all the commands.
/// Register your custom commands here.
std::unordered_map<std::string, command> commands = {
    { "var",             command{ var_command_evaluator,               var_command_docs }             },
    { "fn",              command{ fn_command_evaluator,                fn_command_docs }              },
    { "unary_prefix_op", command{ unary_prefix_op_command_evaluator,   unary_prefix_op_command_docs } },
    { "unary_suffix_op", command{ unary_suffix_op_command_evaluator,   unary_suffix_op_command_docs } },
    { "binary_op_left",  command{ binary_op_left_command_evaluator,    binary_op_left_command_docs }  },
    { "binary_op_right", command{ binary_op_right_command_evaluator,   binary_op_right_command_docs } },
    { "info",            command{ info_command_evaluator,              info_command_docs }            },
    { "list",            command{ list_command_evaluator,              list_command_docs }            },
    { "undef",           command{ undef_command_evaluator,             undef_command_docs }           },
    { "tokens",          command{ tokens_command_evaluator,            tokens_command_docs }          },
    { "ast",             command{ ast_command_evaluator,               ast_command_docs }             },
    { "help",            command{ help_command_evaluator,              help_command_docs }            },
    { "exit",            command{ exit_command_evaluator,              exit_command_docs }            },
};
// clang-format on

// ===
// History management

/// History of all the commands entered by the user.
std::vector<std::string> command_history;

/// Stores the previously executed command that contains tokens.
struct previous_command {
    /// The expression of the previous command.
    code expr;

    /// The tokens of the previous command.
    std::vector<token> tokens;
};

/// Previous command and its tokens.
previous_command previous_cmd;

/// Load the command history from the file.
void load_history()
{
    std::ifstream history_file(COMMAND_HISTORY_FILE);

    if (!history_file)
    {
        return;
    }

    std::string file_content = std::string(std::istreambuf_iterator(history_file), {});
    command_history          = std::views::split(file_content, COMMAND_HISTORY_DELIM) | std::ranges::to<std::vector<std::string>>();
    command_history.erase(std::remove_if(command_history.begin(), command_history.end(), [](const std::string &cmd) { return cmd.empty(); }), command_history.end());
}

/// Saves the command history to a file.
void save_history()
{
    std::ofstream history_file(COMMAND_HISTORY_FILE);

    if (!history_file)
    {
        return;
    }

    for (const auto &cmd : command_history)
    {
        history_file << cmd << COMMAND_HISTORY_DELIM;
    }
}

// ===
// Lifecycle management

/// Whether the REPL is initialized.
bool repl_initialized = false;

/// Initialize the REPL.
///
/// Perform tasks such as loading the command history, or the configuration file.
void initialize_repl()
{
    if (repl_initialized)
    {
        return;
    }

    // Populate the global context
    global_context->populate();

    // Load the command history
    load_history();

    repl_initialized = true;
}

/// Deinitialize the REPL.
///
/// Perform tasks such as saving the command history.
void deinitialize_repl()
{
    if (!repl_initialized)
    {
        return;
    }

    // Save the command history
    save_history();

    repl_initialized = false;
}

/// REPL RAII. This class provides constructor that ininitializes the
/// REPL and destructor that deinitializes the REPL.
struct repl_raii {
    /// Constructor initializes the REPL.
    ///
    /// It performs initialization tasks such as loading the command history, or
    /// the configuration file.
    repl_raii()
    {
        initialize_repl();
    }

    /// Destructor deinitializes the REPL.
    ///
    /// It performs deinitialization tasks such as saving the command history.
    ~repl_raii()
    {
        deinitialize_repl();
    }
};

// ===
// Core

/// Evaluate the repl command.
///
/// Do not call `interpret()` directly. Use `parse_command()` instead.
std::string interpret(std::string_view cmd_input)
{
    // Use the Fluxin's tokenizer to tokenize commands too
    auto tokens = tokenize(cmd_input);

    // No tokens, no output
    if (tokens.size() == 0)
    {
        return "";
    }

    // Check for commands
    if (tokens[0].type == token::token_type::identifier)
    {
        if (commands.contains(tokens[0].value))
        {
            return commands[tokens[0].value].evaluate(cmd_input, tokens);
        }
    }

    // Append the command to previous command
    previous_cmd.expr   = cmd_input;
    previous_cmd.tokens = tokens;

    // Evaluate the command as an expression
    std::ostringstream oss;
    oss << eval(cmd_input, tokens);
    return oss.str();
}

/// Parse the input to evaluate command(s).
///
/// This function splits commands based on ';', removes '\\\n' and prepares the
/// input to be executed.
std::string parse_command(std::string_view input)
{
    auto command_inputs = std::views::split(input, ';') | std::ranges::to<std::vector<std::string>>();

    for (auto &cmd : command_inputs)
    {
        for (size_t pos = 0; (pos = cmd.find("\\\n", pos)) != std::string::npos;)
            cmd.erase(pos, 1);
    }

    std::string output;

    for (std::size_t i = 0; i < command_inputs.size(); i++)
    {
        std::string command_output;
        try
        {
            command_output = interpret(command_inputs[i]);
        }
        catch (const formatted_error &e)
        {
            command_output = e.what();
        }
        catch (const std::exception &e)
        {
            command_output = std::string("$r") + e.what() + "$0";
        }

        output += command_output;

        if (!command_output.empty() && i < command_inputs.size() - 1)
        {
            if (command_output.ends_with('\n'))
            {
                output += ";\n";
            }
            else
            {
                output += "; ";
            }
        }
    }

    return output.empty() || output.ends_with('\n') ? output : output + "\n";
}

// ===
// Main Function.

/// Main Function.
int main(int argc, char *argv[])
{
    // ---
    // Initialization
    repl_raii repl_raii_obj; // Automatic deinitialization

    // ---
    // Argument parsing
    std::vector<std::string> args(argv + 1, argv + argc);

    bool evaluated_from_cmd = false;

    for (const std::string &arg : args)
    {
        if (arg == "--no-colorization")
        {
            colorization = false;
        }
        else if (arg == "--help" || arg == "-h")
        {
            std::println("Usage: {} [--no-colorization] [--help] [expression...]", argv[0]);
            std::println("  --no-colorization: Disable syntax highlighting");
            std::println("  --help: Show this help message");
            std::println("  expression: Expressions to evaluate");
            std::println("If no expression is provided, the REPL will start.");
            return 0;
        }
        else
        {
            evaluated_from_cmd = true;
            std::print("{}", pre_print_format(parse_command(arg)));
        }
    }

    if (evaluated_from_cmd)
    {
        return 0;
    }

    std::string input;
    std::print("> ");
    while (true)
    {
        std::string line;

        if (!std::getline(std::cin, line))
        {
            std::println();
            return 0;
        }

        input += line;

        if (line.ends_with('\\'))
        {
            input += "\n";
            std::print(". ");
            continue;
        }

        std::print("{}", pre_print_format(parse_command(input)));

        std::print("> ");
        input = "";
    }
}

// ===
// Implementation of the commands

/// Display help message for the command `var`.
///
/// Syntax:
/// - `help var`
std::string var_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*var variable_name $0$K[= expression]$0\n"
           "Creates a $*variable$0 with the given name.\n"
           "If an $*expression$0 is provided, it will be evaluated and assigned to the variable.\n"
           "If no expression is provided, the variable will be declared with a value of $*0$0.\n"
           "Example:\n"
           "  $*var x = 5$0\n"
           "  $*var y$0\n"
           "  $*x$0\n"
           "  $*y$0\n";
}

/// `var` command: Declare a variable or assign a value to it.
///
/// Syntax:
/// - `var variable_name`
/// - `var variable_name = expression`
std::string var_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    token name_tok = ts.expect_id();

    if (ts.is_eof())
    {
        // No value provided, just declare the variable
        global_context->set_variable(name_tok.value, 0.0f);
        return "";
    }

    // Value provided
    ts.expect_sym("=");
    ts.expect_not_eof();

    auto expr_tokens = ts.slice_rem();

    // Append the command to previous command
    previous_cmd.expr   = expr;
    previous_cmd.tokens = expr_tokens;

    float value = eval(expr, expr_tokens);
    global_context->set_variable(name_tok.value, value);
    return "";
}

/// Display help message for the command `fn`.
///
/// Syntax:
/// - `help fn`
std::string fn_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*fn function_name(param1, param2, ...) = expression$0\n"
           "Creates a $*function$0 with the given name and parameters.\n"
           "The function can be called with the same parameters to evaluate the expression.\n"
           "Example:\n"
           "  $*fn add(x, y) = x + y$0\n"
           "  $*add(2, 3)$0\n";
}

/// `fn` command: Declare a function.
///
/// Syntax:
/// - `fn function_name(param1, param2, ...) = expression`
std::string fn_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    token name_tok = ts.expect_id();

    token open_paren_tok = ts.expect_punc("(");

    std::vector<std::string> param_names;

    // Parse comma-separated parameters
    bool first_param = true;
    while (true)
    {
        if (ts.is_punc(")"))
        {
            ts.next();
            break;
        }

        if (!first_param)
        {
            ts.expect_punc(",");
        }

        token param_tok = ts.expect_id();
        param_names.push_back(param_tok.value);
        first_param = false;
    }

    ts.expect_sym("=");
    ts.expect_not_eof();

    auto expr_tokens = ts.slice_rem();

    // Append the command to previous command
    previous_cmd.expr   = expr;
    previous_cmd.tokens = expr_tokens;

    auto fn = [=](FLUXINS_FN_PARAMS) -> float {
        FLUXINS_FN_ARITY(name_tok.value, param_names.size());

        auto ctx = std::make_shared<context>();
        for (std::size_t i = 0; i < param_names.size(); i++)
        {
            ctx->set_variable(param_names[i], params[i]);
        }

        return eval(expr, expr_tokens, ctx);
    };

    global_context->set_function(name_tok.value, fn);

    return "";
}

/// Display help message for the command `unary_prefix_op`.
///
/// Syntax:
/// - `help unary_prefix_op`
std::string unary_prefix_op_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*unary_prefix_op symbol(x) = expression$0\n"
           "Creates a $*unary prefix operator$0 with the given symbol.\n"
           "The operator can be used in expressions with the same symbol.\n"
           "Example:\n"
           "  $*unary_prefix_op ++(x) = x + 1$0\n"
           "  $*++5$0\n";
}

/// `unary_prefix_op` command: Declare a unary prefix operator.
///
/// Syntax:
/// - `unary_prefix_op symbol(x) = expression`
std::string unary_prefix_op_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    token symbol_tok = ts.expect_sym();
    ts.expect_punc("(");
    token arg_tok = ts.expect_id();
    ts.expect_punc(")");

    ts.expect_sym("=");
    ts.expect_not_eof();

    auto expr_tokens = ts.slice_rem();

    // Append the command to previous command
    previous_cmd.expr   = expr;
    previous_cmd.tokens = expr_tokens;

    auto op = [=](FLUXINS_UOP_PARAMS) -> float {
        auto ctx = std::make_shared<context>();
        ctx->set_variable(arg_tok.value, x);
        return eval(expr, expr_tokens, ctx);
    };

    if (global_config->unary_prefix_op_exists(symbol_tok.value))
    {
        global_config->get_unary_prefix_op(symbol_tok.value).operate = op;
    }
    else
    {
        global_config->add_unary_prefix_op({ symbol_tok.value, op });
    }

    return "";
}

/// Display help message for the command `unary_suffix_op`.
///
/// Syntax:
/// - `help unary_suffix_op`
std::string unary_suffix_op_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*unary_suffix_op symbol(x) = expression$0\n"
           "Creates a $*unary suffix operator$0 with the given symbol.\n"
           "The operator can be used in expressions with the same symbol.\n"
           "Example:\n"
           "  $*unary_suffix_op --(x) = x - 1$0\n"
           "  $*5--$0\n";
}

/// `unary_suffix_op` command: Declare a unary suffix operator.
///
/// Syntax:
/// - `unary_suffix_op symbol(x) = expression`
std::string unary_suffix_op_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    token symbol_tok = ts.expect_sym();
    ts.expect_punc("(");
    token arg_tok = ts.expect_id();
    ts.expect_punc(")");

    ts.expect_sym("=");
    ts.expect_not_eof();

    auto expr_tokens = ts.slice_rem();

    // Append the command to previous command
    previous_cmd.expr   = expr;
    previous_cmd.tokens = expr_tokens;

    auto op = [=](FLUXINS_UOP_PARAMS) -> float {
        auto ctx = std::make_shared<context>();
        ctx->set_variable(arg_tok.value, x);
        return eval(expr, expr_tokens, ctx);
    };

    if (global_config->unary_suffix_op_exists(symbol_tok.value))
    {
        global_config->get_unary_suffix_op(symbol_tok.value).operate = op;
    }
    else
    {
        global_config->add_unary_suffix_op({ symbol_tok.value, op });
    }

    return "";
}

/// Display help message for the command `binary_op_left`.
///
/// Syntax:
/// - `help binary_op_left`
std::string binary_op_left_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*binary_op_left symbol(x, y) = expression$0\n"
           "Creates a $*binary operator$0 with the given symbol.\n"
           "The operator can be used in expressions with the same symbol.\n"
           "The operator is left associative.\n"
           "Example:\n"
           "  $*binary_op_left + (x, y) = x + y$0\n"
           "  $*5 + 3$0\n";
}

/// `binary_op_left` command: Declare a binary operator with left associativity.
///
/// Syntax:
/// - `binary_op_left symbol(x, y) = expression`
std::string binary_op_left_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    token symbol_tok = ts.expect_sym();
    ts.expect_punc("(");
    token arg1_tok = ts.expect_id();
    ts.expect_punc(",");
    token arg2_tok = ts.expect_id();
    ts.expect_punc(")");

    ts.expect_sym("=");
    ts.expect_not_eof();

    auto expr_tokens = ts.slice_rem();

    // Append the command to previous command
    previous_cmd.expr   = expr;
    previous_cmd.tokens = expr_tokens;

    auto op = [=](FLUXINS_BOP_PARAMS) -> float {
        auto ctx = std::make_shared<context>();
        ctx->set_variable(arg1_tok.value, x);
        ctx->set_variable(arg2_tok.value, y);
        return eval(expr, expr_tokens, ctx);
    };

    if (global_config->binary_op_exists(symbol_tok.value))
    {
        global_config->get_binary_op(symbol_tok.value).operate = op;
        global_config->get_binary_op(symbol_tok.value).assoc   = fluxins::associativity::left;
    }
    else
    {
        global_config->add_binary_op({ symbol_tok.value, fluxins::associativity::left, op });
        global_config->assign_precedence(symbol_tok.value, true);
    }

    return "";
}

/// Display help message for the command `binary_op_right`.
///
/// Syntax:
/// - `help binary_op_right`
std::string binary_op_right_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*binary_op_right symbol(x, y) = expression$0\n"
           "Creates a $*binary operator$0 with the given symbol.\n"
           "The operator can be used in expressions with the same symbol.\n"
           "The operator is right associative.\n"
           "Example:\n"
           "  $*binary_op_right ^ (x, y) = x ^ y$0\n"
           "  $*2 ^ 3$0\n";
}

/// `binary_op_right` command: Declare a binary operator with right associativity.
///
/// Syntax:
/// - `binary_op_right symbol(x, y) = expression`
std::string binary_op_right_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    token symbol_tok = ts.expect_sym();
    ts.expect_punc("(");
    token arg1_tok = ts.expect_id();
    ts.expect_punc(",");
    token arg2_tok = ts.expect_id();
    ts.expect_punc(")");

    ts.expect_sym("=");
    ts.expect_not_eof();

    auto expr_tokens = ts.slice_rem();

    // Append the command to previous command
    previous_cmd.expr   = expr;
    previous_cmd.tokens = expr_tokens;

    auto op = [=](FLUXINS_BOP_PARAMS) -> float {
        auto ctx = std::make_shared<context>();
        ctx->set_variable(arg1_tok.value, x);
        ctx->set_variable(arg2_tok.value, y);
        return eval(expr, expr_tokens, ctx);
    };

    if (global_config->binary_op_exists(symbol_tok.value))
    {
        global_config->get_binary_op(symbol_tok.value).operate = op;
        global_config->get_binary_op(symbol_tok.value).assoc   = fluxins::associativity::right;
    }
    else
    {
        global_config->add_binary_op({ symbol_tok.value, fluxins::associativity::right, op });
        global_config->assign_precedence(symbol_tok.value, true);
    }

    return "";
}

/// Display help message for the command `info`.
///
/// Syntax:
/// - `help info`
std::string info_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*info symbol10 $K[symbol...]$0\n"
           "Displays information about the given symbols.\n"
           "The symbols can be variables, functions, or operators.\n"
           "Example:\n"
           "  $*info x$0\n"
           "  $*info x y$0\n"
           "  $*info +$0\n";
}

/// `info` command: Display information about a symbol.
///
/// Syntax:
/// - `info symbol`
/// - `info symbol1 symbol2...`
std::string info_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    std::ostringstream oss;

    while (!ts.is_eof())
    {
        token name_tok = ts.peek();
        ts.next();

        if (global_context->variables.contains(name_tok.value))
        {
            oss << "$gVariable$0: $G$*" << name_tok.value << "$0\n";
            oss << "$gValue$0: $G$*" << global_context->variables[name_tok.value] << "$0\n";
        }

        if (global_context->functions.contains(name_tok.value))
        {
            oss << "$gFunction$0: $G$*" << name_tok.value << "$0\n";
        }

        if (global_config->unary_prefix_op_exists(name_tok.value))
        {
            oss << "$gUnary prefix operator$0: $G$*" << name_tok.value << "$0\n";
        }

        if (global_config->unary_suffix_op_exists(name_tok.value))
        {
            oss << "$gUnary suffix operator$0: $G$*" << name_tok.value << "$0\n";
        }

        if (global_config->binary_op_exists(name_tok.value))
        {
            oss << "$gBinary operator$0: $G$*" << name_tok.value << "$0\n";
            oss << "$gAssociativity$0: $G$*" << (global_config->get_binary_op(name_tok.value).assoc == fluxins::associativity::left ? "left" : "right") << "$0\n";
            oss << "$gPrecedence$0: $G$*" << global_config->get_precedence(name_tok.value) << "$0\n";
        }

        if (oss.str().empty())
        {
            oss << "$rNo information found for $R$*" << name_tok.value << "$0\n";
        }
    }

    return oss.str();
}

/// Display help message for the command `list`.
///
/// Syntax:
/// - `help list`
std::string list_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*list$0 $K[variables|functions|unary_op|unary_prefix_op|unary_suffix_op|binary_op|binary_op_left|binary_op_right|all]$0\n"
           "Lists all the symbols in the REPL.\n"
           "The symbols can be variables, functions, or operators.\n"
           "It includes built-in symbols as well.\n"
           "You can specify the type of symbols to list.\n"
           "If no type is specified, all symbols will be listed.\n"
           "Example:\n"
           "  $*list$0\n"
           "  $*list variables$0\n";
}

/// `list` command: List all the symbols.
///
/// Syntax:
/// - `list`
/// - `list variables`
/// - `list functions`
/// - `list unary_op`
/// - `list unary_prefix_op`
/// - `list unary_suffix_op`
/// - `list binary_op`
/// - `list binary_op_left`
/// - `list binary_op_right`
/// - `list all`
std::string list_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    std::string type = "all";
    if (!ts.is_eof())
    {
        type = ts.expect_id().value;
        ts.expect_eof();
    }

    std::ostringstream oss;

    if (type == "variables" || type == "all")
    {
        oss << "$gVariables$0:\n";
        for (const auto &[name, value] : global_context->variables)
        {
            oss << "  $G$*" << name << "$0: $gValue$0: $G$*" << value << "$0\n";
        }
    }

    if (type == "functions" || type == "all")
    {
        oss << "$gFunctions$0:\n";
        for (const auto &[name, _] : global_context->functions)
        {
            oss << "  $G$*" << name << "$0\n";
        }
    }

    if (type == "unary_op" || type == "all")
    {
        oss << "$gUnary prefix operators$0:\n";
        for (const auto &[symbol, _] : global_config->unary_prefix_operators)
        {
            oss << "  $G$*" << symbol << "$0\n";
        }
        oss << "$gUnary prefix operators$0:\n";
        for (const auto &[symbol, _] : global_config->unary_prefix_operators)
        {
            oss << "  $G$*" << symbol << "$0\n";
        }
    }

    if (type == "unary_prefix_op" || type == "all")
    {
        oss << "$gUnary prefix operators$0:\n";
        for (const auto &[symbol, _] : global_config->unary_prefix_operators)
        {
            oss << "  $G$*" << symbol << "$0\n";
        }
    }

    if (type == "unary_suffix_op" || type == "all")
    {
        oss << "$gUnary suffix operators$0:\n";
        for (const auto &[symbol, _] : global_config->unary_suffix_operators)
        {
            oss << "  $G$*" << symbol << "$0\n";
        }
    }

    if (type == "binary_op_left" || type == "all")
    {
        oss << "$gBinary operators (left associativity)$0:\n";
        for (const auto &[symbol, assoc, _] : global_config->binary_operators)
        {
            if (assoc == fluxins::associativity::left)
            {
                oss << "  $G$*" << symbol << "$0: $gPrecedence$0: $G$*" << global_config->get_precedence(symbol) << "$0\n";
            }
        }
    }

    if (type == "binary_op_right" || type == "all")
    {
        oss << "$gBinary operators (right associativity)$0:\n";
        for (const auto &[symbol, assoc, _] : global_config->binary_operators)
        {
            if (assoc == fluxins::associativity::right)
            {
                oss << "  $G$*" << symbol << "$0: $gPrecedence$0: $G$*" << global_config->get_precedence(symbol) << "$0\n";
            }
        }
    }

    if (type == "binary_op" || type == "all")
    {
        oss << "$gBinary operators$0:\n";
        for (const auto &[symbol, assoc, _] : global_config->binary_operators)
        {
            oss << "  $G$*" << symbol << "$0: $gAssociativity$0: $G$*" << associativity_to_string(assoc) << "$0, $gPrecedence$0: $G$*" << global_config->get_precedence(symbol) << "$0\n";
        }
    }

    std::string output = oss.str();

    if (output.empty())
    {
        output = "$rUnknown type$0: $R$*" + type + "$0\n";
    }

    return output;
}

/// Display help message for the command `undef`.
std::string undef_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*undef symbol$0 $K[symbol...]$0"
           "Undefines or deletes a symbol.\n"
           "The symbols can be variables, functions, or operators.\n"
           "Example:\n"
           "  $*undef x$0\n"
           "  $*undef x y$0\n"
           "  $*undef +$0\n";
}

/// `undef` command: Undefines or deletes a symbol.
///
/// Syntax:
/// - `undef symbol`
/// - `undef symbol1 symbol2...`
std::string undef_command_evaluator(COMMAND_PARAMS)
{
}

/// Display help message for the command `tokens`.
///
/// Syntax:
/// - `help tokens`
std::string tokens_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*tokens$0\n"
           "Displays the tokens of the previous expression.\n"
           "$bNote$0: This command only works if the previous expression was a valid expression.\n"
           "$bNote$0: If there are multiple commands in the previous expression, only the last one will be shown.\n";
}

/// `tokens` command: Print the tokens of the previous expression.
///
/// Syntax:
/// - `tokens`
std::string tokens_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    ts.expect_eof();

    if (previous_cmd.tokens.empty())
    {
        return "$rNo previous expression$0";
    }

    std::ostringstream oss;
    oss << "$gTokens$0:$G$*\n"
        << tokens_to_string(previous_cmd.expr, previous_cmd.tokens)
        << "$0";

    return oss.str();
}

/// Display help message for the command `ast`.
///
/// Syntax:
/// - `help ast`
std::string ast_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*ast$0\n"
           "Displays the AST of the previous expression.\n"
           "$bNote$0: This command only works if the previous expression was a valid expression.\n"
           "$bNote$0: If there are multiple commands in the previous expression, only the last one will be shown.\n";
}

/// `ast` command: Print the AST of the previous expression.
///
/// Syntax:
/// - `ast`
std::string ast_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    ts.expect_eof();

    if (previous_cmd.tokens.empty())
    {
        return "$rNo previous expression$0";
    }

    std::ostringstream oss;

    // Maybe we should'nt reparse the tokens just to print AST?
    auto ast = parse(previous_cmd.expr, previous_cmd.tokens, global_config);
    oss << "$gAST$0:$G$*\n"
        << ast->to_string(previous_cmd.expr)
        << "$0";

    return oss.str();
}

/// Display help message for the command `help`.
///
/// Syntax:
/// - `help help`
std::string help_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*help$0 $K[command_name]$0\n"
           "Displays the help message for the given command.\n"
           "If no command is specified, it lists all the commands.\n"
           "Example:\n"
           "  $*help$0\n"
           "  $*help var$0\n";
}

/// `help` command: Print help message.
///
/// Syntax:
/// - `help`
/// - `help command_name`
std::string help_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    if (ts.is_eof())
    {
        std::ostringstream oss;
        oss << "$gCommands$0:\n";
        for (const auto &[name, _] : commands)
        {
            oss << "  $G$*" << name << "$0\n";
        }
        return oss.str();
    }

    std::string command_name = ts.expect_id().value;
    ts.expect_eof();

    if (!commands.contains(command_name))
    {
        return "$rUnknown command$0: $R$*" + command_name + "$0\n";
    }

    return commands[command_name].docs(expr, args);
}

/// Display help message for the command `exit`.
///
/// Syntax:
/// - `help exit`
std::string exit_command_docs(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the help command
    ts.next(); // Consume the command name

    ts.expect_eof();

    return "Usage: $W$*exit$0 $K[status]$0\n"
           "Exits the REPL with the given status code.\n"
           "If no status code is provided, it exits with $*0$0.\n"
           "Example:\n"
           "  $*exit$0\n"
           "  $*exit 1$0\n";
}

/// `exit` command: Exit the REPL.
///
/// Syntax:
/// - `exit`
/// - `exit status`
std::string exit_command_evaluator(COMMAND_PARAMS)
{
    token_stream ts(expr, args);
    ts.next(); // Consume the command name

    int status = 0;
    if (!ts.is_eof())
    {
        status = eval(ts.expr, ts.slice_rem());
    }

    // Deinitialize the REPL
    deinitialize_repl();
    // Exit the program
    std::exit(status);
}
