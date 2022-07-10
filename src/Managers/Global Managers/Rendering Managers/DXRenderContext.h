////////////////////////////////////////////////////////////////////////////////
//	File			:	DXRenderContext.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a Set of Meshs
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _DXRENDERCONTEXT_H_
#define _DXRENDERCONTEXT_H_

// Includes
#include <string>
#include <set>
#include <list>
using namespace std;
#include "RenderNode.h"
#include "DXRenderShape.h"
#include "Direct3DManager.h"
#include "..\Memory Manager\CRenderAllocator.h"
#include "..\Camera Manager\CCameraManager.h"

struct RenNodeCmp
{
	bool operator()(const RenderNode* lhs, const RenderNode* rhs)
	{
		// Get Camera Pos
		D3DXMATRIX camMap = CCameraManager::GetInstance()->GetCam()->GetViewMatrix(); 
		D3DXVECTOR3 vCamZVec  = D3DXVECTOR3(camMap._31, camMap._32, camMap._33);
		D3DXVECTOR3 vCamVec	  = D3DXVECTOR3(camMap._41, camMap._42, camMap._43);

		DXRenderShape* pLHSShape = (DXRenderShape*)lhs;
		DXRenderShape* pRHSShape = (DXRenderShape*)rhs;

		D3DXVECTOR3 vLHS = pLHSShape->GetFrame()->GetWorldPosition() - vCamVec;
		D3DXVECTOR3 vRHS = pRHSShape->GetFrame()->GetWorldPosition() - vCamVec;

		// Mags
		//D3DXVec2LengthSq(&vLHS);

		float fLHS = D3DXVec3LengthSq(&vLHS); //D3DXVec3Dot(&vLHS, &vLHS);
		float fRHS = D3DXVec3LengthSq(&vRHS); //D3DXVec3Dot(&vRHS, &vLHS);
		if(fLHS == fRHS)
		{
			return lhs < rhs;
		}
		return fLHS > fRHS;
	}
};

//struct RenNodeCmp
//{
//	bool operator()(const RenderNode* lhs, const RenderNode* rhs)
//	{
//		// Get Camera Pos
//		D3DXMATRIX camMap = CCameraManager::GetInstance()->GetCam()->GetViewMatrix(); 
//		D3DXVECTOR3 vCamVec  = D3DXVECTOR3(camMap._31, camMap._32, camMap._33);
//		D3DXVECTOR3 vCamZVec = D3DXVECTOR3(camMap._41, camMap._42, camMap._43);
//
//		DXRenderShape* pLHSShape = (DXRenderShape*)lhs;
//		DXRenderShape* pRHSShape = (DXRenderShape*)rhs;
//
//		D3DXVECTOR3 vLHS = pLHSShape->GetFrame()->GetWorldPosition() - vCamZVec;
//		D3DXVECTOR3 vRHS = pRHSShape->GetFrame()->GetWorldPosition() - vCamZVec;
//
//		float fLHS = D3DXVec3Dot(&vLHS, &vCamVec);
//		float fRHS = D3DXVec3Dot(&vRHS, &vCamVec);
//		if(fLHS == fRHS)
//		{
//			return fLHS < fRHS;
//		}
//		return fLHS < fRHS;
//	}
//};

// Foward Declares
class RenderShape;
typedef std::list<RenderNode*, /* RenNodeCmp,*/ CRenderAllocator<RenderNode*>> RenderSet;

// Render Context
class DXRenderContext : public RenderNode
{
public:

	// Max Number of Textures
	const static int MAX_TEXTURES = 5;

	friend class DXRenderContextManager;

protected: 

	// Shader Effect
	ID3DXEffect* m_pShaderEffect;

	// Render Set
	RenderSet m_cRenderSet;

	// Textures
	LPDIRECT3DTEXTURE9 m_Textures[MAX_TEXTURES];
	string m_szTexUniforms[MAX_TEXTURES];

	int m_nTexID;

	// Helper Funcs
	static void RenderContextFuncHelper(RenderNode &node, const char* szTech);

public:

	// Constructor
	DXRenderContext(void);

	// Destructor
	~DXRenderContext(void);

	void LoadShader(const char *szFXFile);

	// Render Funcs
	static void FlatRenderContextFunc(RenderNode &node);
	static void TexturedRenderContextFunc(RenderNode &node);
	static void ParticleContextFunc(RenderNode &node);
	static void ParticleZWriteContextFunc(RenderNode &node);
	static void ParticleAlphaContextFunc(RenderNode &node);
	static void LitTexturedRenderContextFunc(RenderNode &node);
	static void AnimationRenderContextFunc(RenderNode &node);
	static void AnimationAlphaRenderContextFunc(RenderNode &node);
	static void TextureChangeContextFunc(RenderNode &node);
	static void BlueLightContextFunc(RenderNode &node);
	//static void SkyBoxRenderContextFunc(RenderNode &node);
	static void GroundRenderContextFunc(RenderNode &node);

	// Add a Texture
	bool AddTexture( const char *pUniformName, const char *pFileName);//, bool bCubeMap = false );
	
	// Render Set Funcs
	inline void AddRenderNode(RenderNode *pNode) { m_cRenderSet.push_back(pNode);	  }
	inline void ClearRenderSet(void)			 { m_cRenderSet.clear();		  }

	// Create Render Set
	inline void CreateRenderSet(/*bool bSorted = false, 
		bool (*SortFunc)(RenderShape *, RenderShape *) = 0*/)
	{
		/*if(bSorted)
		{
			m_pRenderSet = new RenderSetSorted;
			((RenderSetSorted *)m_pRenderSet)->SortFunc = SortFunc;
		}
		else*/
			//m_pRenderSet = MMNEW(RenderSet);
	}

	// Accessors
	inline ID3DXEffect* GetShader(void) const					 { return m_pShaderEffect;		  }
	inline LPDIRECT3DTEXTURE9 GetTexture(unsigned int index = 0) { return m_Textures[index];	  }
	inline string GetTexUniform(unsigned int index = 0)			 { return m_szTexUniforms[index]; } 
	inline RenderSet &GetRenderSet(void)						 { return m_cRenderSet;			  }
	inline int GetTexIdx(void) { return m_nTexID; }

	// Mutators
	inline void SetTexIdx(int nTexID) { m_nTexID = nTexID; }

	inline void SetMVP(const D3DXMATRIX &mvp)
	{
		m_pShaderEffect->SetMatrix("gWVP", &mvp);
	}

	inline void SetWorld(const D3DXMATRIX &world)
	{
		m_pShaderEffect->SetMatrix("gWorld", &world);
	}

	inline void SetViewPosition(const D3DXVECTOR3 &viewPos)
	{
		m_pShaderEffect->SetValue("gViewPosition", &viewPos, sizeof(D3DXVECTOR3));
	}

	inline void SetFlashing(const BOOL &bFlashingY, const BOOL &bFlashingW, const BOOL &bFlashingB)
	{
		m_pShaderEffect->SetValue("gbFlashingYellow", &bFlashingY, sizeof(BOOL));
		m_pShaderEffect->SetValue("gbFlashingWhite", &bFlashingW, sizeof(BOOL));
		m_pShaderEffect->SetValue("gbFlashingBlue", &bFlashingB, sizeof(BOOL));
	}

	inline void SetFlashRate(const float &fFlashRate)
	{
		m_pShaderEffect->SetFloat("gFlashRate", fFlashRate);
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

	inline void SetTextureIdx(const int nTexID)
	{
		m_pShaderEffect->SetInt("nTexNum", nTexID);
	}
};

#endif	// _DXRENDERCONTEXT_H_