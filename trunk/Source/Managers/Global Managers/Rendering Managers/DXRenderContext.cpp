////////////////////////////////////////////////////////////////////////////////
//	File			:	DXRenderContext.cpp
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Set of Meshs
////////////////////////////////////////////////////////////////////////////////

// Includes
#include <fstream> 
using namespace std;
#include "DXRenderContext.h"
#include "Direct3DManager.h"
#include "DXUtil.h"
#include "Renderer.h"

// Default Constructor
DXRenderContext::DXRenderContext(void) : RenderNode(), m_pShaderEffect(NULL), m_nTexID(1)
{
	for(unsigned int tex = 0; tex < MAX_TEXTURES; ++tex)
	{
		m_Textures[tex] = NULL;
		m_szTexUniforms[tex] = "";
	}
}

// Destructor
DXRenderContext::~DXRenderContext(void)
{
	//m_pRenderSet->ClearRenderSet();

	// Release Shader
	//SAFE_RELEASE(m_pShaderEffect);
	//SAFE_DELETE(m_pRenderSet);

	// Release Textures
	for(unsigned int tex = 0; tex < MAX_TEXTURES; ++tex)
	{
		if(NULL != m_Textures[tex])
		{
			SAFE_RELEASE(m_Textures[tex]);
		}
	}
}

// Add Texture
bool DXRenderContext::AddTexture(const char *pUniformName, const char *pFileName)
{
	// Get Device
	LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

	// Find the best Spot
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

	// Create Texture
	if( bestIndex != -1 )
	{
		D3DXCreateTextureFromFile(pDev, pFileName, &m_Textures[bestIndex]);	
		m_szTexUniforms[bestIndex] = pUniformName;
		return true;
	}

	return false;
}

// Load Shader
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

// Helper Funcs
void DXRenderContext::RenderContextFuncHelper(RenderNode &node, const char* szTech)
{
	// Get Context & Effect
	DXRenderContext& contextNode = (DXRenderContext&)node;
	ID3DXEffect* pEffect = contextNode.GetShader();

	// Use Shader
	if(pEffect)
	{
		// Set Technique
		pEffect->SetTechnique(szTech);

		// Draw
		Renderer::Render(contextNode.GetRenderSet());

		// End Effect
		pEffect->End();
	}
}

// Context Funcs
void DXRenderContext::FlatRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "Flat");	
}
void DXRenderContext::TexturedRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "AlphaTest");
}
void DXRenderContext::ParticleContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "Particle");
}
void DXRenderContext::ParticleZWriteContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "ParticleZWrite");
}
void DXRenderContext::ParticleAlphaContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "AlphaParticle");
}
void DXRenderContext::LitTexturedRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "Tex2DDiffuse");
}
void DXRenderContext::AnimationRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "Animated");
}
void DXRenderContext::AnimationAlphaRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "AnimatedAlphaCart");
}
void DXRenderContext::TextureChangeContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "TextureChange");
}
void DXRenderContext::BlueLightContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "BlueLight");
}

//void DXRenderContext::SkyBoxRenderContextFunc(RenderNode &node)
//{
//	RenderContextFuncHelper(node, "AlphaBlend");
//}
void DXRenderContext::GroundRenderContextFunc(RenderNode &node)
{
	RenderContextFuncHelper(node, "MultiTexDiffuse");
}