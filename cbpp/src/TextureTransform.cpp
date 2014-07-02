
#include "ComputeProcessor.h"

#include <map>
#include <cassert>
#include <string>
#include <algorithm>


namespace
{
	// Texture types
	HashString KEYWORD_Texture3Du("Texture3Du");
	HashString KEYWORD_Texture3Dn("Texture3Dn");
	HashString KEYWORD_Texture2Du("Texture2Du");
	HashString KEYWORD_Texture2Dn("Texture2Dn");
	HashString KEYWORD_Texture1Du("Texture1Du");
	HashString KEYWORD_Texture1Dn("Texture1Dn");

	// Surface types
	HashString KEYWORD_Surface3D("Surface3D");
	HashString KEYWORD_Surface2D("Surface2D");
	HashString KEYWORD_Surface1D("Surface1D");

	// Texture texel types
	HashString KEYWORD_char("char");
	HashString KEYWORD_short("short");
	HashString KEYWORD_int("int");
	HashString KEYWORD_long("long");
	HashString KEYWORD_float("float");
	HashString KEYWORD_signed("signed");
	HashString KEYWORD_unsigned("unsigned");

	// CUDA/OpenCL keywords
	HashString KEYWORD_kernel("kernel");
	HashString KEYWORD_extern("extern");
	HashString KEYWORD_global("__global__");

	// ComputeBridge macros
	HashString KEYWORD_cmp_texture_type("cmp_texture_type");
	HashString KEYWORD_cmp_kernel_texture_decl("cmp_kernel_texture_decl");
	HashString KEYWORD_cmp_kernel_texture_decl_comma("cmp_kernel_texture_decl_comma");
	HashString KEYWORD_cmp_kernel_texture_global_def("cmp_kernel_texture_global_def");
	HashString KEYWORD_cmp_kernel_texture_local_def("cmp_kernel_texture_local_def");
	HashString KEYWORD_cmp_surface_type("cmp_surface_type");
	HashString KEYWORD_cmp_kernel_surface_decl("cmp_kernel_surface_decl");
	HashString KEYWORD_cmp_kernel_surface_decl_comma("cmp_kernel_surface_decl_comma");
	HashString KEYWORD_cmp_kernel_surface_global_def("cmp_kernel_surface_global_def");
	HashString KEYWORD_cmp_kernel_surface_local_def("cmp_kernel_surface_local_def");

	// Texture dimensions
	HashString KEYWORD_1("1");
	HashString KEYWORD_2("2");
	HashString KEYWORD_3("3");

	// CUDA read types
	HashString KEYWORD_cudaReadModeElementType("cudaReadModeElementType");
	HashString KEYWORD_cudaReadModeNormalizedFloat("cudaReadModeNormalizedFloat");
}


enum RefType
{
	RefType_None = 0,
	RefType_Texture = 't',
	RefType_Surface = 's',
};



//
// Reference to a texture type in the source file.
// Example: "Texture3Dn<short>"
//
struct TextureRef
{
	TextureRef()
		: type(RefType_None)
		, node(0)
		, position(0)
		, line(0)
		, keyword_token(0)
		, type_token(0)
		, nb_type_tokens(0)
		, end_of_type_token(0)
		, name_token(0)
		, type_key(0)
	{
	}


	cmpU32 Dimensions() const
	{
		assert(keyword_token != 0);
		assert(keyword_token->length > 7);
		cmpU32 dimensions = keyword_token->start[7] - '0';
		assert(dimensions < 4);
		return dimensions;
	}


	char ReadType() const
	{
		assert(keyword_token != 0);
		assert(keyword_token->length > 9);
		char read_type = keyword_token->start[9];
		assert(read_type == 'u' || read_type == 'n');
		return read_type;
	}

	// Is this a texture/surface reference?
	RefType type;

	// Pointer to the statement, typedef or function parameter list
	cmpNode* node;

	// Position/line the reference was found on
	cmpU32 position;
	cmpU32 line;

	// Texture/surface keyword
	cmpToken* keyword_token;

	// Texel type keyword that may consist of two tokens, e.g. "unsigned int"
	// Doesn't exist for surfaces
	cmpToken* type_token;
	cmpU32 nb_type_tokens;

	// Points to a token one place beyond the last token that defines the type
	cmpToken* end_of_type_token;

