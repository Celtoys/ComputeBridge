
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
		case cmpToken_LAngle: return "cmpToken_LAngle";
		case cmpToken_RAngle: return "cmpToken_RAngle";
		case cmpToken_Colon: return "cmpToken_Colon";
		case cmpToken_SemiColon: return "cmpToken_SemiColon";
		case cmpToken_Hash: return "cmpToken_Hash";
		case cmpToken_Plus: return "cmpToken_Plus";
		case cmpToken_Minus: return "cmpToken_Minus";
		case cmpToken_Asterisk: return "cmpToken_Asterisk";
		case cmpToken_Divide: return "cmpToken_Divide";
		case cmpToken_Modulo: return "cmpToken_Modulo";
		case cmpToken_Period: return "cmpToken_Period";
		case cmpToken_Equals: return "cmpToken_Equals";
		case cmpToken_And: return "cmpToken_And";
		case cmpToken_Or: return "cmpToken_Or";
		case cmpToken_Xor: return "cmpToken_Xor";
		case cmpToken_Tilde: return "cmpToken_Tilde";
		case cmpToken_Not: return "cmpToken_Not";
		case cmpToken_Question: return "cmpToken_Question";
		case cmpToken_Comment: return "cmpToken_Comment";
		case cmpToken_String: return "cmpToken_String";
		case cmpToken_Number: return "cmpToken_Number";
		case cmpToken_Symbol: return "cmpToken_Symbol";
		case cmpToken_EOL: return "cmpToken_EOL";
	}

	return "<INTERNAL-ERROR> No token name found";
}



// =====================================================================================================
// cmpToken
// =====================================================================================================



static cmpToken cmpToken_CreateEmpty()
{
	cmpToken token;
	token.type = cmpToken_None;
	token.start = NULL;
	token.length = 0;
	token.line = 0;
	return token;
}


cmpToken cmpToken_Create(cmpLexerCursor* cur, enum cmpTokenType type, cmpU32 length)
{
	cmpToken token;
	token.type = type;
	token.start = cmpLexerCursor_PeekChars(cur, 0);
	token.length = length;
	token.line = cur->line;
	return token;
}



// =====================================================================================================
// cmpLexer
// =====================================================================================================



typedef cmpBool (*cmpLexer_Predicate)(cmpLexerCursor* cur, cmpToken* token, char c, void* state);


static cmpToken cmpLexer_ConsumeTokenPred(cmpLexerCursor* cur, enum cmpTokenType type, cmpU32 initial_length, cmpLexer_Predicate p, void* state)
{
	// Start the token off
	cmpToken token = cmpToken_Create(cur, type, initial_length);
	cmpLexerCursor_ConsumeChars(cur, initial_length);

	// Scan all characters until EOF or the predicate says so
	while (1)
	{
		char c = cmpLexerCursor_PeekChar(cur, 0);
		if (c == EOF)
			break;

		if (!p(cur, &token, c, state))
			break;

		cmpLexerCursor_ConsumeChar(cur);
		token.length++;
	}

	return token;
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
	cmpBool* had_period = (cmpBool*)state;
	if (c == '.')
	{
		if (*had_period)
			return CMP_FALSE;
		return *had_period = CMP_TRUE;
	}
	return isdigit(c) ? CMP_TRUE : CMP_FALSE;
}



static cmpBool cmpLexer_IsSymbol(cmpLexerCursor* cur, cmpToken* token, char c, void* state)
{
	return (c == '_' || isalpha(c) || isdigit(c)) ? CMP_TRUE : CMP_FALSE;
}


