
#include "ComputeProcessor.h"

#include <cassert>


namespace
{
	// Persistent text for symbol insertion
	HashString STRING_include("include");
}


class PrologueTransform : public ITransform
{
	cmpError Apply(ComputeProcessor& processor)
	{
		// Locate the first parse node
		cmpNode* root_node = processor.RootNode();
		assert(root_node != 0);
		cmpNode* first_child = root_node->first_child;
		if (first_child == 0)
			return cmpError_CreateOK();

		// Start an include statement
		TokenList tokens;
		tokens.Add(cmpToken_Hash, 0);
		tokens.Add(STRING_include, 0);
		tokens.Add(cmpToken_Whitespace, 0);

		// Add full path to the prologue header file
		std::string prologue_path = "\"";
		prologue_path += processor.ExecutableDirectory();
		prologue_path += "/../inc/Prologue.cuh\"";
		m_ProloguePath = prologue_path;
		tokens.Add(m_ProloguePath, 0);

		tokens.Add(cmpToken_Whitespace, 0);
		tokens.Add(cmpToken_EOL, 0);

		// Create the containing user node (to be deleted by the parse tree)
		cmpNode* node;
		cmpError error = cmpNode_CreateEmpty(&node);
		if (!cmpError_OK(&error))
			throw error;
		node->type = cmpNode_UserTokens;
		node->first_token = tokens.first;
		node->last_token = tokens.last;

		cmpNode_AddBefore(first_child, node);

		return cmpError_CreateOK();
	}

	String m_ProloguePath;
};


// Register transform
static TransformDesc<PrologueTransform> g_Transform;
