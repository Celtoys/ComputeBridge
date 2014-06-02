
#include "ComputeProcessor.h"

#include <cassert>


ComputeProcessor::ComputeProcessor()
	: m_MemoryFile(0)
	, m_LexerCursor(0)
	, m_ParserCursor(0)
{
}


ComputeProcessor::~ComputeProcessor()
{
	// Destroy all nodes
	for (std::size_t i = 0; i < m_Nodes.size(); i++)
		cmpNode_Destroy(m_Nodes[i]);

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
		m_Tokens.push_back(*token);
		if (verbose)
			printf("[0x%2x] %s %d\n", token->type, cmpTokenType_Name(token->type), token->length);
		cmpToken_Destroy(token);
	}

	// Print any lexer errors
	if (cmpError error = cmpLexerCursor_Error(m_LexerCursor))
	{
		printf("%s(%d): %s\n", filename, cmpLexerCursor_Line(m_LexerCursor), cmpError_Text(&error));
		return false;
	}

	// Build a list of parser nodes
	if (cmpError error = cmpParserCursor_Create(&m_ParserCursor, m_Tokens.data(), m_Tokens.size()))
	{
		printf("Error creating parser cursor: %s\n\n", cmpError_Text(&error));
		return false;
	}
	while (cmpNode* node = cmpParser_ConsumeNode(m_ParserCursor))
		m_Nodes.push_back(node);

	if (verbose)
	{
		for (size_t i = 0; i < m_Nodes.size(); i++)
			cmpParser_LogNodes(m_Nodes[i], 0);
	}

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
	for (size_t i = 0; i < m_Nodes.size(); i++)
		VisitNode(m_Nodes[i], visitor);
}


TokenIterator::TokenIterator(cmpNode& node)
	: first_token(node.start_token)
	, last_token(first_token + node.nb_tokens)
	, token(first_token)
{
}


const cmpToken* TokenIterator::SkipWhitespace()
{
	// Skip all whitespace/EOL tokens
	while (
		token < last_token &&
		token->type == cmpToken_Whitespace &&
		token->type == cmpToken_EOL)
		token++;

	// Return NULL if skipped over all tokens
	return token < last_token ? token : 0;
}


TokenIterator::operator bool () const
{
	return token < last_token;
}


TokenIterator& TokenIterator::operator ++ ()
{
	if (token < last_token)
		token++;
	else
		token = 0;

	return *this;
}
