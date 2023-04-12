# Megumi C Language

> ある春の日、俺は“運命”と出逢った……

Megumi C is a subset of C language. I call it `Megumi C` because **I love Kato Megumi**. :happy:

## Features
1. Token: A C program is a sequence of tokens.
    - [x] 1.1. Keywords
    - [x] 1.2. Identifiers
    - [ ] 1.3. Constants
    - [x] 1.4. Operators
2. Semicolon: A semicolon is a token that ends a statement.
3. Comments: A comment is a sequence of characters that is ignored by the compiler.
    - [x] 3.1. Single-line comments: `// comments`
    - [x] 3.2. Multi-line comments: `/* comments */`
4. Identifiers: An identifier is a sequence of letters, digits, and underscores that begins with a letter or an underscore.
    - [x] 4.1. Keywords cannot be used as identifiers.
5. Constants: A constant is a sequence of digits.
    - [ ] 5.1. Integer constants
6. Keywords: A keyword is a reserved word that has a special meaning to the compiler.
    - [x] 6.1. `int`
    - [x] 6.2. `return`
7. Operators: An operator is a special symbol that tells the compiler to perform specific mathematical or logical manipulations.
    - [x] 7.1. `+`
    - [x] 7.2. `-`
    - [ ] 7.3. `*`
    - [ ] 7.4. `/`
    - [ ] 7.5. `=`
    - [ ] 7.6. `==`
    - [ ] 7.7. `!=`
    - [ ] 7.8. `>`
    - [ ] 7.9. `<`
    - [ ] 7.10. `>=`
    - [ ] 7.11. `<=`
    - [x] 7.12. `!`
8. Expressions: An expression is a combination of constants, variables, operators, and function calls that the compiler evaluates and returns a single value.
    - [ ] 8.1. Arithmetic expressions
    - [ ] 8.2. Relational expressions
    - [ ] 8.3. Logical expressions
9. Functions: A function is a sequence of statements that performs a specific task.
    - [ ] 9.1. Function definition
        - Function can only be defined in the global scope.
        - Function can't be nested.
    - [ ] 9.2. Function call
        - Function can be called in any functions
        - Function can be recursively called
