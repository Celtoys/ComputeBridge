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

#include "Base.h"
#include "ComputeProcessor.h"
#include "fcpp.h"

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
	printf("   -output <path>     Generated file output path\n");
	printf("   -i <path>          Specify additional include search path\n");
}


struct EmitFile : public INodeVisitor
{
	EmitFile(const char* filename)
		: last_error(cmpError_CreateOK())
	{
		if (!Open(file, filename, "wb"))
			last_error = cmpError_Create("Couldn't open file '%s' for writing", filename);
	}

	bool Visit(const ComputeProcessor&, cmpNode& node)
	{
		if (file.fp == 0)
			return false;

		for (TokenIterator i(node); i; ++i)
		{
			const cmpToken& token = *i.token;
			fprintf(file.fp, "%.*s", token.length, token.start);
		}

		return true;
	}

	File file;

	cmpError last_error;
};


struct PPInfo
{
	PPInfo(const std::vector<char>& in_data)
		: in_data(in_data)
		, read_pos(0)
	{
	}

	const std::vector<char>& in_data;
	size_t read_pos;

	std::vector<char> out_data;
};


char* PPInput(char* buffer, int size, void* user_data)
{
	PPInfo& pp_info(*(PPInfo*)user_data);

	// Read until EOF/EOL or not enough output bytes left
	int write_pos = 0;
	while (write_pos < size - 1)
	{
		// Check for EOF
		if (pp_info.read_pos >= pp_info.in_data.size())
			return NULL;

		char c = pp_info.in_data[pp_info.read_pos++];

		// Don't add CR to the buffer as fcpp doesn't recognise that as white-space and will error
		// All characters other than that go into the buffer (including EOL)
		if (c != '\r')
			buffer[write_pos++] = c;

		if (c == '\n')
			break;
	}

	// NULL terminate
	buffer[write_pos] = 0;

	return buffer;
}


void PPOutput(int c, void* user_data)
{
	PPInfo& pp_info(*(PPInfo*)user_data);
	pp_info.out_data.push_back(c);
}


void PPError(void* user_data, char* format, va_list args)
{
	vfprintf(stdout, format, args);
}


std::vector<char> PreProcessFile(const Arguments& args, const std::string& filename, const std::vector<char>& in_data)
{
	fppTag tags[20];
	fppTag* tagptr = tags;
	std::string include;

	// Create/set the user data
	PPInfo pp_info(in_data);
	tagptr->tag = FPPTAG_USERDATA;
	tagptr->data = &pp_info;
	tagptr++;

	// Set the input function
	tagptr->tag = FPPTAG_INPUT;
	tagptr->data = PPInput;
	tagptr++;

	// Set the output function
	tagptr->tag = FPPTAG_OUTPUT;
	tagptr->data = PPOutput;
	tagptr++;

	// Set the error function
	tagptr->tag = FPPTAG_ERROR;
	tagptr->data = PPError;
	tagptr++;

	// Set the input filename
	tagptr->tag = FPPTAG_INPUT_NAME;
	tagptr->data = (void*)filename.data();
	tagptr++;

	// Don't display version information
	tagptr->tag = FPPTAG_SHOWVERSION;
	tagptr->data = (void*)FALSE;
	tagptr++;

	// Point to the end of the filename and scan back, looking for the first path separator
	const char* fptr = filename.data() + filename.length() - 1;
	while (fptr != filename && *fptr != '/' && *fptr != '\\')
		fptr--;

	// Was a path specified in the filename?
	if (fptr != filename)
	{
		int path_length = fptr - filename.data();
		include = std::string(filename, 0, path_length);

		// Replace back-slash with forward-slash as fcpp can't handle it
		for (size_t i = 0; i < include.length(); i++)
		{
			if (include[i] == '\\')
				include[i] = '/';
		}

		// Requirement for FPPTAG_INCLUDE_DIR
		include += '/';

		// Add the location of the filename as an include directory
		tagptr->tag = FPPTAG_INCLUDE_DIR;
		tagptr->data = (void*)include.data();
		tagptr++;
	}

	// Loop reading all include directories
	int nb_include_dirs = 0;
	while (true)
	{
		int index = args.GetIndexOf("-i", nb_include_dirs++);
		if (index == -1)
			break;

		// Add a tag for the include directory
		const std::string& include_dir = args[index];
		tagptr->tag = FPPTAG_INCLUDE_DIR;
		tagptr->data = (void*)include_dir.data();
		tagptr++;
	}

	// End the tag list
	tagptr->tag = FPPTAG_END;
	tagptr->data = 0;
	tagptr++;	

	fppPreProcess(tags);

	return pp_info.out_data;
}


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

	// Load the input file
	std::string input_filename = args[1];
	std::vector<char> input_file;
	if (!LoadFileData(input_filename.c_str(), input_file))
	{
		printf("\nERROR: Failed to open input file %s\n", input_filename.c_str());
		return 1;
	}

	input_file = PreProcessFile(args, input_filename, input_file);

	ComputeProcessor processor(args, input_filename, input_file);
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