	// Only set for function parameters
	cmpToken* name_token;
	String name;

	// Unique key specific to the type of reference
	cmpU32 type_key;
};


//
// A vector of all texture ref instances that share the same dimension/type/read mode
//
typedef std::vector<TextureRef> TextureRefs;


//
// A map from the hash a texture reference to all its found instances
//
typedef std::map<cmpU32, TextureRefs> TextureRefsMap;


//
// Searches the AST of a source file for all texture type references
//
class FindTextureRefs : public INodeVisitor
{
public:
	FindTextureRefs(TextureRefsMap& refs_map)
		: m_TextureRefsMap(refs_map)
		, m_LastError(cmpError_CreateOK())
	{
		m_TextureMatches = MatchHashes(
			KEYWORD_Texture3Dn.hash,
			KEYWORD_Texture3Du.hash,
			KEYWORD_Texture2Dn.hash,
			KEYWORD_Texture2Du.hash,
			KEYWORD_Texture1Dn.hash,
			KEYWORD_Texture1Du.hash);

		m_SurfaceMatches = MatchHashes(
			KEYWORD_Surface3D.hash,
			KEYWORD_Surface2D.hash,
			KEYWORD_Surface1D.hash);

		m_TypeMatches = MatchHashes(
			KEYWORD_char.hash,
			KEYWORD_short.hash,
			KEYWORD_int.hash,
			KEYWORD_long.hash,
			KEYWORD_float.hash,
			KEYWORD_signed.hash,
			KEYWORD_unsigned.hash);
	}


	bool Visit(const ComputeProcessor& processor, cmpNode& node)
	{
		// Filter out the node types we're not interested in
		if (node.type != cmpNode_Statement &&
			node.type != cmpNode_FunctionParams &&
			node.type != cmpNode_Typedef)
			return true;

		try
		{
			// Gather all texture references in the statement
			const char* filename = processor.InputFilename().c_str();
			TokenIterator iterator(node);
			while (ScanStatementForRefs(filename, node, iterator))
				;
		}
		catch (const cmpError& error)
		{
			m_LastError = error;
			return false;
		}

		return true;
	}


	const cmpError& LastError() const
	{
		return m_LastError;
	}


private:
	bool ScanStatementForRefs(const char* filename, cmpNode& node, TokenIterator& iterator)
	{
		// Search for any of the texture keywords
		if (iterator.SeekToken(m_TextureMatches))
		{
			AddTextureRef(filename, node, iterator);
			return true;
		}
		if (iterator.SeekToken(m_SurfaceMatches))
		{
			AddSurfaceRef(filename, node, iterator);
			return true;
		}

		return false;
	}


	void AddTextureRef(const char* filename, cmpNode& node, TokenIterator& iterator)
	{
		// Start the texture reference off with its node/token and token hash
		TextureRef ref;
		ref.type = RefType_Texture;
		ref.node = &node;
		ref.position = (cmpU32)iterator.token->start;
		ref.line = iterator.token->line;
		ref.keyword_token = iterator.token;
		cmpU32 combined_hash = iterator.token->hash;
		++iterator;

		// Ensure '<' follows
		if (iterator.ExpectToken(MatchTypes(cmpToken_LAngle)) == 0)
			throw cmpError_Create("%s(%d): Expecting '<'", filename, iterator.token->line);
		++iterator;

		// Ensure a type name is next
		cmpToken* type_token_0 = iterator.ExpectToken(m_TypeMatches);
		if (type_token_0 == 0)
			throw cmpError_Create("%s(%d): Expecting a type name", filename, iterator.token->line);
		ref.type_token = type_token_0;
		ref.nb_type_tokens = 1;
		combined_hash = cmpHash_Combine(combined_hash, type_token_0->hash);
		++iterator;

		// If the type name was signed/unsigned, expect the rest of the type name
		if (type_token_0->hash == KEYWORD_signed.hash || type_token_0->hash == KEYWORD_unsigned.hash)
		{
			const cmpToken* type_token_1 = iterator.ExpectToken(m_TypeMatches);
			if (type_token_1 == 0)
				throw cmpError_Create("%s(%d): Expecting a type name after unsigned/signed", filename, iterator.token->line);
			if (type_token_1->hash == KEYWORD_signed.hash || type_token_1->hash == KEYWORD_unsigned.hash)
				throw cmpError_Create("%s(%d): Not expecting unsigned/signed twice", filename, iterator.token->line);

			ref.nb_type_tokens = 2;
			combined_hash = cmpHash_Combine(combined_hash, type_token_1->hash);
			++iterator;
		}

		// Ensure '>' closes the type naming
		if (iterator.ExpectToken(MatchTypes(cmpToken_RAngle)) == 0)
			throw cmpError_Create("%s(%d): Expecting '>'", filename, iterator.token->line);
		ref.end_of_type_token = iterator.token;
		++iterator;

		// Ensure that function parameters have a name
		if (node.type == cmpNode_FunctionParams)
		{
			if (iterator.ExpectToken(MatchTypes(cmpToken_Symbol)) == 0)
				throw cmpError_Create("%s(%d): Expecting function parameter to have a name", filename, iterator.token->line);
			ref.name_token = iterator.token;
			++iterator;

			// Allocate a copy of the string for use later
			ref.name = String(ref.name_token->start, ref.name_token->length);
		}

		// Record the texture reference
		ref.type_key = combined_hash;
		m_TextureRefsMap[combined_hash].push_back(ref);
	}


