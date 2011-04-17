#ifndef _RENDERSET_H_
#define _RENDERSET_H_

class RenderNode;

class RenderSet
{
	friend class Renderer;
protected:
	RenderNode *m_pHead;
	RenderNode *m_pTail;
public:
	RenderSet(void);
	~RenderSet(void);

	virtual void AddRenderNode(RenderNode *pNode);
	void ClearRenderSet(void);
	RenderNode *GetHead() 
	{ 
		return m_pHead; 
	}
};

#endif	// _RENDERSET_H_