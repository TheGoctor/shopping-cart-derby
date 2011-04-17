#include "DXRenderContext.h"

#include <fstream> 
using namespace std;

#include "Direct3DManager.h"
#include "DXUtil.h"
#include "Renderer.h"

DXRenderContext::DXRenderContext(void) 
				: m_pShaderEffect(NULL)
{
	for(unsigned int tex = 0; tex < MAX_TEXTURES; ++tex)
	{
		m_Textures[tex] = NULL;
		m_szTexUniforms[tex] = "";
	}
}

DXRenderContext::~DXRenderContext(void)
{
	//m_pRenderSet->ClearRenderSet();
	SAFE_RELEASE(m_pShaderEffect);
	//SAFE_DELETE(m_pRenderSet);

	for(unsigned int tex = 0; tex < MAX_TEXTURES; ++tex)
	{
		if(NULL != m_Textures[tex])
		{
			SAFE_RELEASE(m_Textures[tex]);
		}
	}
}

bool DXRenderContext::AddTexture(const char *pUniformName, const char *pFileName, bool bCubeMap)
{
	LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

	int bestIndex = -1;
	for(int i = MAX_TEXTURES-1; i >= 0; --i)
	{
		if( m_Textures[i] == NULL)
			bestIndex = i;
		else if( m_szTexUniforms[i] == pUniformName )
		{
			bestIndex = i;
			break; 
		}
	}

	if( bestIndex != -1 )
	{
		D3DXCreateTextureFromFile(pDev, pFileName, &m_Textures[bestIndex]);	
		m_szTexUniforms[bestIndex] = pUniformName;
		return true;
	}

	return false;
}

void DXRenderContext::LoadShader(const char *szFXFile)
{
	// Get Singletons
	Direct3DManager * pD3DMan = Direct3DManager::GetInstance();

	// Check for Vaild File Names
	if(NULL == szFXFile)
	{
		// Error
		MessageBoxA(0, szFXFile, "Trying to Load a Non-Existant Shader File", MB_OK);
		return; // Return Failure
	}

	// Create Shader
	ID3DXBuffer *errors;
	D3DXCreateEffectFromFile(pD3DMan->GetDirect3DDevice(), szFXFile, 0, 0, D3DXSHADER_DEBUG, 0, &m_pShaderEffect, &errors);
	if (errors)
	{
		MessageBox(0, (LPCSTR)errors->GetBufferPointer(),0,0);
	}
}

void DXRenderContext::RenderContextFuncHelper(RenderNode &node, const char * szTech)
{
	// Get Context & Effect
	DXRenderContext & contextNode = (DXRenderContext&)node;
	ID3DXEffect * pEffect = contextNode.GetShader();

	// Use Shader
	if(pEffect)
	{
		pEffect->SetTechnique(szTech);

		// Draw
		Renderer::Render(contextNode.GetRenderSet());

		pEffect->End();
	}
}

void DXRenderContext::FlatRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "Flat");	
}

void DXRenderContext::TexturedRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "AlphaTest");
}

void DXRenderContext::LitTexturedRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "Tex2DDiffuse");
}

void DXRenderContext::SkyBoxRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "AlphaBlend");
}

void DXRenderContext::GroundRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "MultiTexDiffuse");
}
