
#include "ComputeProcessor.h"

#include <map>
#include <cassert>


namespace
{
	struct Keyword
	{
		Keyword(const char* text)
			: text(text)
			, length(strlen(text))
			, hash(cmpHash(text, length))
		{
		}

		const char* text;

		cmpU32 length;

		cmpU32 hash;
	};


	// Texture types
	Keyword KEYWORD_Texture3Du("Texture3Du");
	Keyword KEYWORD_Texture3Dn("Texture3Dn");
	Keyword KEYWORD_Texture2Du("Texture2Du");
	Keyword KEYWORD_Texture2Dn("Texture2Dn");
	Keyword KEYWORD_Texture1Du("Texture1Du");
	Keyword KEYWORD_Texture1Dn("Texture1Dn");

	// Texture texel types
	Keyword KEYWORD_char("char");
	Keyword KEYWORD_short("short");
	Keyword KEYWORD_int("int");
	Keyword KEYWORD_long("long");
	Keyword KEYWORD_float("float");
	Keyword KEYWORD_signed("signed");
	Keyword KEYWORD_unsigned("unsigned");

	Keyword KEYWORD_cmp_kernel_fn("cmp_kernel_fn");
	Keyword KEYWORD_cmp_texture_type("cmp_texture_type");
	Keyword KEYWORD_cmp_kernel_texture_decl("cmp_kernel_texture_decl");
	Keyword KEYWORD_cmp_kernel_texture_decl_comma("cmp_kernel_texture_decl_comma");

	// Texture dimensions
	Keyword KEYWORD_1("1");
	Keyword KEYWORD_2("2");
	Keyword KEYWORD_3("3");

	// CUDA read types
	Keyword KEYWORD_cudaReadModeElementType("cudaReadModeElementType");
	Keyword KEYWORD_cudaReadModeNormalizedFloat("cudaReadModeNormalizedFloat");


	cmpU32 CombineHash(cmpU32 combined_hash, cmpU32 hash)
	{
		// A sequence of 32 uniformly random bits so that each bit of the combined hash is changed on application
		// Derived from the golden ratio: UINT_MAX / ((1 + sqrt(5)) / 2)
		// In reality it's just an arbitrary value which happens to work well, avoiding mapping all zeros to zeros.
		// http://burtleburtle.net/bob/hash/doobs.html
		static cmpU32 random_bits = 0x9E3779B9;
		combined_hash ^= hash + random_bits + (combined_hash << 6) + (combined_hash >> 2);
		return combined_hash;
	}
}


//
// Reference to a texture type in the source file.
// Example: "Texture3Dn<short>"
//
struct TextureRef
{
	TextureRef()
		: node(0)
		, line(0)
		, keyword_token(0)
		, type_token(0)
		, nb_type_tokens(0)
		, end_of_type_token(0)
		, name_token(0)
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

	// Pointer to the statement, typedef or function parameter list
	cmpNode* node;

	// Line the texture reference was found on
	cmpU32 line;

	// Texture keyword
	cmpToken* keyword_token;

	// Texel type keyword that may consist of two tokens, e.g. "unsigned int"
	cmpToken* type_token;
	cmpU32 nb_type_tokens;

	// Points to a token one place beyond the last token that defines the type
	cmpToken* end_of_type_token;

