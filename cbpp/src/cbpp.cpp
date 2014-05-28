
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


#include <cstdio>
#include <cstdarg>
#include <string>
#include <vector>

#include "ComputeParser.h"


// stdout configuration
bool g_PrintHeader = true;
bool g_PrintHelp = false;
bool g_Verbose = false;

std::string g_InputFilename;


#define LOG if (g_Verbose) printf


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


class ComputeProcessor
{
public:
	ComputeProcessor()
		: m_MemoryFile(0)
		, m_LexerCursor(0)
		, m_ParserCursor(0)
	{
	}

	int Execute()
	{
		// Open the input file
		LOG("Opening file '%s'\n", g_InputFilename.c_str());
		cmpError error = cmpMemoryFile_Create(&m_MemoryFile, g_InputFilename.c_str());
		if (!cmpError_OK(&error))
		{
			printf("Error opening input file: %s\n\n", cmpError_Text(&error));
			return 1;
		}

		// Build a list of tokens
		LOG("Running lexer\n");
		error = cmpLexerCursor_Create(&m_LexerCursor, cmpMemoryFile_Data(m_MemoryFile), cmpMemoryFile_Size(m_MemoryFile));
		if (!cmpError_OK(&error))
		{
			printf("Error creating lexer cursor: %s\n\n", cmpError_Text(&error));
			return 1;
		}
		std::vector<cmpToken> tokens;
		while (cmpToken token = cmpLexer_ConsumeToken(m_LexerCursor))
		{
			tokens.push_back(token);
			LOG("[0x%2x] %s %d\n", token.type, cmpTokenType_Name(token.type), token.length);
		}

		// Print any lexer errors
		error = cmpLexerCursor_Error(m_LexerCursor);
		if (!cmpError_OK(&error))
		{
			printf("%s(%d): %s\n", g_InputFilename.c_str(), cmpLexerCursor_Line(m_LexerCursor), cmpError_Text(&error));
			return 1;
		}

		// Build a list of parser nodes
		LOG("Running parser\n");
		error = cmpParserCursor_Create(&m_ParserCursor, tokens.data(), tokens.size());
		if (!cmpError_OK(&error))
		{
			printf("Error creating parser cursor: %s\n\n", cmpError_Text(&error));
			return 1;
		}
		std::vector<cmpNode*> nodes;
		while (cmpNode* node = cmpParser_ConsumeNode(m_ParserCursor))
			nodes.push_back(node);

		if (g_Verbose)
		{
			for (size_t i = 0; i < nodes.size(); i++)
				cmpParser_LogNodes(nodes[i], 0);
		}

		for (std::size_t i = 0; i < nodes.size(); i++)
			delete nodes[i];

		// Print any parser errors
		error = cmpParserCursor_Error(m_ParserCursor);
		if (!cmpError_OK(&error))
		{
			printf("%s(%d): %s\n", g_InputFilename.c_str(), cmpLexerCursor_Line(m_LexerCursor), cmpError_Text(&error));
			return 1;
		}

		return 0;
	}

	~ComputeProcessor()
	{
		if (m_ParserCursor != 0)
			cmpParserCursor_Destroy(m_ParserCursor);

		if (m_LexerCursor != 0)
			cmpLexerCursor_Destroy(m_LexerCursor);

		if (m_MemoryFile != 0)
			cmpMemoryFile_Destroy(m_MemoryFile);
	}

private:
	cmpMemoryFile* m_MemoryFile;

	cmpLexerCursor* m_LexerCursor;

	cmpParserCursor* m_ParserCursor;
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
	return processor.Execute();
}