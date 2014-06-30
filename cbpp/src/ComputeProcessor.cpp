
#include "ComputeProcessor.h"

#include <cassert>
#include <string>
#include <algorithm>


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
std::string GetExecutableFullPath()
{
	char buffer[512];
	GetModuleFileName(NULL, buffer, sizeof(buffer) - 1);
	return buffer;
}
#endif


// List of all registered transform descriptions
static std::vector<TransformDescBase*> g_TransformDescs;


Arguments::Arguments(int argc, const char* argv[])
{
	// Copy from the command-line into local storage
	m_Arguments.resize(argc);
	for (size_t i = 0; i < m_Arguments.size(); i++)
		m_Arguments[i] = argv[i];

	// Override first argument with complete path to executable
	m_Arguments[0] = GetExecutableFullPath();
}


size_t Arguments::GetIndexOf(const std::string& arg, int occurrence) const
{
	// Linear search for a matching argument
	int found = 0;
	for (size_t i = 0; i < m_Arguments.size(); i++)
	{
		if (m_Arguments[i] == arg)
		{
			if (found++ == occurrence)
				return i;
		}
	}

	return -1;
}


bool Arguments::Have(const std::string& arg) const
{
	// Does the specific argument exist?
	return GetIndexOf(arg) != -1;
}


std::string Arguments::GetProperty(const std::string& arg, int occurrence) const
{
	// Does the arg exist and does it have a value
	size_t index = GetIndexOf(arg, occurrence);
	if (index == -1 || index + 1 >= m_Arguments.size())
		return "";

	return m_Arguments[index + 1];
}


size_t Arguments::Count() const
{
	return m_Arguments.size();
}


const std::string& Arguments::operator [] (int index) const
{
	return m_Arguments[index];
}


TokenList::TokenList()
	: first(0)
	, last(0)
	, error(cmpError_CreateOK())
{
}


TokenList::TokenList(cmpToken* first_token, cmpToken* last_token)
	: first(first_token)
	, last(last_token)
	, error(cmpError_CreateOK())
{
}


cmpToken* TokenList::Add(cmpToken *token)
{
	cmpToken_AddToList(&first, &last, token);
	return token;
}


cmpToken* TokenList::Add(enum cmpTokenType type, const char* start, cmpU32 length, cmpU32 line)
{
	cmpToken* token;
	error = cmpToken_Create(&token, type, start, length, line);
	if (!cmpError_OK(&error))
		throw error;
	return Add(token);
}


cmpToken* TokenList::Add(enum cmpTokenType type, cmpU32 line)
{
	// Ensure this is a single character token
	assert(type != cmpToken_None);
	assert(type == cmpToken_Whitespace || type == cmpToken_EOL || type < cmpToken_InvalidSeparator);

	// Construnct a single character string out of its character code
	const char* text;
	switch (type)
	{
		case cmpToken_LBrace: text = "{"; break;
		case cmpToken_RBrace: text = "}"; break;
		case cmpToken_Comma: text = ","; break;
		case cmpToken_LBracket: text = "("; break;
		case cmpToken_RBracket: text = ")"; break;
		case cmpToken_LSqBracket: text = "["; break;
		case cmpToken_RSqBracket: text = "]"; break;
		case cmpToken_Colon: text = ":"; break;
		case cmpToken_SemiColon: text = ";"; break;
		case cmpToken_Period: text = "."; break;
		case cmpToken_Question: text = "?"; break;
		case cmpToken_Tilde: text = "~"; break;
		case cmpToken_LAngle: text = "<"; break;
		case cmpToken_RAngle: text = ">"; break;
		case cmpToken_Plus: text = "+"; break;
		case cmpToken_Minus: text = "-"; break;
		case cmpToken_Asterisk: text = "*"; break;
		case cmpToken_Divide: text = "/"; break;
		case cmpToken_Modulo: text = "%%"; break;
		case cmpToken_Equals: text = "="; break;
		case cmpToken_And: text = "&"; break;
		case cmpToken_Or: text = "|"; break;
		case cmpToken_Xor: text = "^"; break;
		case cmpToken_Not: text = "!"; break;
		case cmpToken_Hash: text = "#"; break;

		// Single character tokens for writing to file but not necessarily as input
		case cmpToken_Whitespace: text = " "; break;
		case cmpToken_EOL: text = "\n"; break;

		default:
			assert(false);
	}

	return Add(type, text, 1, line);
}


cmpToken* TokenList::Add(const HashString& string, cmpU32 line)
{
	// Create a symbol token using globally persistent HashString text
	cmpToken* token = Add(cmpToken_Symbol, string.text, string.length, line);
	token->hash = string.hash;
	return token;
}


cmpToken* TokenList::Add(const String& string, cmpU32 line)
{
	cmpToken* token = Add(cmpToken_String, string.text, string.length, line);
	token->hash = cmpHash(string.text, string.length);
	return token;
}


void TokenList::DeleteAll()
{
	// Move one beyond last for while comparison delete to be inclusive
	if (last != 0)
		last = last->next;

	while (first != last)
	{
		cmpToken* next = first->next;
		cmpToken_Destroy(first);
		first = next;
	}

	first = 0;
	last = 0;
}


