
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


#include <string>
#include <vector>
#include <cassert>

#include "../../lib/ComputeParser.h"


// stdout configuration
bool g_PrintHeader = true;
bool g_PrintHelp = false;
bool g_Verbose = false;

std::string g_InputFilename;
std::string g_OutputFilename;


#define LOG if (g_Verbose) printf

#ifndef _MSC_VER
#include <cstring>
#include <strings.h>
#include <cstdio>
#define strcmpi strcasecmp
#endif

const char* ParseArguments(int argc, const char* argv[])
{
	int i;

	for (i = 1; i < argc; i++)
	{
		const char* arg = argv[i];

		// Is this an option?
		if (arg[0] == '-')
		{
			if (!strcmpi(arg, "-h"))
			{
				g_PrintHelp = true;
			}

			else if (!strcmpi(arg, "-noheader"))
			{
				g_PrintHeader = false;
			}

			else if (!strcmpi(arg, "-verbose"))
			{
				g_Verbose = true;
			}

			else if (!strcmpi(arg, "-output") && i < argc - 1)
			{
				g_OutputFilename = argv[i + 1];
				i++;
			}
		}

		else
		{
			// Must be a filename
			g_InputFilename = arg;
		}
	}

	if (g_InputFilename[0] == 0)
		return "No input filename specified";

	return 0;
}


void PrintHeader()
{
	printf("cbpp Compute Bridge Preprocessor Copyright 2014 Celtoys Ltd\n");
	printf("Licensed under the Apache License, Version 2.0 \n");
}


void PrintUsage()
{
	printf("Usage: cbpp [options] filename\n");

	if (g_PrintHelp)
	{
		printf("\nOptions are:\n\n");
		printf("   -noheader          Supress header\n");
		printf("   -verbose           Print logs detailing what cbpp is doing behind the scenes\n");
	}
}


struct INodeVisitor
{
	virtual void Visit(cmpNode& node) = 0;
};


struct EmitFile : public INodeVisitor
{
	EmitFile(const char* filename)
		: fp(0)
	{
		fp = fopen(filename, "wb");
	}

	~EmitFile()
	{
		if (fp != 0)
			fclose(fp);
	}

	void Visit(cmpNode& node)
	{
		for (cmpU32 i = 0; i < node.nb_tokens; i++)
		{
			const cmpToken& token = node.start_token[i];
			fprintf(fp, "%.*s", token.length, token.start);
		}
	}

	FILE* fp;
};


class ComputeProcessor
{
public:
	ComputeProcessor()
		: m_MemoryFile(0)
		, m_LexerCursor(0)
		, m_ParserCursor(0)
	{
	}

	~ComputeProcessor()
	{
		for (std::size_t i = 0; i < m_Nodes.size(); i++)
			cmpNode_Destroy(m_Nodes[i]);

		if (m_ParserCursor != 0)
			cmpParserCursor_Destroy(m_ParserCursor);

		if (m_LexerCursor != 0)
			cmpLexerCursor_Destroy(m_LexerCursor);

		if (m_MemoryFile != 0)
			cmpMemoryFile_Destroy(m_MemoryFile);
	}

	bool Parse(const char* filename)
	{
		// Open the input file
		LOG("Opening file '%s'\n", filename);
		if (cmpError error = cmpMemoryFile_Create(&m_MemoryFile, filename))
		{
			printf("Error opening input file: %s\n\n", cmpError_Text(&error));
			return false;
		}

		// Build a list of tokens
		LOG("Running lexer\n");
		if (cmpError error = cmpLexerCursor_Create(&m_LexerCursor, cmpMemoryFile_Data(m_MemoryFile), cmpMemoryFile_Size(m_MemoryFile)))
		{
			printf("Error creating lexer cursor: %s\n\n", cmpError_Text(&error));
			return false;
		}
		while (cmpToken token = cmpLexer_ConsumeToken(m_LexerCursor))
		{
			m_Tokens.push_back(token);
			LOG("[0x%2x] %s %d\n", token.type, cmpTokenType_Name(token.type), token.length);
		}

		// Print any lexer errors
		if (cmpError error = cmpLexerCursor_Error(m_LexerCursor))
		{
			printf("%s(%d): %s\n", filename, cmpLexerCursor_Line(m_LexerCursor), cmpError_Text(&error));
			return false;
		}

		// Build a list of parser nodes
		LOG("Running parser\n");
		if (cmpError error = cmpParserCursor_Create(&m_ParserCursor, m_Tokens.data(), m_Tokens.size()))
		{
			printf("Error creating parser cursor: %s\n\n", cmpError_Text(&error));
			return false;
		}
		while (cmpNode* node = cmpParser_ConsumeNode(m_ParserCursor))
			m_Nodes.push_back(node);

		if (g_Verbose)
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

	void VisitNodes(INodeVisitor* visitor)
	{
		assert(visitor != 0);
		for (size_t i = 0; i < m_Nodes.size(); i++)
			VisitNode(m_Nodes[i], visitor);
	}

private:
	void VisitNode(cmpNode* node, INodeVisitor* visitor)
	{
		assert(visitor != 0);
		assert(node != 0);
		visitor->Visit(*node);

		for (cmpNode* child = node->first_child; child != 0; child = child->next_sibling)
			VisitNode(child, visitor);
	}

	// Parser runtime
	cmpMemoryFile* m_MemoryFile;
	cmpLexerCursor* m_LexerCursor;
	cmpParserCursor* m_ParserCursor;

	// Generated tokens and AST
	std::vector<cmpToken> m_Tokens;
	std::vector<cmpNode*> m_Nodes;
};


int main(int argc, const char* argv[])
{
	// Attempt to parse arguments
	if (const char* error = ParseArguments(argc, argv))
	{
		PrintHeader();
		printf("\nError parsing arguments: %s\n\n", error);
		PrintUsage();
		return 1;
	}

	// Print program information
	if (g_PrintHeader)
		PrintHeader();
	if (g_PrintHelp)
		PrintUsage();

	ComputeProcessor processor;
	if (!processor.Parse(g_InputFilename.c_str()))
		return 1;

	if (!g_OutputFilename.empty())
	{
		EmitFile emitter(g_OutputFilename.c_str());
		if (emitter.fp == 0)
		{
			printf("Couldn't write to output file %s\n", g_OutputFilename.c_str());
			return 1;
		}
		processor.VisitNodes(&emitter);
	}

	return 0;
}
