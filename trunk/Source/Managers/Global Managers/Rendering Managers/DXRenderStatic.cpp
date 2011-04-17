#include "DXRenderStatic.h"

#include "Direct3DManager.h"
#include "Camera.h"
#include "DXRenderContext.h"
#include "DXMesh.h"

DXRenderStatic::DXRenderStatic(void)
{
	m_Color[0] = 1.0f;
	m_Color[1] = 1.0f;
	m_Color[2] = 1.0f;
	m_Color[3] = 0.5f;
}

DXRenderStatic::~DXRenderStatic(void)
{
}

void DXRenderStatic::VertsRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderStatic & staticNode = (DXRenderStatic&)node;
	DXRenderContext * pContextNode = staticNode.GetRenderContext();
	DXMesh * pMesh = staticNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	// Get Singletons
	CCamera * pCam = CCamera::GetInstance();
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();
	LPDIRECT3DDEVICE9 pDev = pD3DMan->GetDirect3DDevice();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		pContextNode->SetMVP(pCam->GetViewProjectionMatrix());

		// TODO Set LightMVP

		// Render Verts
		pDev->SetVertexDeclaration(pMesh->GetVertDecl());
		pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
		pDev->DrawPrimitive(pMesh->GetMode(), 0, pMesh->GetTriangleCount());

		pEffect->EndPass();
	}
}

void DXRenderStatic::IndexedVertsRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderStatic & staticNode = (DXRenderStatic&)node;
	DXRenderContext * pContextNode = staticNode.GetRenderContext();
	DXMesh * pMesh = staticNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	// Get Singletons
	CCamera * pCam = CCamera::GetInstance();
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();
	LPDIRECT3DDEVICE9 pDev = pD3DMan->GetDirect3DDevice();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		pContextNode->SetMVP(pCam->GetViewProjectionMatrix());

		// TODO Set LightMVP

		// Render Verts
		pDev->SetVertexDeclaration(pMesh->GetVertDecl());
		pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
		pDev->SetIndices(pMesh->GetIndexBuffer());
		pDev->DrawIndexedPrimitive(pMesh->GetMode(), 0, 0, pMesh->GetVertCount(), 0, pMesh->GetTriangleCount());

		pEffect->EndPass();
	}
}