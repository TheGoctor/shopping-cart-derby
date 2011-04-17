#include "DXRenderShape.h"
#include "DXRenderContext.h"
#include "DXMesh.h"
#include "Camera.h"

#include "Direct3DManager.h"

#define			LIGHT_COLOR			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define			AMBIENT_COLOR		D3DXCOLOR(0.35f, 0.35f, 0.35f, 1.0f)
#define			DEFAULT_MTRL		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define			LIGHT_DIR			D3DXVECTOR3(-0.2f, 1.0f, 0.0f)

DXRenderShape::DXRenderShape(void)
{

}

DXRenderShape::~DXRenderShape(void)
{
}

DXRenderShape::DXRenderShape(CFrame* frame)
{
	
}


void DXRenderShape::RenderVertsHelper(DXMesh * pMesh)
{
	// Get Device
	LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

	// Render Verts
	pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
	pDev->SetVertexDeclaration(pMesh->GetVertDecl());
	pDev->DrawPrimitive(pMesh->GetMode(), 0, pMesh->GetTriangleCount());
}

void DXRenderShape::RenderIndexedVertsHelper(DXMesh * pMesh)
{
	// Get Device
	LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

	// Render Indexed Verts
	pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
	pDev->SetIndices(pMesh->GetIndexBuffer());
	pDev->SetVertexDeclaration(pMesh->GetVertDecl());
	pDev->DrawIndexedPrimitive(pMesh->GetMode(), 0, 0, pMesh->GetVertCount(), 0, pMesh->GetTriangleCount());
}

void DXRenderShape::SetMVPHelper(DXRenderShape &shapeNode)
{
	// Get Context
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();

	// Set MVP
	D3DXMATRIX mMVP = shapeNode.GetFrame()->GetWorldMatrix();
	mMVP = mMVP * CCamera::GetInstance()->GetViewProjectionMatrix();
	pContextNode->SetMVP(mMVP);
}

void DXRenderShape::SetWorldHelper(DXRenderShape & shapeNode)
{
	// Get Context
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();

	// Set World
	D3DXMATRIX mWorld = shapeNode.GetFrame()->GetWorldMatrix();
	D3DXMatrixInverse(&mWorld, 0, &mWorld);
	D3DXMatrixTranspose(&mWorld, &mWorld);
	pContextNode->SetWorld(mWorld);
}

void DXRenderShape::SetLightHelper(DXRenderContext * pContextNode)
{
	pContextNode->SetAmbientColor(AMBIENT_COLOR);
	pContextNode->SetDiffuseColor(LIGHT_COLOR);
	pContextNode->SetLightDirection(LIGHT_DIR);
	pContextNode->SetDiffuseMtrl(LIGHT_COLOR);
}

void DXRenderShape::SetTextureHelper(DXRenderContext * pContextNode)
{
	ID3DXEffect * pEffect = pContextNode->GetShader();

	for(unsigned int tex = 0; tex < DXRenderContext::MAX_TEXTURES; ++tex)
	{
		LPDIRECT3DTEXTURE9 refTexture = pContextNode->GetTexture(tex);

		if(NULL != refTexture)
		{
			pEffect->SetTexture(pContextNode->GetTexUniform(tex).c_str(), refTexture);
		}
	}
}

void DXRenderShape::VertsRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderShape & shapeNode = (DXRenderShape&)node;
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();
	DXMesh * pMesh = shapeNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		SetMVPHelper(shapeNode);

		// Commit Changes
		pEffect->CommitChanges();

		// Render Verts
		RenderVertsHelper(pMesh);

		pEffect->EndPass();
	}
}

void DXRenderShape::IndexedVertsRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderShape & shapeNode = (DXRenderShape&)node;
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();
	DXMesh * pMesh = shapeNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		SetMVPHelper(shapeNode);

		// Commit Changes
		pEffect->CommitChanges();

		// Render Verts
		RenderIndexedVertsHelper(pMesh);

		pEffect->EndPass();
	}
}

void DXRenderShape::VertsTextureRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderShape & shapeNode = (DXRenderShape&)node;
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();
	DXMesh * pMesh = shapeNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		SetMVPHelper(shapeNode);

		// Set Textures
		SetTextureHelper(pContextNode);

		// Commit Changes
		pEffect->CommitChanges();

		// Render Verts
		RenderVertsHelper(pMesh);	

		pEffect->EndPass();
	}
}

void DXRenderShape::VertsLitTextureRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderShape & shapeNode = (DXRenderShape&)node;
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();
	DXMesh * pMesh = shapeNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		SetMVPHelper(shapeNode);
		SetWorldHelper(shapeNode);
		SetLightHelper(pContextNode);

		// Set Textures
		SetTextureHelper(pContextNode);

		// Commit Changes
		pEffect->CommitChanges();

		// Render Verts
		RenderVertsHelper(pMesh);

		pEffect->EndPass();
	}
}

void DXRenderShape::IndexedVertsTextureRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderShape & shapeNode = (DXRenderShape&)node;
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();
	DXMesh * pMesh = shapeNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		SetMVPHelper(shapeNode);

		// Set Textures
		SetTextureHelper(pContextNode);

		// Commit Changes
		pEffect->CommitChanges();

		// Render Verts
		RenderIndexedVertsHelper(pMesh);

		pEffect->EndPass();
	}
}

void DXRenderShape::IndexedVertsLitTexturedRenderFunc(RenderNode &node)
{
	// Get Nodes
	DXRenderShape & shapeNode = (DXRenderShape&)node;
	DXRenderContext * pContextNode = shapeNode.GetRenderContext();
	DXMesh * pMesh = shapeNode.GetMesh();
	ID3DXEffect * pEffect = pContextNode->GetShader();

	UINT passes = 0;
	pEffect->Begin(&passes, 0);
	for(UINT i=0; i<passes; i++)
	{
		pEffect->BeginPass(i);

		// Set Values
		SetMVPHelper(shapeNode);
		SetWorldHelper(shapeNode);
		SetLightHelper(pContextNode);

		// Set Textures
		SetTextureHelper(pContextNode);

		// Commit Changes
		pEffect->CommitChanges();

		// Render Verts
		RenderIndexedVertsHelper(pMesh);

		pEffect->EndPass();
	}
}