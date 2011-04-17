#include "RenderSet.h"
#include "RenderNode.h"

#define NULL 0

RenderSet::RenderSet(void) : m_pHead(NULL), m_pTail(NULL)
{
	ClearRenderSet();
}

RenderSet::~RenderSet(void)
{
	ClearRenderSet();
}

void RenderSet::ClearRenderSet(void)
{
	m_pHead = m_pTail = NULL;
	//while(NULL != m_pHead)	// While the node is a valid pointer
	//{
	//	RenderNode * toClear = m_pHead;
	//	
	//	m_pHead = m_pHead->GetNext();

	//	delete toClear;
	//}
}

void RenderSet::AddRenderNode(RenderNode *pNode)
{
	if(m_pTail)
		m_pTail->SetNext(pNode);
	else
		m_pHead = pNode;
	m_pTail = pNode;
	pNode->SetNext(NULL);
}