	void AddSurfaceRef(const char* filename, cmpNode& node, TokenIterator& iterator)
	{
		// Start the surface reference off with its node/token and token hash
		TextureRef ref;
		ref.type = RefType_Surface;
		ref.node = &node;
		ref.position = (cmpU32)iterator.token->start;
		ref.line = iterator.token->line;
		ref.keyword_token = iterator.token;
		ref.end_of_type_token = iterator.token;
		ref.type_key = iterator.token->hash;
		++iterator;

		// Ensure that function parameters have a name
		if (node.type == cmpNode_FunctionParams)
		{
			if (iterator.ExpectToken(MatchTypes(cmpToken_Symbol)) == 0)
				throw cmpError_Create("%s(%d): Expecting function parameter to have a name", filename, iterator.token->line);
			ref.name_token = iterator.token;
			++iterator;

			// Allocate a copy of the string for use later
			ref.name = String(ref.name_token->start, ref.name_token->length);
		}

		// Record the surface reference
		m_TextureRefsMap[ref.type_key].push_back(ref);
	}


	MatchHashes m_TextureMatches;
	MatchHashes m_SurfaceMatches;
	MatchHashes m_TypeMatches;

	TextureRefsMap& m_TextureRefsMap;

	cmpError m_LastError;
};


namespace
{
	const TextureRef& FindFirstTextureRef(const TextureRefs& refs)
	{
		const TextureRef* found_ref = 0;
		cmpU32 first_line = UINT_MAX;

		// Linear search through all texture refs looking for the one that occurs first
		for (size_t i = 0; i < refs.size(); i++)
		{
			const TextureRef& ref = refs[i];
			cmpU32 line = ref.line;
			if (line < first_line)
			{
				found_ref = &ref;
				first_line = line;
			}
		}

		assert(found_ref != 0);
		return *found_ref;
	}


	const HashString* GetDimensionsKeyword(cmpU32 dimensions)
	{
		const HashString* kw_dimensions = 0;
		switch (dimensions)
		{
			case 1: kw_dimensions = &KEYWORD_1; break;
			case 2: kw_dimensions = &KEYWORD_2; break;
			case 3: kw_dimensions = &KEYWORD_3; break;
		}
		assert(kw_dimensions != 0);
		return kw_dimensions;
	}


	bool IsKernelFunction(cmpNode* node)
	{
		if (node->type != cmpNode_FunctionDefn && node->type != cmpNode_FunctionDecl)
			return false;

		//
		// Looking for these starting tokens:
		//
		// 		kernel					[ OpenCL AFTER pre-processor run ]
		// 		extern "C" __global__	[ CUDA AFTER pre-processor run ]
		//
		TokenIterator ti(*node);
		ti.SkipWhitespace();
		if (ti.token == 0)
			return false;

		// Check for OpenCL
		if (ti.token->hash == KEYWORD_kernel.hash)
			return true;

		// Check the CUDA equivalent
		// extern "C" __global__
		static cmpTokenType type_match[] =
		{
			cmpToken_Symbol,	// extern
			cmpToken_String,	// "C"
			cmpToken_Symbol,	// __global__
		};
		static const int NB_TOKENS = sizeof(type_match) / sizeof(type_match[0]);
		static cmpToken* tokens[NB_TOKENS];

		// Match all token types
		for (int i = 0; i < NB_TOKENS; i++)
		{
			cmpToken* token = ti.ExpectToken(MatchTypes(type_match[i]));
			if (ti.token == 0)
				return false;
			tokens[i] = token;
			++ti;
		}

		// Ensure the token contents match
		if (tokens[0]->hash != KEYWORD_extern.hash)
			return false;
		if (tokens[1]->length != 3 || tokens[1]->start[1] != 'C')
			return false;
		if (tokens[2]->hash != KEYWORD_global.hash)
			return false;

		return true;
	}


