////////////////////////////////////////////////////////////////////////////////
//	File			:	RenderNode.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Chaining of Renderable Object
////////////////////////////////////////////////////////////////////////////////

// File Header
#ifndef _RENDERNODE_H_
#define _RENDERNODE_H_

// Render Node
class RenderNode
{
private:
	friend class Renderer;

protected:
	RenderNode *m_pNext; // Pointer to the Next Node

public:

	// Constructor
	RenderNode() : m_pNext(0) {}

	// Render Function Pointer
	void (*RenderFunc)(RenderNode &node);

	// Depth Render Function Pointer
	void (*DepthRenderFunc)(RenderNode &, RenderNode &);

	// Render the Node with the Associated Render Function
	void RenderProcess() { RenderFunc(*this); }

	// Accessor
	inline RenderNode *GetNext(void) { return m_pNext; }

	// Mutator
	inline void SetNext(RenderNode *pNode) { m_pNext = pNode; }
};

#endif	// _RENDERNODE_H_