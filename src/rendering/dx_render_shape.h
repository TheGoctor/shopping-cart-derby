////////////////////////////////////////////////////////////////////////////////
//	File			:	DXRenderShape.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data and Functionality
//for Rendering 						a Mesh with a certain Context
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "rendering/dx_render_static.h"

namespace scd {
// Foward Declares
class DX_Mesh;
class DXRenderContext;
typedef void (*pfRenderFunc)(RenderNode&);

// Render Shape
class dx_render_shape : public DXRenderStatic {
private:
  // Pointer to the Frame
  CFrame* m_pFrame;

  // Texture ID
  int m_nTexID;

  // Helper Funcs
  static void RenderVertsHelper(DXMesh* pMesh);
  static void RenderIndexedVertsHelper(DXMesh* pMesh);
  static void SetMVPHelper(DXRenderShape& shapeNode);
  static void SetVPHelper(DXRenderShape& shapeNode);
  static void SetWorldHelper(DXRenderShape& shapeNode);
  static void SetLightHelper(DXRenderContext* pContextNode);
  static void SetTextureHelper(DXRenderContext* pContextNode);

  static void RenderParticlesHelper(DXMesh* pMesh);

  static void SetMVPBoneHelper(DXRenderShape& shapeNode, CFrame& frame);
  static void SetWorldBoneHelper(DXRenderShape& shapeNode, CFrame& frame);
  static void
  RenderBonesHelper(DXRenderShape& shapeNode, CFrame& frame, LPD3DXMESH sphere);

public:
  // Constructors
  DXRenderShape(void);
  DXRenderShape(CFrame* frame);

  // Destructor
  ~DXRenderShape(void);

  // Render Funcs
  static void VertsRenderFunc(RenderNode& node);
  static void IndexedVertsRenderFunc(RenderNode& node);
  static void VertsTextureRenderFunc(RenderNode& node);
  static void VertsLitTextureRenderFunc(RenderNode& node);
  static void IndexedVertsTextureRenderFunc(RenderNode& node);
  static void IndexedVertsLitTexturedRenderFunc(RenderNode& node);

  static void AnimatedCharacterRenderFunc(RenderNode& node);
  static void TextureChangeRenderFunc(RenderNode& node);
  static void TextureChangeInstRenderFunc(RenderNode& node);

  static void ParticleRenderFunc(RenderNode& node);

  // Testing Animation Hiarch
  static void BonesRenderFunc(RenderNode& node);

// Accessors
#pragma region Inline Methods
  inline CFrame* GetFrame(void) { return m_pFrame; }
#pragma endregion
  static pfRenderFunc GetRenderFunc(ERenderFunc eRenderFunc) {
    static void (*m_pfRenderFuncs[RF_MAX])(RenderNode&) = {
        VertsRenderFunc,
        IndexedVertsRenderFunc,
        IndexedVertsTextureRenderFunc,
        BonesRenderFunc,
        AnimatedCharacterRenderFunc,
        ParticleRenderFunc,
        TextureChangeRenderFunc,
        VertsTextureRenderFunc,
        TextureChangeInstRenderFunc};
    return m_pfRenderFuncs[eRenderFunc];
  }
  inline int GetTexIdx(void) { return m_nTexID; }

  // Mutators
  void SetFrame(CFrame* pFrame) { m_pFrame = pFrame; }
  inline void SetTexIdx(int nTexID) { m_nTexID = nTexID; }
};

} // namespace scd
