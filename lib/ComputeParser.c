
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

#include "ComputeParser.h"

#include <stdarg.h>
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>



cmpU32 cmpHash(const char* str, cmpU32 length)
{
	cmpU32 c, hash = 0;

	// If caller doesn't set the length, calculate it here
	if (length == 0)
		length = strlen(str);

	// Permute hash until terminating NULL or length runs out
	while ((c = *str++) && length--)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}



// =====================================================================================================
// cmpError
// =====================================================================================================



static cmpError cmpError_CreateOK()
{
	cmpError error;
	error.text[0] = 0;
	return error;
}


static cmpError cmpError_Create(const char* format, ...)
{
	cmpError error;

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(error.text, sizeof(error.text) - 1, format, argptr);
	va_end(argptr);

	return error;
}


cmpBool cmpError_OK(const cmpError* error)
{
	assert(error != NULL);
	return error->text[0] == 0 ? CMP_TRUE : CMP_FALSE;
}


const char* cmpError_Text(const cmpError* error)
{
	assert(error != NULL);
	return error->text[0] == 0 ? "No Error" : error->text;
}



// =====================================================================================================
// cmpMemoryFile
// =====================================================================================================



struct cmpMemoryFile
{
	char* data;
	cmpU32 nb_bytes;
};


cmpError cmpMemoryFile_Create(cmpMemoryFile** memory_file, const char* filename)
{
	FILE* fp;

	assert(memory_file != NULL);

	// Allocate the container
	*memory_file = malloc(sizeof(cmpMemoryFile));
	if (*memory_file == NULL)
		return cmpError_Create("malloc(cmpMemoryFile) failed");

	// Set defaults
	(*memory_file)->data = NULL;
	(*memory_file)->nb_bytes = 0;

	// Open the file
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		cmpMemoryFile_Destroy(*memory_file);
		return cmpError_Create("Couldn't open file '%s'", filename);
	}

	// Allocate enough space to store the file in memory
	if (fseek(fp, 0, SEEK_END) != 0)
	{
		fclose(fp);
		cmpMemoryFile_Destroy(*memory_file);
		return cmpError_Create("Couldn't figure out size of file '%s'", filename);
	}
	(*memory_file)->nb_bytes = ftell(fp);
	(*memory_file)->data = malloc((*memory_file)->nb_bytes);
	if (fseek(fp, 0, SEEK_SET) != 0)
	{
		fclose(fp);
		cmpMemoryFile_Destroy(*memory_file);
		return cmpError_Create("Couldn't figure out size of file '%s'", filename);
	}

	// Read the entire file into memory
	if (fread((*memory_file)->data, 1, (*memory_file)->nb_bytes, fp) != (*memory_file)->nb_bytes)
	{
		fclose(fp);
		cmpMemoryFile_Destroy(*memory_file);
		return cmpError_Create("Failed to read %d bytes from file '%s'", (*memory_file)->nb_bytes, filename);
	}

	fclose(fp);
	return cmpError_CreateOK();
}


void cmpMemoryFile_Destroy(cmpMemoryFile* memory_file)
{
	assert(memory_file != NULL);

	if (memory_file->data != NULL)
		free(memory_file->data);

	free(memory_file);
}


const char* cmpMemoryFile_Data(cmpMemoryFile* memory_file)
{
	assert(memory_file != NULL);
	return memory_file->data;
}


cmpU32 cmpMemoryFile_Size(cmpMemoryFile* memory_file)
{
	assert(memory_file != NULL);
	return memory_file->nb_bytes;
}



// =====================================================================================================
// cmpLexerCursor
// =====================================================================================================



struct cmpLexerCursor
{
	// Lexer source file
	const char* file_data;
	cmpU32 file_size;

	// Position within the file
	cmpU32 position;
	cmpU32 line;
	cmpU32 line_position;

	// Current error
	cmpError error;
};


cmpError cmpLexerCursor_Create(cmpLexerCursor** cursor, const char* file_data, cmpU32 file_size)
{
	assert(cursor != NULL);

	// Allocate the container
	*cursor = malloc(sizeof(cmpLexerCursor));
	if (*cursor == NULL)
		return cmpError_Create("malloc(cmpLexerCursor) failed");

	// Set defaults
	(*cursor)->file_data = file_data;
	(*cursor)->file_size = file_size;
	(*cursor)->position = 0;
	(*cursor)->line = 1;
	(*cursor)->line_position = 0;
	(*cursor)->error = cmpError_CreateOK();

	return cmpError_CreateOK();
}


void cmpLexerCursor_Destroy(cmpLexerCursor* cursor)
{
	assert(cursor != NULL);
	free(cursor);
}


cmpU32 cmpLexerCursor_Position(cmpLexerCursor* cursor)
{
	assert(cursor != NULL);
	return cursor->position;
}