	std::string GetFunctionName(cmpNode* function_node)
	{
		assert(function_node->type == cmpNode_FunctionDefn || function_node->type == cmpNode_FunctionDecl);

		// Walk backwards until the first symbol is found
		cmpToken* function_name_token = function_node->last_token;
		while (function_name_token != NULL && function_name_token->type != cmpToken_Symbol)
			function_name_token = function_name_token->prev;

		// Use the symbol token to construct the name
		assert(function_name_token != NULL);
		std::string function_name(function_name_token->start, function_name_token->length);
		return function_name;
	}


	cmpNode* FindContainerParent(cmpNode* node)
	{
		// Typedefs are already a parent
		if (node->type == cmpNode_Typedef)
			return node;

		// Search up for a function definition/declaration
		while (node != 0 &&
			   node->type != cmpNode_FunctionDefn &&
			   node->type != cmpNode_FunctionDecl)
			node = node->parent;

		return node;
	}
}


struct TextureGlobalVar
{
	// Which function/variable pair generated this global variable
	std::string ref_name;
	std::string function_name;

	String global_name;

	TokenList tokens;
};


class TextureType
{
public:
	TextureType(cmpU32 texture_refs_key)
		: m_TextureRefsKey(texture_refs_key)
	{
	}


	~TextureType()
	{
		// Responsibility for cleaning created tokens belongs with this object
		m_TypeDeclTokens.DeleteAll();
		for (size_t i = 0; i < m_GlobalVars.size(); i++)
			m_GlobalVars[i].tokens.DeleteAll();
	}


	void AddTypeDeclaration(const TextureRef& ref, int unique_index)
	{
		if (ref.type == RefType_Texture)
			return AddTextureTypeDeclaration(ref, unique_index);
		if (ref.type == RefType_Surface)
			return AddSurfaceTypeDeclaration(ref, unique_index);

		assert(false && "Invalid RefType");
	}


	void ReplaceTypeInstance(const TextureRef& ref)
	{
		// Earlier errors should have caught the absence of a containing parent
		cmpNode* container_parent = FindContainerParent(ref.node);
		assert(container_parent != 0);

		// Is this a kernel function definition/declaration?
		if (ref.node->type == cmpNode_FunctionParams && IsKernelFunction(container_parent))
		{
			// Declarations/definitions require kernel parameter replacement
			ReplaceKernelParameter(ref, container_parent);

			// Definitions require a global variable and assignment to the variable
			if (container_parent->type == cmpNode_FunctionDefn)
			{
				AddKernelGlobalTextureDef(ref, container_parent);
				AddKernelLocalTextureDef(ref, container_parent);
			}

			return;
		}

		// Create the single replacement token
		cmpToken* token;
		cmpError error = cmpToken_Create(&token, cmpToken_Symbol, m_Name.text, m_Name.length, ref.line);
		if (!cmpError_OK(&error))
			throw error;

		// Cut out the original tokens and replace with the new one
		TokenList old_tokens(ref.keyword_token, ref.end_of_type_token);
		token->prev = old_tokens.first->prev;
		token->prev->next = token;
		token->next = old_tokens.last->next;
		token->next->prev = token;
		old_tokens.DeleteAll();
	}


	const TextureGlobalVar* FindGlobal(const std::string& function_name, const std::string& param_name) const
	{
		for (size_t i = 0; i < m_GlobalVars.size(); i++)
		{
			const TextureGlobalVar& var = m_GlobalVars[i];
			if (var.function_name == function_name && var.ref_name == param_name)
				return &var;
		}

		return 0;
	}


