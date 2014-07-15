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


#ifndef _MSC_VER
#include <cstring>
#include <strings.h>
#include <cstdio>
#define strcmpi strcasecmp
#endif


void PrintHeader()
{
	printf("cbpp Compute Bridge Preprocessor Copyright 2014 Celtoys Ltd\n");
	printf("Licensed under the Apache License, Version 2.0 \n");
}


void PrintUsage()
{
	printf("Usage: cbpp filename -target <cuda|opencl> [options]\n");
}


void PrintHelp()
{
	PrintUsage();
	printf("\nOptions are:\n\n");
	printf("   -noheader          Supress header\n");
	printf("   -verbose           Print logs detailing what cbpp is doing behind the scenes\n");
}


struct EmitFile : public INodeVisitor
{
	EmitFile(const char* filename)
		: fp(0)
		, last_error(cmpError_CreateOK())
	{
		fp = fopen(filename, "wb");
		if (fp == 0)
			last_error = cmpError_Create("Couldn't open file '%s' for writing", filename);
	}

	~EmitFile()
	{
		if (fp != 0)
			fclose(fp);
	}

	bool Visit(const ComputeProcessor&, cmpNode& node)
	{
		if (fp == 0)
			return false;

		for (TokenIterator i(node); i; ++i)
		{
			const cmpToken& token = *i.token;
			fprintf(fp, "%.*s", token.length, token.start);
		}

		return true;
	}

	FILE* fp;

	cmpError last_error;
};


int main(int argc, const char* argv[])
{
	// Build arguments object, expecting a filename
	Arguments args(argc, argv);
	if (args.Count() < 2)
	{
		PrintHeader();
		PrintUsage();
		printf("\nERROR: Not enough arguments, must be at least the input filename\n");
		return 1;
	}

	// Exit immediately on help request
	if (args.Have("-h"))
	{
		PrintHeader();
		PrintHelp();
		return 0;
	}

	// Grab the output filename
	std::string output_filename = args.GetProperty("-output");
	if (output_filename == "")
	{
		PrintHeader();
		PrintUsage();
		printf("\nERROR: No output filename specified\n");
		return 1;
	}

	// Suppress header?
	if (!args.Have("-noheader"))
		PrintHeader();

	ComputeProcessor processor(args);
	if (!processor.ParseFile())
		return 1;

	cmpError error = processor.ApplyTransforms();
	if (!cmpError_OK(&error))
		printf("%s\n", error.text);

	EmitFile emitter(output_filename.c_str());
	if (!processor.VisitNodes(&emitter))
	{
		printf("%s\n", emitter.last_error.text);
		return 1;
	}

	return 0;
}
