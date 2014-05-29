ComputeBridge
-------------

#### The Library

A "loose" Lexer/Parser can turn a shared subset of a few similar languages into a stream of tokens and parse tree nodes that can be used to transform source files independently for each target. This allows you to build a simple compute language that targets both OpenCL and CUDA at the same time.

The loose nature of the parser allows it to be very simple and much faster than a complete parser, however it does mean you gain reduced insight into what the parse tree of a source file is. This is a recursive descent parser that currently recognises:

* Preprocessor directives (you have the option of running a preprocessor before you feed your source file to the library.
* Statements and statement blocks.
* Function declarations/definitions and their parameters.
* Data structure declarations/definitions with their names and tags.
* Type definitions/aliases.

Within these entities, the source file is further broken into tokens that can be used to accurately rewrite an input file.