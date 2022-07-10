////////////////////////////////////////////////////////////////////////////////
//	File			:	DXRenderStatic.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Static Mesh
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _DXRENDERSTATIC_H_
#define _DXRENDERSTATIC_H_

// Includes
#include "RenderNode.h"

// Foward Declares
class DXRenderContext;
class DXMesh;

// Render Static
class DXRenderStatic : public RenderNode
{
protected:
	DXRenderContext *m_pRenderContext;	// Associated Context
	DXMesh *m_pMesh;					// Mesh
	float m_Color[4];					// Color

public:
	
	// Constructor
	DXRenderStatic(void);

	// Destructor
	virtual ~DXRenderStatic(void);

	// Accessors
	DXMesh* GetMesh(void)						  { return m_pMesh;			 }
	DXRenderContext* GetRenderContext(void) const {	return m_pRenderContext; }
	const float* GetColor(void)					  { return m_Color;			 }

	// Mutators
	void SetMesh(DXMesh *_pMesh)							{ m_pMesh = _pMesh;					  }
	void SetRenderContext(DXRenderContext *_pRenderContext)	{ m_pRenderContext = _pRenderContext; }
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