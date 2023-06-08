# Megumi C Language Compiler

[![CMake](https://github.com/SankHyan24/MegumiC/actions/workflows/cmake.yml/badge.svg)](https://github.com/SankHyan24/MegumiC/actions/workflows/cmake.yml)

> ある春の日、俺は“運命”と出逢った……

Megumi C is a subset of C language. I call it `Megumi C` because **I love Kato Megumi**.😎

MegumiC is a compiler I implemented in the Compilers Principle course of Computer Science, Zhejiang University.

The IR code I used refers to [Koopa IR](https://pku-minic.github.io/online-doc/#/misc-app-ref/koopa) from pku-minic.

The Target code is RV32.

## Dependencies

- [Flex](http://gnu.ist.utl.pt/software/flex/flex.html)
- [Bison](https://www.gnu.org/software/bison/)
- [sysy-runtime-lib](https://github.com/pku-minic/sysy-runtime-lib/)

## Language Features

1. Basic Language Grammars:
    - [x] 1.1. Nearly all the C language features except for `struct` and `union`.
2. Data Types:
    - [x] 2.1. `int`: 4 bytes in RV32.
    - [x] 2.2. `ptr`: 4 bytes in RV32. It has the same function as `int*` in C language.
3. Operators:
    - [x] 3.1. Support nearly all the operators in C language.
    - [ ] 3.2. Except for address operator `&`, pointer operator `*` and shift operators `<<` and `>>`.
4. Control Flow:
    - [x] 4.1. `if`-`else` statement.
    - [x] 4.2. `while` statement.
    - [x] 4.3. `break` and `continue` statement.
    - [x] 4.4. `return` statement.
5. Functions:
    - [x] 5.1. Support function declaration and definition, you should use `static` to declare a function.
    - [x] 5.2. Support recursively function call, function parameters and return value (default int).
6. Arrays:
    - [x] 6.1. Support any-dimension array and pointer operation.
    - [x] 6.2. Support array declaration, definition and indexing.
    - [x] 6.3. Support array initialization, you can use `{}` to initialize a 1-dimension array, use nested `{}` to initialize a multi-dimension array.

## Compiler Features

1. Lexical Analysis:
    - [x] 1.1. Support all the tokens in Megumi C language.
2. Syntax Analysis:
    - [x] 2.1. Support all the grammars in Megumi C language.
    - [ ] 2.2. Error recovery not supported now.
3. Semantic Analysis:
    - [x] 3.1. Support all the semantic analysis in Megumi C language.
    - [x] 3.2. Support type checking and type conversion.
4. Intermediate Representation:
    - [x] 4.1. Use Koopa IR.
    - [ ] 4.2. Constant folding not supported now.
5. Code Generation:
    - [x] 5.1. Support all the code generation in Megumi C language.
    - [x] 5.2. Support optimization, like simple peephole optimization.


## A simple Megumi C program

```cpp
static int PrintArr(int a[15], int n)
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
