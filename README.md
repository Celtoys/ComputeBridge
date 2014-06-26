ComputeBridge
-------------

#### The Library

A "loose" Lexer/Parser can transform a shared subset of similar languages into a stream of tokens with an associated high-level Abstract Syntax Tree (AST). This can be used to rewrite source files independently for each target, allowing (in this case) a simple compute language that can rewrite to either CUDA or OpenCL.

The loose nature of the parser allows it to be simple and faster than a complete parser. It gives more control over modifications than a typical C/C++ pre-processor would, generating an AST that includes:

* Preprocessor directives (you have the option of running a preprocessor before you feed your source file to the library).
* Statements and statement blocks.
* Function declarations/definitions with their bodies, parameters and any extra C++ decoration (e.g. const, throw).
* Data structure declarations/definitions with their names and tags.
* Type definitions/aliases.
* C++ classes with their associated constructors/destructors.
* Templated C++ classes or functions.
* Whitespace is preserved for accurate rewriting.

Within these AST nodes the source file is further broken into tokens that include numbers, symbols, operators and keywords.