cmpU32 cmpLexerCursor_Line(cmpLexerCursor* cursor)
{
	assert(cursor != NULL);
	return cursor->line;
}


cmpU32 cmpLexerCursor_LinePosition(cmpLexerCursor* cursor)
{
	assert(cursor != NULL);
	return cursor->line_position;
}


cmpError cmpLexerCursor_Error(cmpLexerCursor* cursor)
{
	assert(cursor != NULL);
	return cursor->error;
}


static void cmpLexerCursor_ConsumeChars(cmpLexerCursor* cursor, int size)
{
	cmpU32 next_pos;

	assert(cursor != NULL);

	// Catches overflow, hitting EOF and errors from any previous calls
	next_pos = cursor->position + size;
	if (next_pos >= cursor->file_size)
	{
		// Move cursor to the EOF
		cursor->position = cursor->file_size;
		return;
	}

	// General case of moving over the next N characters
	cursor->position += size;
}


static void cmpLexerCursor_ConsumeChar(cmpLexerCursor* cursor)
{
	cmpLexerCursor_ConsumeChars(cursor, 1);
}


static const char* cmpLexerCursor_PeekChars(cmpLexerCursor* cursor, cmpU32 lookahead)
{
	assert(cursor != NULL);

	// Nothing to read at EOF
	if (cursor->position + lookahead >= cursor->file_size)
		return 0;

	return cursor->file_data + cursor->position + lookahead;
}


static char cmpLexerCursor_PeekChar(cmpLexerCursor* cursor, cmpU32 lookahead)
{
	assert(cursor != NULL);

	// Nothing to read at EOF
	if (cursor->position + lookahead >= cursor->file_size)
		return EOF;

	return cursor->file_data[cursor->position + lookahead];
}


static void cmpLexerCursor_IncLine(cmpLexerCursor* cursor)
{
	assert(cursor != NULL);
	cursor->line++;
	cursor->line_position = cursor->position + 1;
}


static void cmpLexerCursor_SetError(cmpLexerCursor* cursor, const cmpError* error)
{
	assert(cursor != NULL);
	cursor->error = *error;
}



// =====================================================================================================
// cmpTokenType
// =====================================================================================================



const char* cmpTokenType_Name(enum cmpTokenType type)
{
	switch (type)
	{
		case cmpToken_None: return "cmpToken_None";
		case cmpToken_LBrace: return "cmpToken_LBrace";
		case cmpToken_RBrace: return "cmpToken_RBrace";
		case cmpToken_Comma: return "cmpToken_Comma";
		case cmpToken_LBracket: return "cmpToken_LBracket";
		case cmpToken_RBracket: return "cmpToken_RBracket";
		case cmpToken_LSqBracket: return "cmpToken_LSqBracket";
		case cmpToken_RSqBracket: return "cmpToken_RSqBracket";
		case cmpToken_Colon: return "cmpToken_Colon";
		case cmpToken_SemiColon: return "cmpToken_SemiColon";
		case cmpToken_Period: return "cmpToken_Period";
		case cmpToken_Question: return "cmpToken_Question";
		case cmpToken_Tilde: return "cmpToken_Tilde";
		case cmpToken_LAngle: return "cmpToken_LAngle";
		case cmpToken_RAngle: return "cmpToken_RAngle";
		case cmpToken_Plus: return "cmpToken_Plus";
		case cmpToken_Minus: return "cmpToken_Minus";
		case cmpToken_Asterisk: return "cmpToken_Asterisk";
		case cmpToken_Divide: return "cmpToken_Divide";
		case cmpToken_Modulo: return "cmpToken_Modulo";
		case cmpToken_Equals: return "cmpToken_Equals";
		case cmpToken_And: return "cmpToken_And";
		case cmpToken_Or: return "cmpToken_Or";
		case cmpToken_Xor: return "cmpToken_Xor";
		case cmpToken_Not: return "cmpToken_Not";
		case cmpToken_Hash: return "cmpToken_Hash";
		case cmpToken_InvalidSeparator: return "cmpToken_InvalidSeparator";
		case cmpToken_LessEqual: return "cmpToken_LessEqual";
		case cmpToken_ShiftLeft: return "cmpToken_ShiftLeft";
		case cmpToken_GreaterEqual: return "cmpToken_GreaterEqual";
		case cmpToken_ShiftRight: return "cmpToken_ShiftRight";
		case cmpToken_PlusEqual: return "cmpToken_PlusEqual";
		case cmpToken_Increment: return "cmpToken_Increment";
		case cmpToken_MinusEqual: return "cmpToken_MinusEqual";
		case cmpToken_Decrement: return "cmpToken_Decrement";
		case cmpToken_Pointer: return "cmpToken_Pointer";
		case cmpToken_MultiplyEqual: return "cmpToken_MultiplyEqual";
		case cmpToken_DivideEqual: return "cmpToken_DivideEqual";
		case cmpToken_ModuloEqual: return "cmpToken_ModuloEqual";
		case cmpToken_EqualCompare: return "cmpToken_EqualCompare";
		case cmpToken_AndEqual: return "cmpToken_AndEqual";
		case cmpToken_AndCompare: return "cmpToken_AndCompare";
		case cmpToken_OrEqual: return "cmpToken_OrEqual";
		case cmpToken_OrCompare: return "cmpToken_OrCompare";
		case cmpToken_XorEqual: return "cmpToken_XorEqual";
		case cmpToken_NotEqualCompare: return "cmpToken_NotEqualCompare";
		case cmpToken_SymbolJoin: return "cmpToken_SymbolJoin";
		case cmpToken_Comment: return "cmpToken_Comment";
		case cmpToken_String: return "cmpToken_String";
		case cmpToken_Number: return "cmpToken_Number";
		case cmpToken_Symbol: return "cmpToken_Symbol";
		case cmpToken_EOL: return "cmpToken_EOL";
		case cmpToken_Whitespace: return "cmpToken_Whitespace";
		case cmpToken_Typedef: return "cmpToken_Typedef";
		case cmpToken_Struct: return "cmpToken_Struct";
	}

	return "<INTERNAL-ERROR> No token name found";
}



