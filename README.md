# Compiler Simulator (Scanner & Parser)
A Mini **Compiler Front-End** for a Custom Programming Language

## Overview
This project implements the first two stages of a compiler:
- **Lexical Analysis** (Scanner)
- **Syntax Analysis** (Parser)

The goal is to read a source code file written in a custom-designed programming language, break it into tokens, **validate its syntax** according to grammar rules, and **detect lexical and syntactic errors**.
The project provides hands-on experience with how programming languages work internally — how a compiler reads, interprets, and validates code before execution.

## What We Built
### ✔️ 1. Scanner (Lexical Analyzer)

The scanner reads the source code character-by-character and converts it into a sequence of **tokens** such as:

- Keywords

- Identifiers

- Constants

- Operators

- Braces

- Type names

- Comments

It also:

- Ignores whitespace and comments

- Detects invalid identifiers and malformed tokens

- Processes **include commands** by opening additional source files recursively

- Outputs each token with its type and line number

### ✔️ 2. Parser (Syntax Analyzer)

The parser takes the scanner output and checks whether the token sequence matches the **grammar rules** of the language.

It handles:

- Variable declarations

- Function declarations

- Expressions

- Conditionals (`IfTrue`, `Otherwise`)

- Loops (`RepeatWhen`, `Reiterate`)

- Return statements (Turnback)

- Blocks and compound statements
