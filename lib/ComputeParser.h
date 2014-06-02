
//
// Copyright 2014 Celtoys Ltd
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef INCLUDED_COMPUTE_PARSER_H
#define INCLUDED_COMPUTE_PARSER_H


#ifdef __cplusplus
extern "C" {
#endif



//
// Boolean
//
typedef unsigned int cmpBool;
#define CMP_TRUE ((cmpBool)1)
#define CMP_FALSE ((cmpBool)0)



//
// Simple type aliases
//
typedef unsigned char cmpU8;
typedef unsigned short cmpU16;
typedef unsigned int cmpU32;
typedef char cmpS8;
typedef short cmpS16;
typedef int cmpS32;



//
// Calculate hash of a string with given length.
// Pass zero length on a null-terminated string to get the function to calculate the length.
//
cmpU32 cmpHash(const char* str, cmpU32 length);



//
// --- cmpError ----------------------------------------------------------------------------------------
// Simple error type, storing error text as a string.
// Instances of this type should not be created but are returned by functions in this API.
//

// Does the error description represent a successful operation and not an error?
struct cmpError;
cmpBool cmpError_OK(const struct cmpError* error);

typedef struct cmpError
{
	char text[512];

	// Returns true if the object is in an error state
	#ifdef __cplusplus
		operator bool () const
		{
			return !cmpError_OK(this);
		}
	#endif
} cmpError;

// Guaranteed to return a description of the error
const char* cmpError_Text(const cmpError* error);



//
// --- cmpMemoryFile -----------------------------------------------------------------------------------
// Stores an entire file in memory for reading by the lexer.
// Use of this is optional as you can provide your own data source to the lexer.
//
typedef struct cmpMemoryFile cmpMemoryFile;

cmpError cmpMemoryFile_Create(cmpMemoryFile** memory_file, const char* filename);

void cmpMemoryFile_Destroy(cmpMemoryFile* memory_file);

const char* cmpMemoryFile_Data(cmpMemoryFile* memory_file);

cmpU32 cmpMemoryFile_Size(cmpMemoryFile* memory_file);



//
// --- cmpLexerCursor -----------------------------------------------------------------------------------
// Used by the lexer to track its position and error state in a file.
//
typedef struct cmpLexerCursor cmpLexerCursor;

cmpError cmpLexerCursor_Create(cmpLexerCursor** cursor, const char* file_data, cmpU32 file_size);

void cmpLexerCursor_Destroy(cmpLexerCursor* cursor);

cmpU32 cmpLexerCursor_Position(cmpLexerCursor* cursor);

cmpU32 cmpLexerCursor_Line(cmpLexerCursor* cursor);

cmpU32 cmpLexerCursor_LinePosition(cmpLexerCursor* cursor);

cmpError cmpLexerCursor_Error(cmpLexerCursor* cursor);



//
// --- cmpTokenType ------------------------------------------------------------------------------------
// List of all tokens the lexer can process.
//
enum cmpTokenType
{
	cmpToken_None,

	// Single character tokens match their character values for simpler switch code
	cmpToken_LBrace = '{',
	cmpToken_RBrace = '}',
	cmpToken_Comma = ',',
	cmpToken_LBracket = '(',
	cmpToken_RBracket = ')',
	cmpToken_LSqBracket = '[',
	cmpToken_RSqBracket = ']',
	cmpToken_Colon = ':',
	cmpToken_SemiColon = ';',
	cmpToken_Period = '.',
	cmpToken_Question = '?',
	cmpToken_Tilde = '~',
	cmpToken_LAngle = '<',
	cmpToken_RAngle = '>',
	cmpToken_Plus = '+',
	cmpToken_Minus = '-',
	cmpToken_Asterisk = '*',
	cmpToken_Divide = '/',
	cmpToken_Modulo = '%',
	cmpToken_Equals = '=',
	cmpToken_And = '&',
	cmpToken_Or = '|',
	cmpToken_Xor = '^',
	cmpToken_Not = '!',
	cmpToken_Hash = '#',

	// Used to ensure no tokens that follow have the same enum value as the single character tokens
	cmpToken_InvalidSeparator = 1000,

	// Composite operator tokens
	cmpToken_LessEqual,
	cmpToken_ShiftLeft,
	cmpToken_GreaterEqual,
	cmpToken_ShiftRight,
	cmpToken_PlusEqual,
	cmpToken_Increment,
	cmpToken_MinusEqual,
	cmpToken_Decrement,
	cmpToken_Pointer,
	cmpToken_MultiplyEqual,
	cmpToken_DivideEqual,
	cmpToken_ModuloEqual,
	cmpToken_EqualCompare,
	cmpToken_AndEqual,
	cmpToken_AndCompare,
	cmpToken_OrEqual,
	cmpToken_OrCompare,
	cmpToken_XorEqual,
	cmpToken_NotEqualCompare,
	cmpToken_SymbolJoin,

	// High-level semantic tokens
	cmpToken_Comment,
	cmpToken_String,
	cmpToken_Number,
	cmpToken_Symbol,

	// Tokens used for rewriting that can safely be ignored
	cmpToken_EOL,
	cmpToken_Whitespace,

	// Keyword tokens
	cmpToken_Typedef,
	cmpToken_Struct,
};

// Return the name of a given token
const char* cmpTokenType_Name(enum cmpTokenType type);



//
// --- cmpToken ----------------------------------------------------------------------------------------
// Token description returned by the lexer
//
typedef struct cmpToken
{
	enum cmpTokenType type;

	// Pointer into the memory file with the length of the token
	const char* start;
	cmpU32 length;

	cmpU32 line;

	// Some tokens record a hash of their contents for quick comparisons
	cmpU32 hash;

	// All tokens must be linked in order for the parser to process them
	struct cmpToken* prev;
	struct cmpToken* next;
} cmpToken;

void cmpToken_Destroy(cmpToken* token);

void cmpToken_AddToList(cmpToken** first_token, cmpToken** last_token, cmpToken* token);



//
// --- cmpLexer ----------------------------------------------------------------------------------------
// Very loose Lexer for C-style languages.
// Single function for returning the current token at the cursor and advancing to the next one.
// Returns NULL when there are no more tokens left to read.
//
cmpToken* cmpLexer_ConsumeToken(cmpLexerCursor* cur);



//
// --- cmpParserCursor ----------------------------------------------------------------------------------------
//
typedef struct cmpParserCursor cmpParserCursor;

cmpError cmpParserCursor_Create(cmpParserCursor** cursor, cmpToken* first_token);

void cmpParserCursor_Destroy(cmpParserCursor* cursor);

cmpU32 cmpParserCursor_Line(cmpParserCursor* cursor);

cmpError cmpParserCursor_Error(cmpParserCursor* cursor);


//
// --- cmpNodeType ------------------------------------------------------------------------------------
// List of all nodes the parser generates.
//
enum cmpNodeType
{
	cmpNode_None,
	cmpNode_PPDirective,
	cmpNode_Statement,
	cmpNode_StatementBlock,
	cmpNode_FunctionDefn,
	cmpNode_FunctionDecl,
	cmpNode_FunctionParams,
	cmpNode_StructDefn,
	cmpNode_StructDecl,
	cmpNode_StructTag,
	cmpNode_StructName,
	cmpNode_Typedef,

	// Points directly to a single token that can be ignored semantically but used for rewriting
	cmpNode_Token,

	// A node that contains user modifications to the file
	cmpNode_UserTokens,
};

const char* cmpNodeType_Name(enum cmpNodeType type);


//
// --- cmpNode ----------------------------------------------------------------------------------------
//
typedef struct cmpNode
{
	enum cmpNodeType type;

	// Links to child nodes
	struct cmpNode* first_child;
	struct cmpNode* last_child;
	struct cmpNode* next_sibling;

	cmpToken* first_token;
	cmpToken* last_token;
} cmpNode;


void cmpNode_Destroy(cmpNode* node);

cmpNode* cmpParser_ConsumeNode(cmpParserCursor* cur);

void cmpParser_LogNodes(const cmpNode* node, cmpU32 depth);



#ifdef __cplusplus
}
#endif


#endif