// =====================================================================================================
// cmpToken
// =====================================================================================================



static cmpError cmpToken_Create(cmpToken**token, cmpLexerCursor* cur, enum cmpTokenType type, cmpU32 length)
{
	assert(token != NULL);

	// Allocate the container
	*token = malloc(sizeof(cmpToken));
	if (*token == NULL)
		return cmpError_Create("malloc(cmpToken) failed");

	(*token)->type = type;
	(*token)->start = cmpLexerCursor_PeekChars(cur, 0);
	(*token)->length = length;
	(*token)->line = cur->line;
	(*token)->hash = 0;
	(*token)->prev = NULL;
	(*token)->next = NULL;

	return cmpError_CreateOK();
}


void cmpToken_Destroy(cmpToken* token)
{
	assert(token != NULL);
	free(token);
}


void cmpToken_AddToList(cmpToken** first_token, cmpToken** last_token, cmpToken* token)
{
	assert(first_token != NULL);
	assert(last_token != NULL);
	assert(token != NULL);

	if (*first_token == NULL)
	{
		*first_token = token;
		*last_token = token;
	}
	else
	{
		(*last_token)->next = token;
		token->prev = *last_token;
		*last_token = token;
	}
}



// =====================================================================================================
// cmpLexer
// =====================================================================================================



typedef cmpBool (*cmpLexer_Predicate)(cmpLexerCursor* cur, cmpToken* token, char c, void* state);


static cmpToken* cmpLexer_ConsumeTokenPred(cmpLexerCursor* cur, enum cmpTokenType type, cmpU32 initial_length, cmpLexer_Predicate p, void* state)
{
	// Start the token off
	cmpToken* token;
	cmpError error = cmpToken_Create(&token, cur, type, initial_length);
	if (!cmpError_OK(&error))
	{
		cmpLexerCursor_SetError(cur, &error);
		return NULL;
	}
	cmpLexerCursor_ConsumeChars(cur, initial_length);

	// Scan all characters until EOF or the predicate says so
	while (1)
	{
		char c = cmpLexerCursor_PeekChar(cur, 0);
		if (c == EOF)
			break;

		if (!p(cur, token, c, state))
			break;

		cmpLexerCursor_ConsumeChar(cur);
		token->length++;
	}

	return token;
}


static cmpBool cmpLexer_IsWhitespace(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}


static cmpBool cmpLexer_IsCComment(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	char* last_c = (char*)state;
	if (*last_c == '*' && c == '/')
	{
		cmpLexerCursor_ConsumeChar(cur);
		token->length++;
		return CMP_FALSE;
	}

	// Process line increments that can occur in a comment
	if (c == '\n')
		cmpLexerCursor_IncLine(cur);

	*last_c = c;
	return CMP_TRUE;
}


static cmpBool cmpLexer_IsCppComment(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	// Don't consume EOL, let outer-loop character match increment line number
	return c != '\n' && c != '\r';
}


static cmpBool cmpLexer_IsString(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	if (c == '"')
	{
		// Consume closing quotation
		cmpLexerCursor_ConsumeChar(cur);
		token->length++;
		return CMP_FALSE;
	}

	return CMP_TRUE;
}


static cmpBool cmpLexer_IsNumber(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	// Loosely matches hex and exponent numbers, allowing all letters as part of the number
	// As this isn't valid input C, it simply passes the error onto whatever compiles the output
	// It will also fuse together +/- operators as numbers
	return (isalnum(c) || c == '.' || c == '+' || c == '-') ? CMP_TRUE : CMP_FALSE;
}


