#ifndef _DXRENDERSTATIC_H_
#define _DXRENDERSTATIC_H_

#include "RenderNode.h"

class DXRenderContext;
class DXMesh;

class DXRenderStatic : public RenderNode
{
protected:
	DXRenderContext *m_pRenderContext;
	DXMesh *m_pMesh;
	
	float m_Color[4];
public:
	DXRenderStatic(void);
	virtual ~DXRenderStatic(void);

	DXMesh *GetMesh(void) 
	{ 
		return m_pMesh; 
	}
	void SetMesh(DXMesh *_pMesh) 
	{ 
		m_pMesh = _pMesh; 
	}
	void SetRenderContext(DXRenderContext *_pRenderContext)
	{ 
		m_pRenderContext = _pRenderContext; 
	}
	DXRenderContext *GetRenderContext(void) const 
	{ 
		return m_pRenderContext; 
	}
	const float *GetColor(void) 
	{ 
		return m_Color; 
	}
	void SetColor(float color4[4]) 
	{
		m_Color[0] = color4[0];
		m_Color[1] = color4[1];
		m_Color[2] = color4[2];
		m_Color[3] = color4[3];
	}

	// Render Funcs
	static void VertsRenderFunc(RenderNode &node);
	static void IndexedVertsRenderFunc(RenderNode &node);
};

#endif	// _DXRENDERSTATIC_H_