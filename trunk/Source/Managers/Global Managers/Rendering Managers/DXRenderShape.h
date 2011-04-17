#ifndef _DXRENDERSHAPE_H_
#define _DXRENDERSHAPE_H_

#include "dxrenderstatic.h"
#include "..\\..\\..\\CFrame.h"

class DX_Mesh;
class DXRenderContext;

typedef void(*pfRenderFunc)(RenderNode&);

enum ERenderFunc { RF_MIN = -1, RF_VERT, RF_INDEXED_VERT, RF_MAX };

class DXRenderShape : public DXRenderStatic
{
private:

	// Pointer to the Frame
	CFrame* m_Frame;

	// Helper Funcs
	static void RenderVertsHelper(DXMesh * pMesh);
	static void RenderIndexedVertsHelper(DXMesh * pMesh);
	static void SetMVPHelper(DXRenderShape & shapeNode);
	static void SetWorldHelper(DXRenderShape & shapeNode);
	static void SetLightHelper(DXRenderContext * pContextNode);
	static void SetTextureHelper(DXRenderContext * pContextNode);

public:
	DXRenderShape(void);
	~DXRenderShape(void);
	DXRenderShape(CFrame* frame);

	#pragma region Inline Methods
	inline CFrame* GetFrame(void) 
	{ 
		return m_Frame; 
	}

	void SetFrame(CFrame* pFrame)
	{
		m_Frame = pFrame;
	}

	#pragma endregion

	// Render Funcs
	static void VertsRenderFunc(RenderNode &node);
	static void IndexedVertsRenderFunc(RenderNode &node);
	static void VertsTextureRenderFunc(RenderNode &node);
	static void VertsLitTextureRenderFunc(RenderNode &node);
	static void IndexedVertsTextureRenderFunc(RenderNode &node);
	static void IndexedVertsLitTexturedRenderFunc(RenderNode &node);

	// Accessor
	static pfRenderFunc GetRenderFunc(ERenderFunc eRenderFunc)
	{
		static void (*m_pfRenderFuncs[RF_MAX])(RenderNode&) = { VertsRenderFunc, IndexedVertsRenderFunc };
		return m_pfRenderFuncs[eRenderFunc]; 
	}
};

#endif	// _DXRENDERSHAPE_H_