static cmpBool cmpLexer_IsSymbol(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	return (c == '_' || isalnum(c)) ? CMP_TRUE : CMP_FALSE;
}


// Lazy-initialised hashes for all keywords - assumes no collisions
static cmpU32 HASH_typedef = 0;
static cmpU32 HASH_struct = 0;


static void cmpLexer_IdentifyKeywordTokens(cmpToken* token)
{	
	assert(token != NULL);
	assert(token->start != NULL);

	// Store token hash of the symbol for callers to use
	token->hash = cmpHash(token->start, token->length);

	// Initialise keyword hashes first-time round
	if (HASH_typedef == 0)
	{
		HASH_typedef = cmpHash("typedef", 0);
		HASH_struct = cmpHash("struct", 0);
	}

	// Switch on first character to reduce token hashing and sequential compares
	switch (token->start[0])
	{
		case 't':
			if (token->hash == HASH_typedef)
				token->type = cmpToken_Typedef;
			break;

		case 's':
			if (token->hash == HASH_struct)
				token->type = cmpToken_Struct;
			break;
	}
}


//
// Tables that map the possible permutations from a single character that operators can take
//
typedef struct
{
	char c;
	enum cmpTokenType type;
} OpMatch;
static OpMatch OP_LAngle[] = { '=', cmpToken_LessEqual, '<', cmpToken_ShiftLeft, 0 };
static OpMatch OP_RAngle[] = { '=', cmpToken_GreaterEqual, '>', cmpToken_ShiftRight, 0 };
static OpMatch OP_Plus[] = { '=', cmpToken_PlusEqual, '+', cmpToken_Increment, 0 };
static OpMatch OP_Minus[] = { '=', cmpToken_MinusEqual, '-', cmpToken_Decrement, '>', cmpToken_Pointer, 0 };
static OpMatch OP_Asterisk[] = { '=', cmpToken_MultiplyEqual, 0 };
static OpMatch OP_Divide[] = { '=', cmpToken_DivideEqual, 0 };
static OpMatch OP_Modulo[] = { '=', cmpToken_ModuloEqual, 0 };
static OpMatch OP_Equals[] = { '=', cmpToken_EqualCompare, 0 };
static OpMatch OP_And[] = { '=', cmpToken_AndEqual, '&', cmpToken_AndCompare, 0 };
static OpMatch OP_Or[] = { '=', cmpToken_OrEqual, '|', cmpToken_OrCompare, 0 };
static OpMatch OP_Xor[] = { '=', cmpToken_XorEqual, 0 };
static OpMatch OP_Not[] = { '=', cmpToken_NotEqualCompare, 0 };
static OpMatch OP_Hash[] = { '#', cmpToken_SymbolJoin, 0 };


static cmpToken* cmpLexer_ConsumeOperator(cmpLexerCursor* cur, enum cmpTokenType type, const OpMatch* op_matches)
{
	cmpU32 index = 0;
	char c;

	// Start the token off
	cmpToken* token;
	cmpError error = cmpToken_Create(&token, cur, type, 1);
	if (!cmpError_OK(&error))
	{
		cmpLexerCursor_SetError(cur, &error);
		return NULL;
	}

	// Consume initial character and inspect the next one
	cmpLexerCursor_ConsumeChar(cur);
	c = cmpLexerCursor_PeekChar(cur, 0);

	// Run the next character against a series of matches to see if it's an operator token
	while (op_matches[index].type != 0)
	{
		if (c == op_matches[index].c)
		{
			token->length = 2;
			cmpLexerCursor_ConsumeChar(cur);
			return token;
		}
		index++;
	}

	// No match found, it's just a single character token
	return token;
}


static cmpToken* cmpLexer_ConsumeCharacter(cmpLexerCursor* cur, enum cmpTokenType type)
{
	// Create a single character token
	cmpToken* token;
	cmpError error = cmpToken_Create(&token, cur, type, 1);
	if (!cmpError_OK(&error))
	{
		cmpLexerCursor_SetError(cur, &error);
		return NULL;
	}
	cmpLexerCursor_ConsumeChar(cur);
	return token;
}


static cmpToken* cmpLexer_ConsumeEOL(cmpLexerCursor* cur, enum cmpTokenType type)
{
	// Create a single character token
	cmpToken* token;
	cmpError error = cmpToken_Create(&token, cur, type, 1);
	if (!cmpError_OK(&error))
	{
		cmpLexerCursor_SetError(cur, &error);
		return NULL;
	}

	// After the token is created so that the token correctly points to the EOL character
	// Before the character is consumed so that the line's character position is recorded correctly
	cmpLexerCursor_IncLine(cur);

	cmpLexerCursor_ConsumeChar(cur);
	return token;
}