	cmpU32 TextureRefsKey() const
	{
		return m_TextureRefsKey;
	}


	cmpU32 Dimensions() const
	{
		return m_Dimensions;
	}


	char ReadType() const
	{
		return m_ReadType;
	}


private:
	// Non-copyable
	TextureType(const TextureType&);
	TextureType& operator = (const TextureType&);


	void AddTextureTypeDeclaration(const TextureRef& ref, int unique_index)
	{
		// Add cmp_texture_type(type, channels, read, name) macro call

		m_TypeDeclTokens.Add(KEYWORD_cmp_texture_type, ref.line);
		m_TypeDeclTokens.Add(cmpToken_LBracket, ref.line);

		AddTexelTypeNameTokens(ref);
		AddTextureDimensionsToken(ref);
		AddReadTypeToken(ref);
		AddUniqueNameToken(ref, unique_index, "Texture");

		m_TypeDeclTokens.Add(cmpToken_RBracket, ref.line);
		m_TypeDeclTokens.Add(cmpToken_SemiColon, ref.line);

		AddNodeBeforeContainerParent(m_TypeDeclTokens, ref.node);
	}


	void AddSurfaceTypeDeclaration(const TextureRef& ref, int unique_index)
	{
		// Add cmp_surface_type(channels, name) macro call

		m_TypeDeclTokens.Add(KEYWORD_cmp_surface_type, ref.line);
		m_TypeDeclTokens.Add(cmpToken_LBracket, ref.line);

		AddTextureDimensionsToken(ref);
		AddUniqueNameToken(ref, unique_index, "Surface");

		m_TypeDeclTokens.Add(cmpToken_RBracket, ref.line);
		m_TypeDeclTokens.Add(cmpToken_SemiColon, ref.line);

		AddNodeBeforeContainerParent(m_TypeDeclTokens, ref.node);
	}


	void AddTexelTypeNameTokens(const TextureRef& ref)
	{
		const cmpToken* type_token = ref.type_token;
		assert(type_token != 0);
		m_TypeDeclTokens.Add(cmpToken_Symbol, type_token->start, type_token->length, ref.line);
		if (ref.nb_type_tokens > 1)
		{
			type_token = type_token->next;
			assert(type_token != 0);
			m_TypeDeclTokens.Add(cmpToken_Symbol, type_token->start, type_token->length, ref.line);
		}
		m_TypeDeclTokens.Add(cmpToken_Comma, ref.line);
	}


	void AddReadTypeToken(const TextureRef& ref)
	{
		m_ReadType = ref.ReadType();
		if (m_ReadType == 'u')
			m_TypeDeclTokens.Add(KEYWORD_cudaReadModeElementType, ref.line);
		else
			m_TypeDeclTokens.Add(KEYWORD_cudaReadModeNormalizedFloat, ref.line);
		m_TypeDeclTokens.Add(cmpToken_Comma, ref.line);
	}


	void AddTextureDimensionsToken(const TextureRef& ref)
	{
		m_Dimensions = ref.Dimensions();
		const HashString* kw_dimensions = GetDimensionsKeyword(m_Dimensions);
		m_TypeDeclTokens.Add(cmpToken_Number, kw_dimensions->text, kw_dimensions->length, ref.line);
		m_TypeDeclTokens.Add(cmpToken_Comma, ref.line);
	}


	void AddUniqueNameToken(const TextureRef& ref, int unique_index, const char* name)
	{
		// Generate a unique type name and add as a symbol token
		char type_name[64];
		sprintf(type_name, "__%sTypeName_%d__", name, unique_index);
		m_Name = String(type_name);
		m_TypeDeclTokens.Add(cmpToken_Symbol, m_Name.text, m_Name.length, ref.line);
	}


	void AddNodeBeforeContainerParent(const TokenList& tokens, cmpNode* child_node)
	{
		// Create the containing node (to be deleted by the parse tree)
		cmpNode* node;
		cmpError error = cmpNode_CreateEmpty(&node);
		if (!cmpError_OK(&error))
			throw error;
		node->type = cmpNode_UserTokens;
		node->first_token = tokens.first;
		node->last_token = tokens.last;

		// Add right before the containing parent
		cmpNode* insert_before_node = FindContainerParent(child_node);
		if (insert_before_node == NULL)
			throw cmpError_Create("Failed to find a container parent for insertion");
		cmpNode_AddBefore(insert_before_node, node);
	}


