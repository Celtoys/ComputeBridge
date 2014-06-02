
#include "ComputeProcessor.h"

#include <cassert>


ComputeProcessor::ComputeProcessor()
	: m_MemoryFile(0)
	, m_LexerCursor(0)
	, m_ParserCursor(0)
	, m_FirstToken(0)
	, m_LastToken(0)
	, m_RootNode(0)
{
}


ComputeProcessor::~ComputeProcessor()
{
	// Destroy all tokens
	while (m_FirstToken != 0)
	{
		cmpToken* next = m_FirstToken->next;
		cmpToken_Destroy(m_FirstToken);
		m_FirstToken = next;
	}

	// Destroy all nodes
	if (m_RootNode != 0)
		cmpNode_Destroy(m_RootNode);

	// Destroy all parser objects
	if (m_ParserCursor != 0)
		cmpParserCursor_Destroy(m_ParserCursor);
	if (m_LexerCursor != 0)
		cmpLexerCursor_Destroy(m_LexerCursor);
	if (m_MemoryFile != 0)
		cmpMemoryFile_Destroy(m_MemoryFile);
}


bool ComputeProcessor::ParseFile(const char* filename, bool verbose)
{
	// Open the input file
	if (cmpError error = cmpMemoryFile_Create(&m_MemoryFile, filename))
	{
		printf("Error opening input file: %s\n\n", cmpError_Text(&error));
		return false;
	}

	// Build a list of tokens
	if (cmpError error = cmpLexerCursor_Create(&m_LexerCursor, cmpMemoryFile_Data(m_MemoryFile), cmpMemoryFile_Size(m_MemoryFile)))
	{
		printf("Error creating lexer cursor: %s\n\n", cmpError_Text(&error));
		return false;
	}
	while (cmpToken* token = cmpLexer_ConsumeToken(m_LexerCursor))
	{
		cmpToken_AddToList(&m_FirstToken, &m_LastToken, token);
		if (verbose)
			printf("[0x%2x] %s %d\n", token->type, cmpTokenType_Name(token->type), token->length);
	}

	// Print any lexer errors
	if (cmpError error = cmpLexerCursor_Error(m_LexerCursor))
	{
		printf("%s(%d): %s\n", filename, cmpLexerCursor_Line(m_LexerCursor), cmpError_Text(&error));
		return false;
	}

	cmpError error = cmpNode_CreateEmpty(&m_RootNode);
	if (!cmpError_OK(&error))
	{
		printf("Error: %s\n", error.text);
		return false;
	}

	// Build a list of parser nodes
	if (cmpError error = cmpParserCursor_Create(&m_ParserCursor, m_FirstToken))
	{
		printf("Error creating parser cursor: %s\n\n", cmpError_Text(&error));
		return false;
	}
	while (cmpNode* node = cmpParser_ConsumeNode(m_ParserCursor))
		cmpNode_AddChild(m_RootNode, node);

	if (verbose)
		cmpParser_LogNodes(m_RootNode, 0);

	// Print any parser errors
	if (cmpError error = cmpParserCursor_Error(m_ParserCursor))
	{
		printf("%s(%d): %s\n",filename, cmpParserCursor_Line(m_ParserCursor), cmpError_Text(&error));
		return false;
	}

	return true;
}


namespace
{
	void VisitNode(cmpNode* node, INodeVisitor* visitor)
	{
		assert(visitor != 0);
		assert(node != 0);
		visitor->Visit(*node);

		for (cmpNode* child = node->first_child; child != 0; child = child->next_sibling)
			VisitNode(child, visitor);
	}
}


void ComputeProcessor::VisitNodes(INodeVisitor* visitor)
{
	assert(visitor != 0);
	VisitNode(m_RootNode, visitor);
}


TokenIterator::TokenIterator(cmpNode& node)
	: first_token(node.first_token)
	, last_token(node.last_token ? node.last_token->next : 0)
	, token(first_token)
{
}


cmpToken* TokenIterator::SkipWhitespace()
{
	// Skip all whitespace/EOL tokens
	while (
		token != last_token &&
		(token->type == cmpToken_Whitespace ||
		 token->type == cmpToken_EOL))
		token = token->next;

	// Return NULL if skipped over all tokens
	return token == last_token ? token : 0;
}


TokenIterator::operator bool () const
{
	return token != last_token;
}


TokenIterator& TokenIterator::operator ++ ()
{
	if (token != last_token)
		token = token->next;
	else
		token = 0;

	return *this;
}
