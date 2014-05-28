ComputeBridge
-------------

#### The Library

A "loose" Lexer/Parser can turn a shared subset of a few similar languages into a stream of tokens and parse tree nodes that can be used to transform source files independently for each target. This allows you to build a simple compute language that targets both OpenCL and CUDA at the same time.

The loose nature of the library allows it to be very simple, but it does mean you gain limited insight into what the parse tree of a source file is. This is a recursive descent parser that currently recognises:

* Preprocessor directives (you have the option of running a preprocessor before you feed your source file to the library.
* Statements.
* Statement blocks.
* Functions.

Within these entities, the source file is further broken into tokens that can be used to identify function names, parameters, etc. 