	void ReplaceKernelParameter(const TextureRef& ref, cmpNode* function_node)
	{
		// Parameters with no names should have been caught as errors earlier
		cmpToken* name_token = ref.name_token;
		assert(name_token != 0);

		// Seek to the next parameter separator or the end of the parameters
		TokenList old_tokens(ref.keyword_token, ref.name_token);
		while (old_tokens.last != 0 &&
			   old_tokens.last->type != cmpToken_Comma &&
			   old_tokens.last->type != cmpToken_RBracket)
			old_tokens.last = old_tokens.last->next;
		assert(old_tokens.last != 0);

		// Middle parameters replace parameter and comma
		// End parameters replace just the parameter
		const HashString* keyword = (ref.type == RefType_Texture) ? 
			&KEYWORD_cmp_kernel_texture_decl_comma : &KEYWORD_cmp_kernel_surface_decl_comma;
		if (old_tokens.last->type == cmpToken_RBracket)
		{
			keyword = (ref.type == RefType_Texture) ?
				&KEYWORD_cmp_kernel_texture_decl : &KEYWORD_cmp_kernel_surface_decl;
			old_tokens.last = ref.name_token;
		}

		// All token replacement happens on the same line 
		cmpU32 line = ref.name_token->line;

		// Start of the replacement tokens
		TokenList new_tokens;
		new_tokens.Add(*keyword, line);
		new_tokens.Add(cmpToken_LBracket, line);

		// Add texture dimensions
		cmpU32 dimensions = ref.Dimensions();
		const HashString* kw_dimensions = GetDimensionsKeyword(dimensions);
		new_tokens.Add(cmpToken_Number, kw_dimensions->text, kw_dimensions->length, line);
		new_tokens.Add(cmpToken_Comma, line);

		// Finish with the parameter name
		new_tokens.Add(cmpToken_Symbol, ref.name.text, ref.name.length, line);
		new_tokens.Add(cmpToken_RBracket, line);

		// Replace the old tokens with the new ones
		old_tokens.first->prev->next = new_tokens.first;
		new_tokens.first->prev = old_tokens.first->prev;
		old_tokens.last->next->prev = new_tokens.last;
		new_tokens.last->next = old_tokens.last->next;

		// As the new tokens are being placed in the main token list, they'll get cleaned up safely
		// What remains is to clean the old tokens that were pulled out
		old_tokens.DeleteAll();
	}


	void AddKernelGlobalTextureDef(const TextureRef& ref, cmpNode* function_node)
	{
		TextureGlobalVar var;

		// Start the token list
		cmpU32 line = function_node->first_token->line;
		HashString keyword = (ref.type == RefType_Texture) ?
			KEYWORD_cmp_kernel_texture_global_def : KEYWORD_cmp_kernel_surface_global_def;
		var.tokens.Add(keyword, line);
		var.tokens.Add(cmpToken_LBracket, line);
		var.tokens.Add(cmpToken_Symbol, m_Name.text, m_Name.length, line);
		var.tokens.Add(cmpToken_Comma, line);

		// Finish off with a unique name for variable
		std::string function_name = GetFunctionName(function_node);
		char texture_var[64];
		const char* name = (ref.type == RefType_Texture) ? "Texture" : "Surface";
		sprintf(texture_var, "__%sVar_%s_%s__", name, function_name.c_str(), ref.name.text);
		var.ref_name = ref.name.text;
		var.function_name = function_name;
		var.global_name = String(texture_var);
		var.tokens.Add(cmpToken_Symbol, var.global_name.text, var.global_name.length, line);
		var.tokens.Add(cmpToken_RBracket, line);
		var.tokens.Add(cmpToken_SemiColon, line);

		AddNodeBeforeContainerParent(var.tokens, ref.node);

		m_GlobalVars.push_back(var);
	}


