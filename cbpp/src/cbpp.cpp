
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

#include "ComputeProcessor.h"

#include <string>

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
		for (TokenIterator i(node); i; ++i)
		{
			const cmpToken& token = *i.token;
			fprintf(fp, "%.*s", token.length, token.start);
		}
	}

	FILE* fp;
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
	if (!processor.ParseFile(g_InputFilename.c_str(), g_Verbose))
		return 1;

	processor.ApplyTransforms();

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