cmpToken cmpLexer_ConsumeToken(cmpLexerCursor* cur)
{
	char c;
	cmpError error;
	cmpToken token;

	// State for some of the lexer predicates
	cmpBool had_period;
	char last_c;

start:

	had_period = CMP_FALSE;
	last_c = 0;

	// Read the current character and return an empty token at stream end
	c = cmpLexerCursor_PeekChar(cur, 0);
	if (c == EOF)
		return cmpToken_CreateEmpty();

	switch (c)
	{
		// Branch to the start only if it's a whitespace (the least-common case)
		case ' ':
		case '\t':
		case '\v':
		case '\f':
		case '\r':
			cmpLexerCursor_ConsumeChar(cur);
			goto start;

		// Mark EOL only for identifying the end of a pre-processor directive
		case '\n':
			token = cmpToken_Create(cur, cmpToken_EOL, 1); 
			cmpLexerCursor_IncLine(cur);
			cmpLexerCursor_ConsumeChar(cur);
			return token;

		// Structural single character tokens
		case cmpToken_LBrace:
		case cmpToken_RBrace:
		case cmpToken_Comma:
		case cmpToken_LBracket:
		case cmpToken_RBracket:
		case cmpToken_LSqBracket:
		case cmpToken_RSqBracket:
		case cmpToken_LAngle:
		case cmpToken_RAngle:
		case cmpToken_Colon:
		case cmpToken_SemiColon:
		case cmpToken_Hash:
		case cmpToken_Plus:
		case cmpToken_Minus:
		case cmpToken_Asterisk:
		case cmpToken_Modulo:
		case cmpToken_Period:
		case cmpToken_Equals:
		case cmpToken_And:
		case cmpToken_Or:
		case cmpToken_Xor:
		case cmpToken_Tilde:
		case cmpToken_Not:
		case cmpToken_Question:
			token = cmpToken_Create(cur, (enum cmpTokenType)c, 1);
			cmpLexerCursor_ConsumeChar(cur);
			return token;

		// Comments or divide
		case '/':
			if (cmpLexerCursor_PeekChar(cur, 1) == '*')
				return cmpLexer_ConsumeTokenPred(cur, cmpToken_Comment, 2, cmpLexer_IsCComment, &last_c);
			if (cmpLexerCursor_PeekChar(cur, 1) == '/')
				return cmpLexer_ConsumeTokenPred(cur, cmpToken_Comment, 2, cmpLexer_IsCppComment, NULL);

			token = cmpToken_Create(cur, cmpToken_Divide, 1);
			cmpLexerCursor_ConsumeChar(cur);
			return token;

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
			return cmpLexer_ConsumeTokenPred(cur, cmpToken_Number, 1, cmpLexer_IsNumber, &had_period);

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
			return cmpLexer_ConsumeTokenPred(cur, cmpToken_Symbol, 1, cmpLexer_IsSymbol, NULL);

		default:
			error = cmpError_Create("Unexpected character '%x'(%c)", c, c);
			cmpLexerCursor_SetError(cur, &error);
			return cmpToken_CreateEmpty();
	}
}



// =====================================================================================================
// cmpParserCursor
// =====================================================================================================



struct cmpParserCursor
{
	// Pointer to the token array
	const cmpToken* tokens;
	cmpU32 nb_tokens;

	// Current position in the token array
	cmpU32 position;

	// Current line number, as read from the latest token
	cmpU32 line;

	// Last error encountered
	cmpError error;
};


cmpError cmpParserCursor_Create(cmpParserCursor** cursor, const cmpToken* tokens, cmpU32 nb_tokens)
{
	assert(cursor != NULL);

	// Allocate the container
	*cursor = malloc(sizeof(cmpParserCursor));
	if (*cursor == NULL)
		return cmpError_Create("malloc(cmpParserCursor) failed");

	// Set defaults
	(*cursor)->tokens = tokens;
	(*cursor)->nb_tokens = nb_tokens;
	(*cursor)->position = 0;
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


static const cmpToken* cmpParserCursor_PeekToken(cmpParserCursor* cursor, cmpU32 lookahead)
{
	assert(cursor != NULL);

	// Nothing to read at EOF
	if (cursor->position + lookahead < cursor->nb_tokens)
	{
		const cmpToken* token = cursor->tokens + cursor->position + lookahead;
		cursor->line = token->line;
		return token;
	}

	return NULL;
}


static const cmpToken* cmpParserCursor_ConsumeToken(cmpParserCursor* cursor)
{
	const cmpToken* token = cmpParserCursor_PeekToken(cursor, 0);
	if (cursor->position < cursor->nb_tokens)
		cursor->position++;
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
	}

	return "<INTERNAL-ERROR> No node name found";
}



// =====================================================================================================
// cmpNode
// =====================================================================================================



static cmpError cmpNode_Create(cmpNode** node, enum cmpNodeType type, cmpParserCursor* cur)
{
	assert(node != NULL);

	// Allocate the container
	*node = malloc(sizeof(cmpNode));
	if (*node == NULL)
		return cmpError_Create("malloc(cmpNode) failed");

	// Set defaults
	(*node)->type = type;
	(*node)->first_child = NULL;
	(*node)->last_child = NULL;
	(*node)->next_sibling = NULL;
	(*node)->start_token = cmpParserCursor_PeekToken(cur, 0);
	(*node)->nb_tokens = 1;

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


static void cmpNode_AddChild(cmpNode* node, cmpNode* child_node)
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
		const cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
			break;
		if (token->type == cmpToken_EOL)
			break;

		cmpParserCursor_ConsumeToken(cur);
		node->nb_tokens++;
	}

	return node;
}