	void AddKernelLocalTextureDef(const TextureRef& ref, cmpNode* function_node)
	{
		// Search for the function's statement block, which should be there as verified by earlier checks
		cmpNode* block_node = function_node->first_child;
		while (block_node != NULL && block_node->type != cmpNode_StatementBlock)
			block_node = block_node->next_sibling;
		assert(block_node != NULL);

		const TextureGlobalVar& var = m_GlobalVars.back();

		// Build tokens for the definition
		TokenList tokens;
		cmpU32 line = function_node->first_token->line;
		HashString keyword = (ref.type == RefType_Texture) ?
			KEYWORD_cmp_kernel_texture_local_def : KEYWORD_cmp_kernel_surface_local_def;
		tokens.Add(keyword, line);
		tokens.Add(cmpToken_LBracket, line);
		tokens.Add(cmpToken_Symbol, m_Name.text, m_Name.length, line);
		tokens.Add(cmpToken_Comma, line);
		tokens.Add(cmpToken_Symbol, ref.name.text, ref.name.length, line);
		tokens.Add(cmpToken_Comma, line);
		tokens.Add(cmpToken_Symbol, var.global_name.text, var.global_name.length, line);
		tokens.Add(cmpToken_RBracket, line);
		tokens.Add(cmpToken_SemiColon, line);

		// Add to the end of the node's token list
		// Initially, that last token should be the brace without any EOL info
		block_node->last_token->next = tokens.first;
		tokens.first->prev = block_node->last_token;
		block_node->last_token = tokens.last;
	}


	// Key used to lookup texture refs that use this type
	cmpU32 m_TextureRefsKey;

	// Name of the uniquely generated string type
	String m_Name;

	// Tokens created for the unique typedef
	TokenList m_TypeDeclTokens;

	// Raw type info
	cmpU32 m_Dimensions;
	char m_ReadType;

	// List of global variables instantiated with this type
	std::vector<TextureGlobalVar> m_GlobalVars;
};


static bool TextureRefPtrSort(const TextureRef* a, const TextureRef* b)
{
	return a->position < b->position;
}


class TextureTransform : public ITransform
{
public:
	TextureTransform()
		: m_UniqueTypeIndex(0)
	{
	}

	~TextureTransform()
	{
		for (size_t i = 0; i < m_TextureTypes.size(); i++)
			delete m_TextureTypes[i];
	}

private:
	cmpError Apply(ComputeProcessor& processor)
	{
		if (cmpError error = FindAllTextureRefs(processor))
			return error;

		if (cmpError error = AddTypeDeclarations())
			return error;

		if (cmpError error = TransformAST())
			return error;

		if (cmpError error = WriteBinary(processor))
			return error;

		return cmpError_CreateOK();
	}


	cmpError FindAllTextureRefs(ComputeProcessor& processor)
	{
		// Find all texture references
		FindTextureRefs ftr(m_TextureRefsMap);
		if (!processor.VisitNodes(&ftr))
			return ftr.LastError();
		if (m_TextureRefsMap.size() == 0)
			return cmpError_CreateOK();

		return cmpError_CreateOK();
	}


	cmpError AddTypeDeclarations()
	{
		// Build a list of all unique texture types introduced
		for (TextureRefsMap::const_iterator i = m_TextureRefsMap.begin(); i != m_TextureRefsMap.end(); ++i)
		{
			const TextureRefs& refs = i->second;

			// Generate a texture type from the first instance of this texture reference
			const TextureRef& first_ref = FindFirstTextureRef(refs);
			TextureType* texture_type = new TextureType(i->first);

			// Place a type declaration somewhere before the first node
			try
			{
				texture_type->AddTypeDeclaration(first_ref, m_UniqueTypeIndex++);
			}
			catch (const cmpError& error)
			{
				delete texture_type;
				return error;
			}

			m_TextureTypes.push_back(texture_type);
		}

		return cmpError_CreateOK();
	}


	cmpError TransformAST()
	{
		// Replace the type of all texture references with the newly generated unique ones
		for (size_t i = 0; i < m_TextureTypes.size(); i++)
		{
			TextureType& texture_type = *m_TextureTypes[i];
			const TextureRefs& refs = m_TextureRefsMap.find(texture_type.TextureRefsKey())->second;

			for (size_t j = 0; j < refs.size(); j++)
			{
				try
				{
					texture_type.ReplaceTypeInstance(refs[j]);
				}
				catch (const cmpError& error)
				{
					return error;
				}
			}
		}

		return cmpError_CreateOK();
	}