cmpToken* cmpLexer_ConsumeToken(cmpLexerCursor* cur)
{
	char c;
	cmpError error;
	cmpToken* token;

	// State for some of the lexer predicates
	char last_c = 0;

	// Read the current character and return an empty token at stream end
	c = cmpLexerCursor_PeekChar(cur, 0);
	if (c == EOF)
		return NULL;

	switch (c)
	{
		// Keep whitespace for rewriting
		case ' ':
		case '\t':
		case '\v':
		case '\f':
		case '\r':
			return cmpLexer_ConsumeTokenPred(cur, cmpToken_Whitespace, 1, cmpLexer_IsWhitespace, NULL);

		// Mark EOL only for identifying the end of a pre-processor directive
		case '\n':
			return cmpLexer_ConsumeEOL(cur, cmpToken_EOL);

		// Structural single character tokens
		case cmpToken_LBrace:
		case cmpToken_RBrace:
		case cmpToken_Comma:
		case cmpToken_LBracket:
		case cmpToken_RBracket:
		case cmpToken_LSqBracket:
		case cmpToken_RSqBracket:
		case cmpToken_Colon:
		case cmpToken_SemiColon:
		case cmpToken_Period:
		case cmpToken_Question:
		case cmpToken_Tilde:
			return cmpLexer_ConsumeCharacter(cur, (enum cmpTokenType)c);

		// Single/double character operators
		case cmpToken_LAngle:
			return cmpLexer_ConsumeOperator(cur, cmpToken_LAngle, OP_LAngle);
		case cmpToken_RAngle:
			return cmpLexer_ConsumeOperator(cur, cmpToken_RAngle, OP_RAngle);
		case cmpToken_Plus:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Plus, OP_Plus);
		case cmpToken_Minus:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Minus, OP_Minus);
		case cmpToken_Asterisk:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Asterisk, OP_Asterisk);
		case cmpToken_Modulo:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Modulo, OP_Modulo);
		case cmpToken_Equals:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Equals, OP_Equals);
		case cmpToken_And:
			return cmpLexer_ConsumeOperator(cur, cmpToken_And, OP_And);
		case cmpToken_Or:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Or, OP_Or);
		case cmpToken_Xor:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Xor, OP_Xor);
		case cmpToken_Not:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Not, OP_Not);
		case cmpToken_Hash:
			return cmpLexer_ConsumeOperator(cur, cmpToken_Hash, OP_Hash);

		// Comments or divide
		case '/':
			if (cmpLexerCursor_PeekChar(cur, 1) == '*')
				return cmpLexer_ConsumeTokenPred(cur, cmpToken_Comment, 2, cmpLexer_IsCComment, &last_c);
			if (cmpLexerCursor_PeekChar(cur, 1) == '/')
				return cmpLexer_ConsumeTokenPred(cur, cmpToken_Comment, 2, cmpLexer_IsCppComment, NULL);
			return cmpLexer_ConsumeOperator(cur, cmpToken_Divide, OP_Divide);

		case '"':
			return cmpLexer_ConsumeTokenPred(cur, cmpToken_String, 1, cmpLexer_IsString, NULL);

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return cmpLexer_ConsumeTokenPred(cur, cmpToken_Number, 1, cmpLexer_IsNumber, NULL);

		// Symbol tokens
		case '_':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
			token = cmpLexer_ConsumeTokenPred(cur, cmpToken_Symbol, 1, cmpLexer_IsSymbol, NULL);
			cmpLexer_IdentifyKeywordTokens(token);
			return token;

		default:
			error = cmpError_Create("Unexpected character '%x'(%c)", c, c);
			cmpLexerCursor_SetError(cur, &error);
			return NULL;
	}
}



// =====================================================================================================
// cmpParserCursor
// =====================================================================================================



struct cmpParserCursor
{
	// Pointer to the token list
	cmpToken* first_token;

	// Current position in the token array
	cmpToken* cur_token;

	// Current line number, as read from the latest token
	cmpU32 line;

	// Last error encountered
	cmpError error;
};


cmpError cmpParserCursor_Create(cmpParserCursor** cursor, cmpToken* first_token)
{
	assert(cursor != NULL);

	// Allocate the container
	*cursor = malloc(sizeof(cmpParserCursor));
	if (*cursor == NULL)
		return cmpError_Create("malloc(cmpParserCursor) failed");

	// Set defaults
	(*cursor)->first_token = first_token;
	(*cursor)->cur_token = first_token;
	(*cursor)->line = 0;
	(*cursor)->error = cmpError_CreateOK();

	return cmpError_CreateOK();
}


void cmpParserCursor_Destroy(cmpParserCursor* cursor)
{
	assert(cursor != NULL);
	free(cursor);
}