ComputeProcessor::ComputeProcessor(const ::Arguments& arguments)
	: m_Arguments(arguments)
	, m_InputFilename(arguments[1])
	, m_Target(ComputeTarget_None)
	, m_MemoryFile(0)
	, m_LexerCursor(0)
	, m_ParserCursor(0)
	, m_RootNode(0)
{
	// Parse the executable path looking for its directory
	m_ExecutableDirectory = m_Arguments[0];
	size_t sep = m_ExecutableDirectory.rfind('\\');
	if (sep == -1)
		sep = m_ExecutableDirectory.rfind('/');
	if (sep != -1)
		m_ExecutableDirectory = m_ExecutableDirectory.substr(0, sep);
}


ComputeProcessor::~ComputeProcessor()
{
	// Destroy all transforms, assuming transform description list and allocate transforms lists are in sync
	for (size_t i = 0; i < m_Transforms.size(); i++)
	{
		const TransformDescBase* desc = g_TransformDescs[i];
		assert(desc != 0);
		assert(desc->delete_func != 0);
		desc->delete_func(m_Transforms[i]);
	}

	// Destroy all tokens
	m_Tokens.DeleteAll();

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


bool ComputeProcessor::ParseFile()
{
	const char* filename = m_InputFilename.c_str();
	bool verbose = m_Arguments.Have("-verbose");

	// Decide which for which target to emit
	std::string target = m_Arguments.GetProperty("-target");
	std::transform(target.begin(), target.end(), target.begin(), tolower);
	if (target == "cuda")
		m_Target = ComputeTarget_CUDA;
	else if (target == "opencl")
		m_Target = ComputeTarget_OpenCL;
	else
	{
		printf("Valid compute target not specified\n\n");
		return false;
	}

	// Open the input file
	if (cmpError error = cmpMemoryFile_Create(&m_MemoryFile, filename))
	{
		printf("Error opening input file: %s\n\n", cmpError_Text(&error));
		return false;
	}

	// Build a list of tokens
	if (cmpError error = cmpLexerCursor_Create(&m_LexerCursor, cmpMemoryFile_Data(m_MemoryFile), cmpMemoryFile_Size(m_MemoryFile), verbose))
	{
		printf("Error creating lexer cursor: %s\n\n", cmpError_Text(&error));
		return false;
	}
	while (cmpToken* token = cmpLexer_ConsumeToken(m_LexerCursor))
	{
		m_Tokens.Add(token);
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
	if (cmpError error = cmpParserCursor_Create(&m_ParserCursor, m_Tokens.first, verbose))
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
	bool VisitNode(const ComputeProcessor& processor, cmpNode* node, INodeVisitor* visitor)
	{
		assert(visitor != 0);
		assert(node != 0);
		if (!visitor->Visit(processor, *node))
			return false;

		for (cmpNode* child = node->first_child; child != 0; child = child->next_sibling)
		{
			if (!VisitNode(processor, child, visitor))
				return false;
		}

		return true;
	}
}


bool ComputeProcessor::VisitNodes(INodeVisitor* visitor)
{
	assert(visitor != 0);
	return VisitNode(*this, m_RootNode, visitor);
}


cmpError ComputeProcessor::ApplyTransforms()
{
	if (m_Transforms.empty())
	{
		// Create all transforms for the processor the first time this function is called
		for (size_t i = 0; i < g_TransformDescs.size(); i++)
		{
			const TransformDescBase* desc = g_TransformDescs[i];
			assert(desc != 0);
			assert(desc->new_func != 0);
			ITransform* transform = desc->new_func();
			assert(transform != 0);
			m_Transforms.push_back(transform);
		}
	}

	// Apply all transforms in the (indeterminate) order they are registered
	for (size_t i = 0; i < m_Transforms.size(); i++)
	{
		ITransform* transform = m_Transforms[i];
		assert(transform != 0);
		cmpError error = transform->Apply(*this);
		if (!cmpError_OK(&error))
			return error;
	}

	return cmpError_CreateOK();
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


String::String()
	: text(0)
	, length(0)
{
}


String::String(const char *source, size_t src_length)
	: text(0)
	, length(0)
{
	Set(source, src_length);
}


String::String(const std::string& source)
	: text(0)
	, length(0)
{
	Set(source.c_str(), source.length());
}


String::String(const String& rhs)
	: text(rhs.text)
	, length(rhs.length)
{
	// Take ownership
	rhs.text = 0;
}


String::~String()
{
	if (text != 0)
		delete [] text;	
}


void String::Set(const char *source, size_t src_length)
{
	// Copy and null terminate
	assert(text == 0);
	length = src_length;
	text = new char[length + 1];
	memcpy(text, source, length);
	text[length] = 0;
}


String& String::operator = (const String& rhs)
{
	// Take ownership
	assert(text == 0);
	text = rhs.text;
	length = rhs.length;
	rhs.text = 0;
	return *this;
}



TransformDescBase::TransformDescBase(NewFunc new_func, DeleteFunc delete_func)
	: new_func(new_func)
	, delete_func(delete_func)
{
	// Register transform description
	g_TransformDescs.push_back(this);
}