	cmpError WriteBinary(const ComputeProcessor& processor)
	{
		// The absence of an output binary filename is not an error
		std::string output_bin = processor.Arguments().GetProperty("-output_bin");
		if (output_bin == "")
			return cmpError_CreateOK();

		typedef std::vector<const TextureRef*> TextureRefPtrs;
		typedef std::map<std::string, TextureRefPtrs> TextureRefPtrsMap;
		TextureRefPtrsMap ref_ptrs_map;

		// Iterate over all texture references
		for (TextureRefsMap::const_iterator i = m_TextureRefsMap.begin(); i != m_TextureRefsMap.end(); ++i)
		{
			const TextureRefs& refs = i->second;
			for (size_t j = 0; j < refs.size(); j++)
			{
				const TextureRef& ref = refs[j];

				// Looking for function parameters
				if (ref.node->type != cmpNode_FunctionParams)
					continue;

				// The function must be a kernel function definition (declarations are prototypes)
				cmpNode* function_node = FindContainerParent(ref.node);
				if (function_node->type != cmpNode_FunctionDefn || !IsKernelFunction(function_node))
					continue;

				// Group texture refs by function
				std::string function_name = GetFunctionName(function_node);
				ref_ptrs_map[function_name].push_back(&ref);
			}
		}

		// Sort all references by their position in the parameter list (implied from file position)
		for (TextureRefPtrsMap::iterator i = ref_ptrs_map.begin(); i != ref_ptrs_map.end(); ++i)
		{
			TextureRefPtrs& ptrs = i->second;
			std::sort(ptrs.begin(), ptrs.end(), TextureRefPtrSort);
		}

		// Open the output file
		FILE* fp = fopen(output_bin.c_str(), "wb");
		if (fp == NULL)
			return cmpError_Create("Failed to write to output binary file '%s'", output_bin.c_str());

		// Write the header
		char ID[] = "CUDAKernelTextureParams";
		fwrite(ID, 1, sizeof(ID) - 1, fp);
		cmpU32 nb_functions = ref_ptrs_map.size();
		fwrite(&nb_functions, 1, sizeof(nb_functions), fp);

		// Iterate over every function's list of texture references
		for (TextureRefPtrsMap::const_iterator i = ref_ptrs_map.begin(); i != ref_ptrs_map.end(); ++i)
		{
			// Write the function name
			std::string function_name = i->first;
			size_t function_name_size = function_name.length();
			fwrite(&function_name_size, 1, sizeof(function_name_size), fp);
			fwrite(function_name.c_str(), 1, function_name_size, fp);

			// Write the number of texture parameters in the function
			const TextureRefPtrs& ptrs = i->second;
			size_t nb_ptrs = ptrs.size();
			fwrite(&nb_ptrs, 1, sizeof(nb_ptrs), fp);

			for (size_t j = 0; j < nb_ptrs; j++)
			{
				const TextureRef& ref = *ptrs[j];

				// Map the texture reference to its type
				const TextureType* type = FindTextureType(ref.type_key);
				if (type == 0)
					continue;

				// Map the texture reference to the global variable it generated
				const TextureGlobalVar* var = type->FindGlobal(function_name, ref.name.text);
				if (var == 0)
					continue;

				// Write global variable name
				fwrite(&var->global_name.length, 1, sizeof(var->global_name.length), fp);
				fwrite(var->global_name.text, 1, var->global_name.length, fp);

				// Write type info
				fwrite(&ref.type, 1, 1, fp);
				cmpU32 dimensions = type->Dimensions();
				fwrite(&dimensions, 1, sizeof(dimensions), fp);
				char read_type = type->ReadType();
				fwrite(&read_type, 1, 1, fp);
			}
		}

		fclose(fp);

		return cmpError_CreateOK();
	}


	const TextureType* FindTextureType(cmpU32 type_key) const
	{
		for (size_t i = 0; i < m_TextureTypes.size(); i++)
		{
			const TextureType* type = m_TextureTypes[i];
			if (type->TextureRefsKey() == type_key)
				return type;
		}

		return 0;
	}


	// Used to generate unique type names for texture references
	int m_UniqueTypeIndex;

	TextureRefsMap m_TextureRefsMap;

	std::vector<TextureType*> m_TextureTypes;
};


// Register transform
static TransformDesc<TextureTransform> g_Transform;
