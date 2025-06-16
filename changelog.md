# v1.0.0

## Initial Release

This release is the first release of the Fluxins expression parser/evaluator. This release includes a list of default operators and symbols that allows expressions to be expressive.

# v1.0.1

## New Features

- `fluxins::express` is a new shorthand way to tokenize, parse and evaluate the expression with one function call! 

## Bug Fixes

- Fixed missing include errors, including those that did not cause any errors on my computer.

# v1.0.2

A lot of tiny tweaks has been done to polish the repository.

Introduced `compile_flags.txt` with minimal compile flags to work on files with LSP.

## Removed

- Removed unused helpers (`fluxins::code::get_line` and `fluxins::code::get_lines`).
