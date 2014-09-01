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
#include <algorithm>

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
	printf("   -d <sym|sym=val>   Define macro symbols\n");
	printf("   -show_includes     Print the included files to stdout\n");
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


std::string FormatPath(std::string path)
{
	// Replace back-slash with forward-slash as fcpp can't handle it
	std::replace(path.begin(), path.end(), '\\', '/');

	// Requirement for FPPTAG_INCLUDE_DIR
	if (path.back() != '/')
		path += '/';

	return path;
}


std::vector<char> PreProcessFile(const Arguments& args, std::string filename, const std::vector<char>& in_data, ComputeTarget target)
{
	fppTag tags[200];
	fppTag* tagptr = tags;

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

	// Don't display version information
	tagptr->tag = FPPTAG_SHOWVERSION;
	tagptr->data = (void*)FALSE;
	tagptr++;

	// NOTE: Ignore this comment for now, I've set the option to TRUE. Will investigate why it's now
	// working when I get a chance.
	//
	// When using the "-show_includes" option I would like cbpp to output the FULL PATH to included files.
	// When you do a local include "X", fcpp will only print the relative path to the included file.
	// Rather than refactoring the internals of fcpp I've added this new tag which forces all includes
	// to go down the non-local search path route. This requires the directory of the input file to be
	// registered as an include directory manually.
	tagptr->tag = FPPTAG_ALLOW_INCLUDE_LOCAL;
	tagptr->data = (void*)TRUE;
	tagptr++;

	// Promote the input filename to an absolute path so that relative paths can provide an include directory
	if (!IsPathAbsolute(filename))
	{
		std::string cwd = GetCurrentWorkingDirectory();
		filename = JoinPaths(cwd, filename);
		std::replace(filename.begin(), filename.end(), '\\', '/');
	}

	// Set the input filename
	tagptr->tag = FPPTAG_INPUT_NAME;
	tagptr->data = (void*)filename.data();
	tagptr++;

	// Add the directory of the input file to the list of include search directories
	std::string filename_dir = GetPathDirectory(filename);
	if (!filename_dir.empty())
	{		
		// Add the location of the filename as an include directory
		filename_dir = FormatPath(filename_dir);
		tagptr->tag = FPPTAG_INCLUDE_DIR;
		tagptr->data = (void*)filename_dir.data();
		tagptr++;
	}

	// Have to modify include directories specified on the command-line and keep them
	// around in memory so that the fppTag mechanism can reference them.
	// Pre-allocate max count of includes using arg count as push_back() can destroy
	// memory of previously pushed items. Not ideal.
	std::vector<std::string> include_dirs(args.Count());

	// Loop reading all include directories
	int include_dir_idx = 0;
	while (true)
	{
		std::string cmd_include_dir = args.GetProperty("-i", include_dir_idx);
		if (cmd_include_dir == "")
			break;

		std::string& include_dir = include_dirs[include_dir_idx++];
		include_dir = FormatPath(cmd_include_dir);

		// Add a tag for the include directory
		tagptr->tag = FPPTAG_INCLUDE_DIR;
		tagptr->data = (void*)include_dir.data();
		tagptr++;
	}

	// Add the target platform define
	// NOTE: The Prologue header gets added AFTER pre-processing so this define is
	//       not used to control that. At least allows the source to know who they're
	//       compiling for.
	tagptr->tag = FPPTAG_DEFINE;
	tagptr->data = (void*)((target == ComputeTarget_OpenCL) ? "CMP_OPENCL" : "CMP_CUDA");
	tagptr++;

	// Loop reading all defines
	int nb_defines = 0;
	while (true)
	{
		int index = args.GetIndexOf("-d", nb_defines++);
		if (index == -1 || index > args.Count() - 1)
			break;

		// Add a tag for the define
		const std::string& define = args[index + 1];
		tagptr->tag = FPPTAG_DEFINE;
		tagptr->data = (void*)define.data();
		tagptr++;
	}

	// Optionally show include dependencies
	if (args.Have("-show_includes"))
	{
		tagptr->tag = FPPTAG_OUTPUTINCLUDES;
		tagptr->data = (void*)TRUE;
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

	// Decide for which target to emit
	std::string target_name = args.GetProperty("-target");
	std::transform(target_name.begin(), target_name.end(), target_name.begin(), tolower);
	ComputeTarget target = ComputeTarget_None;
	if (target_name == "cuda")
		target = ComputeTarget_CUDA;
	else if (target_name == "opencl")
		target = ComputeTarget_OpenCL;
	else
	{
		printf("ERROR: Valid compute target not specified\n\n");
		return 1;
	}

	// Load the input file
	std::string input_filename = args[1];
	std::vector<char> input_file;
	if (!LoadFileData(input_filename.c_str(), input_file))
	{
		printf("\nERROR: Failed to open input file %s\n", input_filename.c_str());
		return 1;
	}

	input_file = PreProcessFile(args, input_filename, input_file, target);

	ComputeProcessor processor(args, input_filename, input_file, target);
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