	// Only set for function parameters
	cmpToken* name_token;
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
	FindTextureRefs()
		: m_LastError(cmpError_CreateOK())
	{
		m_TextureMatches = MatchHashes(
			KEYWORD_Texture3Dn.hash,
			KEYWORD_Texture3Du.hash,
			KEYWORD_Texture2Dn.hash,
			KEYWORD_Texture2Du.hash,
			KEYWORD_Texture1Dn.hash,
			KEYWORD_Texture1Du.hash);

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

		// Search for any of the texture keywords
		TokenIterator iterator(node);
		if (iterator.SeekToken(m_TextureMatches) == 0)
			return true;

		const char* filename = processor.Filename().c_str();

		// Start the texture reference off with its node/token and token hash
		TextureRef ref;
		ref.node = &node;
		ref.line = iterator.token->line;
		ref.keyword_token = iterator.token;
		cmpU32 combined_hash = iterator.token->hash;
		++iterator;

		// Ensure '<' follows
		if (iterator.ExpectToken(MatchTypes(cmpToken_LAngle)) == 0)
		{
			m_LastError = cmpError_Create("%s(%d): Expecting '<'", filename, iterator.token->line);
			return false;
		}
		++iterator;

		// Ensure a type name is next
		cmpToken* type_token_0 = iterator.ExpectToken(m_TypeMatches);
		if (type_token_0 == 0)
		{
			m_LastError = cmpError_Create("%s(%d): Expecting a type name", filename, iterator.token->line);
			return false;
		}
		ref.type_token = type_token_0;
		ref.nb_type_tokens = 1;
		combined_hash = CombineHash(combined_hash, type_token_0->hash);
		++iterator;

		// If the type name was signed/unsigned, expect the rest of the type name
		if (type_token_0->hash == KEYWORD_signed.hash || type_token_0->hash == KEYWORD_unsigned.hash)
		{
			const cmpToken* type_token_1 = iterator.ExpectToken(m_TypeMatches);
			if (type_token_1 == 0)
			{
				m_LastError = cmpError_Create("%s(%d): Expecting a type name after unsigned/signed", filename, iterator.token->line);
				return false;
			}
			if (type_token_1->hash == KEYWORD_signed.hash || type_token_1->hash == KEYWORD_unsigned.hash)
			{
				m_LastError = cmpError_Create("%s(%d): Not expecting unsigned/signed twice", filename, iterator.token->line);
				return false;
			}

			ref.nb_type_tokens = 2;
			combined_hash = CombineHash(combined_hash, type_token_1->hash);
			++iterator;
		}

		// Ensure '>' closes the type naming
		if (iterator.ExpectToken(MatchTypes(cmpToken_RAngle)) == 0)
		{
			m_LastError = cmpError_Create("%s(%d): Expecting '>'", filename, iterator.token->line);
			return false;
		}
		ref.end_of_type_token = iterator.token;
		++iterator;

		// Ensure that function parameters have a name
		if (node.type == cmpNode_FunctionParams)
		{
			if (iterator.ExpectToken(MatchTypes(cmpToken_Symbol)) == 0)
			{
				m_LastError = cmpError_Create("%s(%d): Expecting function parameter to have a name", filename, iterator.token->line);
				return false;
			}
			ref.name_token = iterator.token;
			++iterator;
		}

		// Record the texture reference
		m_TextureRefsMap[combined_hash].push_back(ref);
		return true;
	}


	const TextureRefsMap& Results() const
	{
		return m_TextureRefsMap;
	}


	const cmpError& LastError() const
	{
		return m_LastError;
	}


private:
	MatchHashes m_TextureMatches;
	MatchHashes m_TypeMatches;

	TextureRefsMap m_TextureRefsMap;

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


	const Keyword* GetDimensionsKeyword(cmpU32 dimensions)
	{
		const Keyword* kw_dimensions = 0;
		switch (dimensions)
		{
			case 1: kw_dimensions = &KEYWORD_1; break;
			case 2: kw_dimensions = &KEYWORD_2; break;
			case 3: kw_dimensions = &KEYWORD_3; break;
		}
		assert(kw_dimensions != 0);
		return kw_dimensions;
	}


	cmpToken* AddToken(TokenList& tokens, const Keyword& keyword, cmpU32 line)
	{
		// Create a symbol token using globally persistent keyword text
		cmpToken* token = tokens.Add(cmpToken_Symbol, keyword.text, keyword.length, line);
		if (token != 0)
			token->hash = keyword.hash;
		return token;
	}
}


//
// Persistent pointers to text that cmpToken objects can reference. TextureType objects may be moved
// around in memory, ruling out embedded char arrays. std::string may make small-string optimisations
// by embedding local char arrays so that's out of the window, too.
// Transfers ownership on copy.
//
struct String
{
	String()
		: text(0)
		, length(0)
	{
	}


	String(const std::string& source)
		: text(0)
		, length(0)
	{
		// Copy and null terminate
		length = source.length();
		text = new char[length + 1];
		memcpy(this->text, source.c_str(), length);
		this->text[length] = 0;
	}


	String(const String& rhs)
		: text(rhs.text)
		, length(rhs.length)
	{
		// Take ownership
		rhs.text = 0;
	}


	~String()
	{
		if (text != 0)
			delete [] text;
	}


	String& operator = (const String& rhs)
	{
		assert(text == 0);

		// Take ownership
		text = rhs.text;
		length = rhs.length;
		rhs.text = 0;
		return *this;
	}

	mutable char* text;
	size_t length;
};


class TextureType
{
public:
	TextureType(cmpU32 texture_refs_key)
		: m_TextureRefsKey(texture_refs_key)
		, m_LastError(cmpError_CreateOK())
	{
	}


	~TextureType()
	{
		// Responsibility for cleaning created tokens belongs with this object
		m_TypeDeclTokens.DeleteAll();
	}


