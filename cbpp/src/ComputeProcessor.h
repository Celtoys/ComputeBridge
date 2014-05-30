
#ifndef INCLUDED_COMPUTE_PROCESSOR_H
#define INCLUDED_COMPUTE_PROCESSOR_H


struct cmpNode;
struct cmpMemoryFile;
struct cmpLexerCursor;
struct cmpParserCursor;
struct cmpToken;


#include <vector>


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
	void VisitNodes(INodeVisitor* visitor);

private:

	// Parser runtime
	cmpMemoryFile* m_MemoryFile;
	cmpLexerCursor* m_LexerCursor;
	cmpParserCursor* m_ParserCursor;

	// Generated tokens and AST
	std::vector<cmpToken> m_Tokens;
	std::vector<cmpNode*> m_Nodes;
};


#endif