static cmpNode* cmpParser_ConsumeStatementBlock(cmpParserCursor* cur);


static cmpBool cmpParser_ConsumeFunction(cmpParserCursor* cur, cmpNode* node)
{
	// Skip over all parameters
	cmpU32 nb_brackets = 0;
	while (1)
	{
		const cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			cmpError error = cmpError_Create("Unexpected EOF when parsing function parameters");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return CMP_FALSE;
		}

		if (token->type == cmpToken_LBracket)
			nb_brackets++;

		// Allow multiple open/close brackets in the list (in the case of pre-processor use)
		if (token->type == cmpToken_RBracket && --nb_brackets == 0)
		{
			cmpParserCursor_ConsumeToken(cur);
			node->nb_tokens++;
			break;
		}

		cmpParserCursor_ConsumeToken(cur);
		node->nb_tokens++;
	}

	// Parse the function body, if it exists
	while (1)
	{
		cmpNode* child_node = cmpParser_ConsumeNode(cur);
		if (child_node == NULL)
		{
			cmpNode_Destroy(node);
			return CMP_FALSE;
		}

		// Add everything encountered as a child
		cmpNode_AddChild(node, child_node);

		// Terminate as a function declaration
		if (child_node->type == cmpNode_Token && child_node->start_token->type == cmpToken_SemiColon)
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

	return CMP_TRUE;
}


static cmpNode* cmpParser_ConsumeStatement(cmpParserCursor* cur)
{
	const cmpToken* t0;
	const cmpToken* t1;

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
		const cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			error = cmpError_Create("Unexpected EOF when parsing statement");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}
		if (token->type != cmpToken_Symbol)
			break;
		cmpParserCursor_ConsumeToken(cur);
		node->nb_tokens++;
	}

	// Token has not been consumed and is guaranteed to be non-null

	// Check to see if this is a function definition/declaration and grab its parameters
	if (node->nb_tokens > 1)
	{
		const cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token->type == cmpToken_LBracket)
		{
			if (!cmpParser_ConsumeFunction(cur, node))
				return NULL;
			return node;
		}
	}

	// Now consume child statements if this is a definition
	while (1)
	{
		const cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
		if (token == NULL)
		{
			error = cmpError_Create("Unexpected EOF when parsing statement");
			cmpParserCursor_SetError(cur, &error);
			cmpNode_Destroy(node);
			return NULL;
		}

		// If no statement block is encountered, change this to a declaration
		if (token->type == cmpToken_SemiColon)
		{
			cmpParserCursor_ConsumeToken(cur);
			break;
		}

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
		node->nb_tokens++;
	}

	// Hacky check to see if this is naming a struct
	t0 = cmpParserCursor_PeekToken(cur, 0);
	t1 = cmpParserCursor_PeekToken(cur, 1);
	if (t0 != NULL && t1 != NULL && t0->type == cmpToken_Symbol && t1->type == cmpToken_SemiColon)
	{
		cmpParserCursor_ConsumeToken(cur);
		cmpParserCursor_ConsumeToken(cur);
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
		if (child_node->type == cmpNode_Token && child_node->start_token->type == cmpToken_RBrace)
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
	const cmpToken* token = cmpParserCursor_PeekToken(cur, 0);
	if (token == NULL)
		return NULL;

	switch (token->type)
	{
		// Tokens that need to be present in the AST for accurate rewriting
		case cmpToken_SemiColon:
		case cmpToken_Comment:
		case cmpToken_EOL:
		case cmpToken_RBrace:
			return cmpParser_ConsumeToken(cur);

		// Pre-processor directives
		case cmpToken_Hash:
			return cmpParser_ConsumePPDirective(cur);

		// Statements
		case cmpToken_Symbol:
			return cmpParser_ConsumeStatement(cur);

		// Statement blocks
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
		printf("%.*s[%d] %s %d\n", depth, "                    ", node->type, cmpNodeType_Name(node->type), node->nb_tokens);
		cmpParser_LogNodes(node->first_child, depth + 1);
		node = node->next_sibling;
	}
}
