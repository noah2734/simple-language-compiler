# Simple Language Compiler

A compiler for a custom programming language supporting scalar variables, arrays, expressions, and array-based operations.

This project explores the core concepts behind compiler construction, including reading source code written in a custom language, validating its structure, and translating supported statements into an executable or intermediate representation.

## Example

The compiler can process programs using syntax such as:

```text
SCALAR x y z
ARRAY a b c
{
    a[x] = a[.]/b[.];
}
```

This example demonstrates:

* Scalar variable declarations
* Array declarations
* Indexed array access
* Arithmetic expressions
* Assignment statements
* Structured program blocks

## Features

* Parses source code written in a custom programming language
* Supports scalar and array declarations
* Processes variable and array references
* Handles arithmetic expressions and assignment operations
* Validates supported program syntax
* Translates source-language instructions through a compiler pipeline
* Provides a foundation for experimenting with programming-language and compiler-design concepts

## Compiler Concepts

This project applies several concepts commonly used in compiler development, including:

* Lexical and syntactic analysis
* Source-language parsing
* Expression processing
* Variable and array handling
* Syntax validation
* Program translation

## Example Program

```text
SCALAR x y z
ARRAY a b c
{
    a[x] = a[.]/b[.];
}
```

The compiler reads the declarations and statements in the source program and processes them according to the grammar and rules of the custom language.

## Project Structure

The repository contains the source code responsible for parsing and compiling programs written in the custom language.

Additional documentation for the grammar, compiler stages, and supported syntax can be added as the project evolves.

## Running the Project

Clone the repository:

```bash
git clone <repository-url>
cd <repository-name>
```

Then build and run the compiler using the development environment or build process appropriate for the project.

> Build and execution instructions should be updated here with the project's specific commands.

## What I Learned

Building this compiler provided hands-on experience with how programming languages are interpreted by software, including how source code can be broken into structured components, validated against language rules, and translated into a form that can be executed or further processed.

The project strengthened my understanding of:

* Compiler architecture
* Parsing and language grammars
* Data structures
* Expression evaluation
* Error handling
* Low-level program representation
* Software design for language-processing systems

## Future Improvements

Potential extensions include:

* More descriptive compiler error messages
* Additional data types
* Additional arithmetic and logical operators
* Control-flow statements
* Functions and procedures
* Expanded array operations
* Additional compiler optimizations
* Automated test coverage

## License

This project is available for educational and portfolio purposes.
