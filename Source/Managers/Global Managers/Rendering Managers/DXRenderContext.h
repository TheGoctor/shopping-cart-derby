#ifndef _DXRENDERCONTEXT_H_
#define _DXRENDERCONTEXT_H_

#include "RenderNode.h"
#include "RenderSet.h"
#include "Direct3DManager.h"

#include <string>
using namespace std;

class RenderShape;

class DXRenderContext : public RenderNode
{
public:
	const static int MAX_TEXTURES = 4;

protected: 

	// Shader Effect
	ID3DXEffect	* m_pShaderEffect;

	// Render Set
	RenderSet *m_pRenderSet;

	// Textures
	LPDIRECT3DTEXTURE9 m_Textures[MAX_TEXTURES];
	string m_szTexUniforms[MAX_TEXTURES];

	bool LoadShaderFile(const char *szFile);
	static void RenderContextFuncHelper(RenderNode &node, const char * szTech);

public:
	DXRenderContext(void);
	~DXRenderContext(void);

	void LoadShader(const char *szFXFile);

	// Render Funcs
	static void FlatRenderContextFunc(RenderNode &node);
	static void TexturedRenderContextFunc(RenderNode &node);
	static void LitTexturedRenderContextFunc(RenderNode &node);
	static void SkyBoxRenderContextFunc(RenderNode &node);
	static void GroundRenderContextFunc(RenderNode &node);

	bool AddTexture( const char *pUniformName, const char *pFileName, bool bCubeMap = false );

	// Accessors
	inline ID3DXEffect * GetShader(void) const 
	{ 
		return m_pShaderEffect; 
	}
	inline LPDIRECT3DTEXTURE9 GetTexture(unsigned int index = 0)
	{ 
		return m_Textures[index]; 
	}
	inline string GetTexUniform(unsigned int index = 0)
	{ 
		return m_szTexUniforms[index]; 
	}

	inline void CreateRenderSet(bool bSorted = false, 
		bool (*SortFunc)(RenderShape *left, RenderShape *right) = 0)
	{
		/*if(bSorted)
		{
			m_pRenderSet = new RenderSetSorted;
			((RenderSetSorted *)m_pRenderSet)->SortFunc = SortFunc;
		}
		else*/
			m_pRenderSet = MMNEW(RenderSet);
	}
	inline RenderSet &GetRenderSet(void) { return *m_pRenderSet; }
	inline void AddRenderNode(RenderNode *pNode)
	{
		m_pRenderSet->AddRenderNode(pNode);
	}
	inline void ClearRenderSet(void) 
	{ 
		m_pRenderSet->ClearRenderSet();
	}

	inline void SetMVP(const D3DXMATRIX &mvp)
	{
		m_pShaderEffect->SetMatrix("gWVP", &mvp);
	}

	inline void SetWorld(const D3DXMATRIX &world)
	{
		m_pShaderEffect->SetMatrix("gWorld", &world);
	}

	inline void SetLightDirection(const D3DXVECTOR3 &LightDir)
	{
		m_pShaderEffect->SetValue("gLightDirection", &LightDir, sizeof(D3DXVECTOR3));
	}

	inline void SetAmbientColor(const D3DXCOLOR &AmbColor)
	{
		m_pShaderEffect->SetValue("gAmbientLight", &AmbColor, sizeof(D3DXCOLOR));
	}

	inline void SetDiffuseColor(const D3DXCOLOR &DifColor)
	{
		m_pShaderEffect->SetValue("diffuseLightColor", &DifColor, sizeof(D3DXCOLOR));
	}

	inline void SetDiffuseMtrl(const D3DXCOLOR &DifMtrl)
	{
		m_pShaderEffect->SetValue("diffuseMtrlColor", &DifMtrl, sizeof(D3DXCOLOR));
	}
};

#endif	// _DXRENDERCONTEXT_H_