	bool AddTypeDeclaration(const TextureRef& ref, int unique_index)
	{
		// Start off the macro call
		if (!AddToken(m_TypeDeclTokens, KEYWORD_cmp_texture_type, ref.line))
			return Failed(m_TypeDeclTokens);
		if (!m_TypeDeclTokens.Add(cmpToken_LBracket, ref.line))
			return Failed(m_TypeDeclTokens);

		// Add the texel type name tokens
		const cmpToken* type_token = ref.type_token;
		assert(type_token != 0);
		if (!m_TypeDeclTokens.Add(cmpToken_Symbol, type_token->start, type_token->length, ref.line))
			return Failed(m_TypeDeclTokens);
		if (ref.nb_type_tokens > 1)
		{
			type_token = type_token->next;
			assert(type_token != 0);
			if (!m_TypeDeclTokens.Add(cmpToken_Symbol, type_token->start, type_token->length, ref.line))
				return Failed(m_TypeDeclTokens);
		}
		if (!m_TypeDeclTokens.Add(cmpToken_Comma, ref.line))
			return Failed(m_TypeDeclTokens);

		// Add the texture dimension token
		cmpU32 dimensions = ref.Dimensions();
		const Keyword* kw_dimensions = GetDimensionsKeyword(dimensions);
		if (!m_TypeDeclTokens.Add(cmpToken_Number, kw_dimensions->text, kw_dimensions->length, ref.line))
			return Failed(m_TypeDeclTokens);
		if (!m_TypeDeclTokens.Add(cmpToken_Comma, ref.line))
			return Failed(m_TypeDeclTokens);

		// Add read type token
		char read_type = ref.ReadType();
		if (read_type == 'u')
		{
			if (!AddToken(m_TypeDeclTokens, KEYWORD_cudaReadModeElementType, ref.line))
				return Failed(m_TypeDeclTokens);
		}
		else
		{
			if (!AddToken(m_TypeDeclTokens, KEYWORD_cudaReadModeNormalizedFloat, ref.line))
				return Failed(m_TypeDeclTokens);
		}
		if (!m_TypeDeclTokens.Add(cmpToken_Comma, ref.line))
			return Failed(m_TypeDeclTokens);

		// Generate a unique type name and add as a symbol token
		char type_name[64];
		sprintf(type_name, "__TextureTypeName_%d__", unique_index);
		m_Name = String(type_name);
		if (!m_TypeDeclTokens.Add(cmpToken_Symbol, m_Name.text, m_Name.length, ref.line))
			return Failed(m_TypeDeclTokens);

		// Close the statement
		if (!m_TypeDeclTokens.Add(cmpToken_RBracket, ref.line))
			return Failed(m_TypeDeclTokens);
		if (!m_TypeDeclTokens.Add(cmpToken_SemiColon, ref.line))
			return Failed(m_TypeDeclTokens);

		// Create the containing node (to be deleted by the parse tree)
		cmpNode* type_node;
		m_LastError = cmpNode_CreateEmpty(&type_node);
		if (!cmpError_OK(&m_LastError))
			return false;
		type_node->type = cmpNode_UserTokens;
		type_node->first_token = m_TypeDeclTokens.first;
		type_node->last_token = m_TypeDeclTokens.last;

		// Add right before the containing parent
		cmpNode* insert_before_node = FindContainerParent(ref.node);
		if (insert_before_node == NULL)
		{
			m_LastError = cmpError_Create("Failed to find good location for type declaration");
			return false;
		}
		cmpNode_AddBefore(insert_before_node, type_node);

		return true;
	}


	bool ReplaceTypeInstance(const TextureRef& ref)
	{
		// Earlier errors should have caught the absence of a containing parent
		cmpNode* container_parent = FindContainerParent(ref.node);
		assert(container_parent != 0);

		// function decl: remove parameter
		// function defn: remove parameter, insert global definition, insert local param

		// Is this a kernel function definition/declaration?
		if (ref.node->type == cmpNode_FunctionParams &&
			(container_parent->type == cmpNode_FunctionDefn ||
			 container_parent->type == cmpNode_FunctionDecl))
		{
			TokenIterator i(*container_parent);
			i.SkipWhitespace();
			if (i.token != NULL && i.token->hash == KEYWORD_cmp_kernel_fn.hash)
				return ReplaceKernelParameter(ref, container_parent);
		}

		// Create the single replacement token
		cmpToken* token;
		m_LastError = cmpToken_Create(&token, cmpToken_Symbol, m_Name.text, m_Name.length, ref.line);
		if (!cmpError_OK(&m_LastError))
			return false;

		// Cut out the original tokens and replace with the new one
		TokenList old_tokens(ref.keyword_token, ref.end_of_type_token);
		token->prev = old_tokens.first->prev;
		token->prev->next = token;
		token->next = old_tokens.last->next;
		token->next->prev = token;
		old_tokens.DeleteAll();

		return true;
	}


