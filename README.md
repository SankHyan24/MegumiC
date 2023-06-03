# Megumi C Language Compiler

> ある春の日、俺は“運命”と出逢った……

Megumi C is a subset of C language. I call it `Megumi C` because **I love Kato Megumi**.😎

MegumiC is a compiler I implemented in the Compilers Principle course of Computer Science, Zhejiang University.

The IR code I used refers to [Koopa IR](https://pku-minic.github.io/online-doc/#/misc-app-ref/koopa) from pku-minic.

The Target code is RV32.

## Dependencies
- [Flex]()
- [Bison]()
- [sysy-runtime-lib](https://github.com/pku-minic/sysy-runtime-lib/)

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
    - [x] 5.1. Integer constants
6. Keywords: A keyword is a reserved word that has a special meaning to the compiler.
    - [x]  `int` `return` `static` `while` `if` `else` `break` `continue`
7. Operators: An operator is a special symbol that tells the compiler to perform specific mathematical or logical manipulations.
    - [x] 7.1. Binary operations: `+` `-` `*` `/`
    - [x] 7.2. Logical operations: `==` `!=` `>` `<` `>=` `<=` 
8. Expressions: An expression is a combination of constants, variables, operators, and function calls that the compiler evaluates and returns a single value.
    - [x] 8.1. Arithmetic expressions
    - [x] 8.2. Relational expressions
    - [x] 8.3. Logical expressions
    - [x] 8.3. Immediate numbers and single characters
9. Functions: A function is a sequence of statements that performs a specific task.
    - [x] 9.1. Function definition
        - Function can only be defined in the global scope.
        - Function defination must start by a `static` keyword.
    - [x] 9.2. Function call
        - Function can be called in any functions
        - Function can be recursively called

## A simple Megumi C program
```c
static int PrintArr(int a[100], int n)
{
	int i = 0;
	while (i < n)
	{
		putch(a[i]);
		i = i + 1;
	}
	return 0;
}
// print Hello, world!
static int main()
{
	int n[15] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!', 10};
	PrintArr(n, 14);
	return 0;
}
```

A simple [QuickSort program](https://github.com/SankHyan24/MegumiC/blob/main/test/task1.c) written in Megumi C.