cmpU32 cmpParserCursor_Line(cmpParserCursor* cursor)
{
	assert(cursor != NULL);
	return cursor->line;
}


cmpError cmpParserCursor_Error(cmpParserCursor* cursor)
{
	assert(cursor != NULL);
	return cursor->error;
}


static cmpToken* cmpParserCursor_PeekToken(cmpParserCursor* cursor, cmpU32 lookahead)
{
	cmpToken* token;

	assert(cursor != NULL);

	// Seek to the lookahead token
	token = cursor->cur_token;
	while (lookahead && token != NULL)
	{
		token = token->next;
		lookahead--;
	}

	if (token != NULL)
	{
		cursor->line = token->line;
		return token;
	}

	return NULL;
}


static cmpToken* cmpParserCursor_ConsumeToken(cmpParserCursor* cursor)
{
	cmpToken* token = cmpParserCursor_PeekToken(cursor, 0);
	if (token != NULL)
		cursor->cur_token = token->next;
	return token;
}


static void cmpParserCursor_SetError(cmpParserCursor* cursor, const cmpError* error)
{
	assert(cursor != NULL);
	cursor->error = *error;
}



// =====================================================================================================
// cmpNodeType
// =====================================================================================================



const char* cmpNodeType_Name(enum cmpNodeType type)
{
	switch (type)
	{
		case cmpNode_None: return "cmpNode_None";
		case cmpNode_PPDirective: return "cmpNode_PPDirective";
		case cmpNode_Statement: return "cmpNode_Statement";
		case cmpNode_StatementBlock: return "cmpNode_StatementBlock";
		case cmpNode_FunctionDefn: return "cmpNode_FunctionDefn";
		case cmpNode_FunctionDecl: return "cmpNode_FunctionDecl";
		case cmpNode_FunctionParams: return "cmpNode_FunctionParams";
		case cmpNode_StructDefn: return "cmpNode_StructDefn";
		case cmpNode_StructDecl: return "cmpNode_StructDecl";
		case cmpNode_StructTag: return "cmpNode_StructTag";
		case cmpNode_StructName: return "cmpNode_StructName";
		case cmpNode_Typedef: return "cmpNode_Typedef";
		case cmpNode_Token: return "cmpNode_Token";
		case cmpNode_UserTokens: return "cmpNode_UserTokens";
	}

	return "<INTERNAL-ERROR> No node name found";
}



// =====================================================================================================
// cmpNode
// =====================================================================================================



cmpError cmpNode_CreateEmpty(cmpNode** node)
{
	assert(node != NULL);

	// Allocate the container
	*node = malloc(sizeof(cmpNode));
	if (*node == NULL)
		return cmpError_Create("malloc(cmpNode) failed");

	// Set defaults
	(*node)->type = cmpNode_None;
	(*node)->first_child = NULL;
	(*node)->last_child = NULL;
	(*node)->next_sibling = NULL;
	(*node)->first_token = NULL;
	(*node)->last_token = NULL;

	return cmpError_CreateOK();
}


cmpError cmpNode_Create(cmpNode** node, enum cmpNodeType type, cmpParserCursor* cur)
{
	cmpError error = cmpNode_CreateEmpty(node);
	if (!cmpError_OK(&error))
		return error;

	// Override defaults
	(*node)->type = type;
	(*node)->first_token = cmpParserCursor_PeekToken(cur, 0);
	(*node)->last_token = (*node)->first_token;
	return cmpError_CreateOK();
}


void cmpNode_Destroy(cmpNode* node)
{
	assert(node != NULL);

	// Release children
	while (node->first_child != NULL)
	{
		cmpNode* next_sibling = node->first_child->next_sibling;
		cmpNode_Destroy(node->first_child);
		node->first_child = next_sibling;
	}

	free(node);
}


void cmpNode_AddChild(cmpNode* node, cmpNode* child_node)
{
	assert(node != NULL);

	// Add to the end of the list
	if (node->last_child == NULL)
	{
		node->first_child = child_node;
		node->last_child = child_node;
	}
	else
	{
		node->last_child->next_sibling = child_node;
		node->last_child = child_node;
	}
}



// =====================================================================================================
// cmpParser
// =====================================================================================================



static cmpNode* cmpParser_ConsumePPDirective(cmpParserCursor* cur)
{
	// Create the node
	cmpNode* node;
	cmpError error = cmpNode_Create(&node, cmpNode_PPDirective, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);

	// Loop looking for EOL to terminate the directive
	while (1)
	{
		cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
			break;
		if (token->type == cmpToken_EOL)
			break;

		cmpParserCursor_ConsumeToken(cur);
		node->last_token = token;
	}

	return node;
}


static cmpNode* cmpParser_ConsumeStatementBlock(cmpParserCursor* cur);