	bool ReplaceKernelParameter(const TextureRef& ref, cmpNode* function_node)
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
		const Keyword* keyword = &KEYWORD_cmp_kernel_texture_decl_comma;
		if (old_tokens.last->type == cmpToken_RBracket)
		{
			keyword = &KEYWORD_cmp_kernel_texture_decl;
			old_tokens.last = ref.name_token;
		}

		// All token replacement happens on the same line 
		cmpU32 line = ref.name_token->line;

		// Start of the replacement tokens
		TokenList new_tokens;
		if (AddToken(new_tokens, *keyword, line) == 0)
			return Failed(new_tokens);
		if (new_tokens.Add(cmpToken_LBracket, line) == 0)
			return Failed(new_tokens);

		// Add texture dimensions
		cmpU32 dimensions = ref.Dimensions();
		const Keyword* kw_dimensions = GetDimensionsKeyword(dimensions);
		if (new_tokens.Add(cmpToken_Number, kw_dimensions->text, kw_dimensions->length, line) == 0)
			return Failed(new_tokens);
		if (new_tokens.Add(cmpToken_Comma, line) == 0)
			return Failed(new_tokens);

		// Finish with the parameter name
		if (new_tokens.Add(cmpToken_Symbol, name_token->start, name_token->length, line) == 0)
			return Failed(new_tokens);
		if (new_tokens.Add(cmpToken_RBracket, line) == 0)
			return Failed(new_tokens);

		// Replace the old tokens with the new ones
		old_tokens.first->prev->next = new_tokens.first;
		new_tokens.first->prev = old_tokens.first->prev;
		old_tokens.last->next->prev = new_tokens.last;
		new_tokens.last->next = old_tokens.last->next;

		// As the new tokens are being placed in the main token list, they'll get cleaned up safely
		// What remains is to clean the old tokens that were pulled out
		old_tokens.DeleteAll();

		return true;
	}


	cmpU32 TextureRefsKey() const
	{
		return m_TextureRefsKey;
	}


	cmpError LastError() const
	{
		return m_LastError;
	}


private:
	bool Failed(const TokenList& list)
	{
		// Record error for future inspection
		m_LastError = list.error;
		return false;
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


	// Key used to lookup texture refs that use this type
	cmpU32 m_TextureRefsKey;

	// Name of the uniquely generated string type
	String m_Name;

	// Tokens created for the unique typedef
	TokenList m_TypeDeclTokens;

	cmpError m_LastError;
};


class TextureTransform : public ITransform
{
public:
	TextureTransform()
		: m_UniqueTypeIndex(0)
	{
	}

private:
	cmpError Apply(ComputeProcessor& processor)
	{
		// Find all texture references
		FindTextureRefs ftr;
		if (!processor.VisitNodes(&ftr))
			return ftr.LastError();
		const TextureRefsMap& refs_map = ftr.Results();
		if (refs_map.size() == 0)
			return cmpError_CreateOK();

		// Build a list of all unique texture types introduced
		for (TextureRefsMap::const_iterator i = refs_map.begin(); i != refs_map.end(); ++i)
		{
			const TextureRefs& refs = i->second;

			// Generate a texture type from the first instance of this texture reference
			const TextureRef& first_ref = FindFirstTextureRef(refs);
			TextureType texture_type(i->first);

			// Place a type declaration somewhere before the first node
			if (!texture_type.AddTypeDeclaration(first_ref, m_UniqueTypeIndex++))
				return texture_type.LastError();

			m_TextureTypes.push_back(texture_type);
		}

		// Replace the type of all texture references with the newly generated unique ones
		for (size_t i = 0; i < m_TextureTypes.size(); i++)
		{
			TextureType& texture_type = m_TextureTypes[i];
			const TextureRefs& refs = refs_map.find(texture_type.TextureRefsKey())->second;

			for (size_t j = 0; j < refs.size(); j++)
			{
				if (!texture_type.ReplaceTypeInstance(refs[j]))
					return texture_type.LastError();
			}
		}

		return cmpError_CreateOK();
	}

	// Used to generate unique type names for texture references
	int m_UniqueTypeIndex;

	std::vector<TextureType> m_TextureTypes;
};


// Register transform
static TransformDesc<TextureTransform> g_TextureTransform;
