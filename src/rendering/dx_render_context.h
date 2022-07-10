#pragma once

#include "core/camera_manager.h"
#include "rendering/direct3d_manager.h"
#include "rendering/render_node.h"

namespace scd {

struct RenNodeCmp {
  bool operator()(const RenderNode* lhs, const RenderNode* rhs) {
    // Get Camera Pos
    scd::transform camMap =
        CCameraManager::GetInstance()->GetCam()->GetViewMatrix();
    D3DXVECTOR3 vCamZVec = D3DXVECTOR3(camMap._31, camMap._32, camMap._33);
    D3DXVECTOR3 vCamVec = D3DXVECTOR3(camMap._41, camMap._42, camMap._43);

    DXRenderShape* pLHSShape = (DXRenderShape*)lhs;
    DXRenderShape* pRHSShape = (DXRenderShape*)rhs;

    D3DXVECTOR3 vLHS = pLHSShape->GetFrame()->GetWorldPosition() - vCamVec;
    D3DXVECTOR3 vRHS = pRHSShape->GetFrame()->GetWorldPosition() - vCamVec;

    // Mags
    // D3DXVec2LengthSq(&vLHS);

    float fLHS = D3DXVec3LengthSq(&vLHS); // D3DXVec3Dot(&vLHS, &vLHS);
    float fRHS = D3DXVec3LengthSq(&vRHS); // D3DXVec3Dot(&vRHS, &vLHS);
    if (fLHS == fRHS) {
      return lhs < rhs;
    }
    return fLHS > fRHS;
  }
};

// Foward Declares
class RenderShape;
typedef std::list<RenderNode*, /* RenNodeCmp,*/ CRenderAllocator<RenderNode*>>
    RenderSet;

// Render Context
class DXRenderContext : public RenderNode {
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
  static void RenderContextFuncHelper(RenderNode& node, const char* szTech);

public:
  // Constructor
  DXRenderContext(void);

  // Destructor
  ~DXRenderContext(void);

  void LoadShader(const char* szFXFile);

  // Render Funcs
  static void FlatRenderContextFunc(RenderNode& node);
  static void TexturedRenderContextFunc(RenderNode& node);
  static void ParticleContextFunc(RenderNode& node);
  static void ParticleZWriteContextFunc(RenderNode& node);
  static void ParticleAlphaContextFunc(RenderNode& node);
  static void LitTexturedRenderContextFunc(RenderNode& node);
  static void AnimationRenderContextFunc(RenderNode& node);
  static void AnimationAlphaRenderContextFunc(RenderNode& node);
  static void TextureChangeContextFunc(RenderNode& node);
  static void BlueLightContextFunc(RenderNode& node);
  // static void SkyBoxRenderContextFunc(RenderNode &node);
  static void GroundRenderContextFunc(RenderNode& node);

  // Add a Texture
  bool AddTexture(
      const char* pUniformName,
      const char* pFileName); //, bool bCubeMap = false );

  // Render Set Funcs
  inline void AddRenderNode(RenderNode* pNode) {
    m_cRenderSet.push_back(pNode);
  }
  inline void ClearRenderSet(void) { m_cRenderSet.clear(); }

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
    // m_pRenderSet = MMNEW(RenderSet);
  }

  // Accessors
  inline ID3DXEffect* GetShader(void) const { return m_pShaderEffect; }
  inline LPDIRECT3DTEXTURE9 GetTexture(unsigned int index = 0) {
    return m_Textures[index];
  }
  inline string GetTexUniform(unsigned int index = 0) {
    return m_szTexUniforms[index];
  }
  inline RenderSet& GetRenderSet(void) { return m_cRenderSet; }
  inline int GetTexIdx(void) { return m_nTexID; }

  // Mutators
  inline void SetTexIdx(int nTexID) { m_nTexID = nTexID; }

  inline void SetMVP(const scd::transform& mvp) {
    m_pShaderEffect->SetMatrix("gWVP", &mvp);
  }

  inline void SetWorld(const scd::transform& world) {
    m_pShaderEffect->SetMatrix("gWorld", &world);
  }

  inline void SetViewPosition(const D3DXVECTOR3& viewPos) {
    m_pShaderEffect->SetValue("gViewPosition", &viewPos, sizeof(D3DXVECTOR3));
  }

  inline void SetFlashing(
      const BOOL& bFlashingY,
      const BOOL& bFlashingW,
      const BOOL& bFlashingB) {
    m_pShaderEffect->SetValue("gbFlashingYellow", &bFlashingY, sizeof(BOOL));
    m_pShaderEffect->SetValue("gbFlashingWhite", &bFlashingW, sizeof(BOOL));
    m_pShaderEffect->SetValue("gbFlashingBlue", &bFlashingB, sizeof(BOOL));
  }

  inline void SetFlashRate(const float& fFlashRate) {
    m_pShaderEffect->SetFloat("gFlashRate", fFlashRate);
  }

  inline void SetLightDirection(const D3DXVECTOR3& LightDir) {
    m_pShaderEffect->SetValue(
        "gLightDirection", &LightDir, sizeof(D3DXVECTOR3));
  }

  inline void SetAmbientColor(const scd::vector4& AmbColor) {
    m_pShaderEffect->SetValue("gAmbientLight", &AmbColor, sizeof(scd::vector4));
  }

  inline void SetDiffuseColor(const scd::vector4& DifColor) {
    m_pShaderEffect->SetValue(
        "diffuseLightColor", &DifColor, sizeof(scd::vector4));
  }

  inline void SetDiffuseMtrl(const scd::vector4& DifMtrl) {
    m_pShaderEffect->SetValue(
        "diffuseMtrlColor", &DifMtrl, sizeof(scd::vector4));
  }

  inline void SetTextureIdx(const int nTexID) {
    m_pShaderEffect->SetInt("nTexNum", nTexID);
  }
};

} // namespace scd