static cmpNode* cmpParser_ConsumeFunction(cmpParserCursor* cur, cmpNode* node)
{
	cmpU32 nb_brackets;

	// Create a node for the function parameters
	cmpNode* params_node;
	cmpError error = cmpNode_Create(&params_node, cmpNode_FunctionParams, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);
	cmpNode_AddChild(node, params_node);

	// Skip over all parameters
	nb_brackets = 1;
	while (1)
	{
		cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			cmpError error = cmpError_Create("Unexpected EOF when parsing function parameters");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}

		if (token->type == cmpToken_LBracket)
			nb_brackets++;

		// Allow multiple open/close brackets in the list (in the case of pre-processor use)
		if (token->type == cmpToken_RBracket && --nb_brackets == 0)
		{
			cmpParserCursor_ConsumeToken(cur);
			params_node->last_token = token;
			break;
		}

		cmpParserCursor_ConsumeToken(cur);
		params_node->last_token = token;
	}

	// Parse the function body, if it exists
	while (1)
	{
		cmpNode* child_node = cmpParser_ConsumeNode(cur);
		if (child_node == NULL)
		{
			cmpNode_Destroy(node);
			return NULL;
		}

		// Add everything encountered as a child
		cmpNode_AddChild(node, child_node);

		// Terminate as a function declaration
		if (child_node->type == cmpNode_Token && child_node->first_token->type == cmpToken_SemiColon)
		{
			node->type = cmpNode_FunctionDecl;
			break;
		}

		// Terminate as a function definition
		if (child_node->type == cmpNode_StatementBlock)
		{
			node->type = cmpNode_FunctionDefn;
			break;
		}
	}

	return node;
}


static cmpNode* cmpParser_ConsumeStatement(cmpParserCursor* cur)
{
	// Create the node
	cmpNode* node;
	cmpError error = cmpNode_Create(&node, cmpNode_Statement, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);

	// Skip over any leading symbols
	while (1)
	{
		cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			error = cmpError_Create("Unexpected EOF when parsing statement");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}
		if (token->type != cmpToken_Symbol && token->type != cmpToken_Whitespace)
			break;
		cmpParserCursor_ConsumeToken(cur);
		node->last_token = token;
	}

	// Token has not been consumed and is guaranteed to be non-null

	// Check to see if this is a function definition/declaration and grab its parameters
	if (node->first_token != node->last_token)
	{
		cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token->type == cmpToken_LBracket)
			return cmpParser_ConsumeFunction(cur, node);
	}

	// Now consume child statements if this is a definition
	while (1)
	{
		cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			error = cmpError_Create("Unexpected EOF when parsing statement");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}

		// No statement block encountered, this is a typical statement
		if (token->type == cmpToken_SemiColon)
			break;

		// Recurse into a statement block
		if (token->type == cmpToken_LBrace)
		{
			cmpNode* child_node = cmpParser_ConsumeStatementBlock(cur);
			if (child_node == NULL)
			{
				cmpNode_Destroy(node);
				return NULL;
			}

			// Add as the only child of the function
			cmpNode_AddChild(node, child_node);
			break;
		}

		cmpParserCursor_ConsumeToken(cur);
		node->last_token = token;
	}

	return node;
}


static cmpBool cmpParser_ConsumeTypedefStructName(cmpParserCursor* cur, cmpNode* node)
{
	cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
	if (token != NULL && token->type == cmpToken_Symbol)
	{
		cmpNode* child_node;
		cmpError error = cmpNode_Create(&child_node, cmpNode_StructName, cur);
		if (!cmpError_OK(&error))
		{
			cmpParserCursor_SetError(cur, &error);
			return CMP_FALSE;
		}

		cmpParserCursor_ConsumeToken(cur);
		cmpNode_AddChild(node, child_node);
		node->type = cmpNode_StructDecl;
		return CMP_TRUE;
	}

	return CMP_FALSE;
}


