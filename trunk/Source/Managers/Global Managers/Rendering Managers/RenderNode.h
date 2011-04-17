// FILE HEADER
#ifndef _RENDERNODE_H_
#define _RENDERNODE_H_

class RenderNode
{
	friend class RenderSet;
	friend class Renderer;
protected:
	RenderNode *m_pNext;
public:

	RenderNode() 
	{ 
		m_pNext = 0; 
	}

	// The RenderFunc pointer
	void (*RenderFunc)(RenderNode &node);
	void (*DepthRenderFunc)(RenderNode &, RenderNode &);
	void RenderProcess() 
	{ 
		RenderFunc(*this); 
	}

	inline RenderNode *GetNext(void) 
	{ 
		return m_pNext; 
	}
	inline void SetNext(RenderNode *pNode) 
	{ 
		m_pNext = pNode; 
	}
};

#endif	// _RENDERNODE_H_