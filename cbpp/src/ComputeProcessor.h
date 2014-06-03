
#ifndef INCLUDED_COMPUTE_PROCESSOR_H
#define INCLUDED_COMPUTE_PROCESSOR_H


#include "../../lib/ComputeParser.h"
#include <vector>


class ComputeProcessor;


struct ITransform
{
	virtual void Apply(ComputeProcessor& processor) = 0;
};


struct INodeVisitor
{
	virtual void Visit(cmpNode& node) = 0;
};


class ComputeProcessor
{
public:
	ComputeProcessor();
	~ComputeProcessor();

	bool ParseFile(const char* filename, bool verbose);
	void ApplyTransforms();
	void VisitNodes(INodeVisitor* visitor);

private:

	// Parser runtime
	cmpMemoryFile* m_MemoryFile;
	cmpLexerCursor* m_LexerCursor;
	cmpParserCursor* m_ParserCursor;

	// Linked list of tokens
	cmpToken* m_FirstToken;
	cmpToken* m_LastToken;

	// Abstract syntax tree
	cmpNode* m_RootNode;

	// List of active transforms
	std::vector<ITransform*> m_Transforms;
};



//
// Iterator for tokens of a given node.
//
struct TokenIterator
{
	TokenIterator(cmpNode& node);

	//
	// Moves the iterator along to the first token that matches the predicate.
	// This will return a pointer to the located token.
	// If the token can't be found, the iterator will be moved to the end of the node and return NULL.
	//
	template <typename MATCH>
	cmpToken* SeekToken(const MATCH& match)
	{
		while (token < last_token)
		{
			if (match(*token))
				return token;
			token = token->next;
		}

		return 0;
	}

	//
	// Skips all whitespace, checking to see if the token that follows matches the predicate.
	// Returns a pointer to the next token beyond the whitespace if it matches.
	// If there is no match NULL is returned but the iterator stays over that next token.
	//
	template <typename MATCH>
	cmpToken* ExpectToken(const MATCH& match)
	{
		SkipWhitespace();

		if (token != last_token && match(*token))
			return token;

		return 0;
	}

	cmpToken* SkipWhitespace();

	operator bool () const;

	TokenIterator& operator ++ ();

	cmpToken* first_token;
	cmpToken* last_token;
	cmpToken* token;
};



//
// Checks a token field specified by the template parameters to see if it is equal to any one of the
// constructor-initialised values.
//
template <typename TYPE, TYPE cmpToken::*PTR>
struct MatchValues
{
	static const cmpU32 MAX_VALUES = 8;

	MatchValues()
		: nb_values(0)
	{
	}
	MatchValues(TYPE h0)
		: nb_values(1)
	{
		values[0] = h0;
	}
	MatchValues(TYPE h0, TYPE h1)
		: nb_values(2)
	{
		values[0] = h0;
		values[1] = h1;
	}
	MatchValues(TYPE h0, TYPE h1, TYPE h2)
		: nb_values(3)
	{
		values[0] = h0;
		values[1] = h1;
		values[2] = h2;
	}
	MatchValues(TYPE h0, TYPE h1, TYPE h2, TYPE h3)
		: nb_values(4)
	{
		values[0] = h0;
		values[1] = h1;
		values[2] = h2;
		values[3] = h3;
	}
	MatchValues(TYPE h0, TYPE h1, TYPE h2, TYPE h3, TYPE h4)
		: nb_values(5)
	{
		values[0] = h0;
		values[1] = h1;
		values[2] = h2;
		values[3] = h3;
		values[4] = h4;
	}
	MatchValues(TYPE h0, TYPE h1, TYPE h2, TYPE h3, TYPE h4, TYPE h5)
		: nb_values(6)
	{
		values[0] = h0;
		values[1] = h1;
		values[2] = h2;
		values[3] = h3;
		values[4] = h4;
		values[5] = h5;
	}
	MatchValues(TYPE h0, TYPE h1, TYPE h2, TYPE h3, TYPE h4, TYPE h5, TYPE h6)
		: nb_values(7)
	{
		values[0] = h0;
		values[1] = h1;
		values[2] = h2;
		values[3] = h3;
		values[4] = h4;
		values[5] = h5;
		values[6] = h6;
	}
	MatchValues(TYPE h0, TYPE h1, TYPE h2, TYPE h3, TYPE h4, TYPE h5, TYPE h6, TYPE h7)
		: nb_values(8)
	{
		values[0] = h0;
		values[1] = h1;
		values[2] = h2;
		values[3] = h3;
		values[4] = h4;
		values[5] = h5;
		values[6] = h6;
		values[7] = h7;
	}

	bool operator () (const cmpToken& token) const
	{
		for (cmpU32 i = 0; i < nb_values; i++)
		{
			if ((token.*PTR) == values[i])
				return true;
		}

		return false;
	}

	TYPE values[MAX_VALUES];
	cmpU32 nb_values;
};



//
// Match specialisations for token types and hashes
//
typedef MatchValues<cmpTokenType, &cmpToken::type> MatchTypes;
typedef MatchValues<cmpU32, &cmpToken::hash> MatchHashes;



//
// Type that records the functions required to create and destroy transforms.
//
struct TransformDescBase
{
	typedef ITransform* (*NewFunc)();
	typedef void (*DeleteFunc)(ITransform*);

	TransformDescBase(NewFunc new_func, DeleteFunc delete_func);

	NewFunc new_func;
	DeleteFunc delete_func;
};


//
// Create a static instance of this type to auto-register your transform.
//
template <typename TYPE>
struct TransformDesc : public TransformDescBase
{
	static ITransform* New()
	{
		return static_cast<ITransform*>(new TYPE());
	}
	static void Delete(ITransform* transform)
	{
		delete static_cast<TYPE*>(transform);
	}

	TransformDesc()
		: TransformDescBase(New, Delete)
	{
	}
};

#endif