static cmpNode* cmpParser_ConsumeStruct(cmpParserCursor* cur)
{
	cmpToken* next_token;
	cmpBool name_is_tag = CMP_FALSE;

	// Create the node
	cmpNode* node;
	cmpError error = cmpNode_Create(&node, cmpNode_StructDefn, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);

	// Input can either be "struct" or "typedef struct" so consume the struct
	next_token = cmpParserCursor_PeekToken(cur, 0);
	if (next_token != NULL && next_token->type == cmpToken_Struct)
	{
		cmpParserCursor_ConsumeToken(cur);
		node->last_token = next_token;
		name_is_tag = CMP_TRUE;
	}

	// Consume tag or name
	next_token = cmpParserCursor_PeekToken(cur, 0);
	if (next_token != NULL && next_token->type == cmpToken_Symbol)
	{
		cmpNode* child_node;

		if (name_is_tag)
		{
			// typedef struct TAG
			error = cmpNode_Create(&child_node, cmpNode_StructTag, cur);
		}
		else
		{
			// struct NAME
			error = cmpNode_Create(&child_node, cmpNode_StructName, cur);
		}

		if (!cmpError_OK(&error))
		{
			cmpParserCursor_SetError(cur, &error);
			return NULL;
		}

		cmpParserCursor_ConsumeToken(cur);
		cmpNode_AddChild(node, child_node);
	}

	// If a symbol follows, this is a "typedef struct" forward declaration
	if (name_is_tag && cmpParser_ConsumeTypedefStructName(cur, node))
		return node;

	// Parse the struct body, if it exists
	while (1)
	{
		cmpNode* child_node = cmpParser_ConsumeNode(cur);
		if (child_node == NULL)
		{
			cmpNode_Destroy(node);
			return NULL;
		}

		// Add everything encountered as a child
		cmpNode_AddChild(node, child_node);

		// Terminate as a structure declaration
		if (child_node->type == cmpNode_Token && child_node->first_token->type == cmpToken_SemiColon)
		{
			node->type = cmpNode_StructDecl;
			break;
		}

		// Terminate as a struct definition
		if (child_node->type == cmpNode_StatementBlock)
		{
			node->type = cmpNode_StructDefn;
			break;
		}
	}

	// If this is a "typedef struct", try to consume a name
	if (name_is_tag)
		cmpParser_ConsumeTypedefStructName(cur, node);

	return node;
}


static cmpNode* cmpParser_ConsumeTypedef(cmpParserCursor* cur)
{
	cmpNode* node;
	cmpError error;

	// Redirect as a struct if this is "typedef struct"
	cmpToken* next_token = cmpParserCursor_PeekToken(cur, 1);
	if (next_token != NULL && next_token->type == cmpToken_Struct)
		return cmpParser_ConsumeStruct(cur);

	// Create the node
	error = cmpNode_Create(&node, cmpNode_Typedef, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);

	// Consume all tokens until the semi-colon
	while (1)
	{
		cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			error = cmpError_Create("Unexpected EOF when parsing typedef");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}

		if (token->type == cmpToken_SemiColon)
			break;

		cmpParserCursor_ConsumeToken(cur);
		node->last_token = token;
	}

	return node;
}


static cmpNode* cmpParser_ConsumeStatementBlock(cmpParserCursor* cur)
{
	// Create the node
	cmpNode* node;
	cmpError error = cmpNode_Create(&node, cmpNode_StatementBlock, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);

	// Loop, processing child statements within the block
	while (1)
	{
		cmpNode* child_node = cmpParser_ConsumeNode(cur);
		if (child_node == NULL)
		{
			error = cmpError_Create("Unexpected EOF when parsing statement block");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}

		// Add to the parent
		cmpNode_AddChild(node, child_node);

		// Look for terminating right brace
		if (child_node->type == cmpNode_Token && child_node->first_token->type == cmpToken_RBrace)
			break;
	}

	return node;
}


static cmpNode* cmpParser_ConsumeToken(cmpParserCursor* cur)
{
	// Create the node
	cmpNode* node;
	cmpError error = cmpNode_Create(&node, cmpNode_Token, cur);
	if (!cmpError_OK(&error))
	{
		cmpParserCursor_SetError(cur, &error);
		return NULL;
	}
	cmpParserCursor_ConsumeToken(cur);
	return node;
}


cmpNode* cmpParser_ConsumeNode(cmpParserCursor* cur)
{
	cmpError error;
	cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
	if (token == NULL)
		return NULL;

	switch (token->type)
	{
		// Tokens that need to be present in the AST for accurate rewriting
		case cmpToken_SemiColon:
		case cmpToken_Comment:
		case cmpToken_EOL:
		case cmpToken_Whitespace:
		case cmpToken_RBrace:
			return cmpParser_ConsumeToken(cur);

		// Tokens that create semantic nodes
		case cmpToken_Hash:
			return cmpParser_ConsumePPDirective(cur);
		case cmpToken_Typedef:
			return cmpParser_ConsumeTypedef(cur);
		case cmpToken_Struct:
			return cmpParser_ConsumeStruct(cur);
		case cmpToken_Symbol:
			return cmpParser_ConsumeStatement(cur);
		case cmpToken_LBrace:
			return cmpParser_ConsumeStatementBlock(cur);

		default:
			error = cmpError_Create("Unexpected token '%s'", cmpTokenType_Name(token->type));
			cmpParserCursor_SetError(cur, &error);
			return NULL;
	}

	return NULL;
}


void cmpParser_LogNodes(const cmpNode* node, cmpU32 depth)
{
	while (node != 0)
	{
		printf("%.*s[%d] %s\n", depth, "                    ", node->type, cmpNodeType_Name(node->type));
		cmpParser_LogNodes(node->first_child, depth + 1);
		node = node->next_sibling;
	}
}
