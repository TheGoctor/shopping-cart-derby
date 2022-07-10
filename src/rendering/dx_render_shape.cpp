////////////////////////////////////////////////////////////////////////////////
//	File			:	dx_render_shape.cpp
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data and Functionality
// for Rendering 						a Mesh with a certain
// Context
////////////////////////////////////////////////////////////////////////////////
#include "dx_render_shape.h"

// Includes
#include "core/camera_manager.h"
#include "rendering/direct3d_manager.h"
#include "rendering/dx_mesh.h"
#include "rendering/dx_render_context.h"
#include "rendering/particle_emitter.h"

// Defines
#define LIGHT_COLOR scd::vector4(1.0f, 1.0f, 1.0f, 1.0f)
#define AMBIENT_COLOR scd::vector4(0.35f, 0.35f, 0.35f, 1.0f)
#define DEFAULT_MTRL scd::vector4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT_DIR D3DXVECTOR3(-0.2f, 1.0f, 0.0f)

namespace scd {

// Default Constructoe
dx_render_shape::dx_render_shape(void) {}

// Destructor
dx_render_shape::~dx_render_shape(void) {}

// Constructor
dx_render_shape::dx_render_shape(CFrame* /*frame*/)
    : m_nTexID(1) {}

////////////////////////////////////////////////////////////////////////////////
//	RENDER HELPER FUNCS
////////////////////////////////////////////////////////////////////////////////

// Draw Verts
void dx_render_shape::RenderVertsHelper(DXMesh* pMesh) {
  // Get Device
  LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

  // Render Verts
  pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
  pDev->SetVertexDeclaration(pMesh->GetVertDecl());
  pDev->DrawPrimitive(pMesh->GetMode(), 0, pMesh->GetTriangleCount());
}

// Draw Indexed Verts
void dx_render_shape::RenderIndexedVertsHelper(DXMesh* pMesh) {
  // Get Device
  LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

  // Render Indexed Verts
  pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
  pDev->SetIndices(pMesh->GetIndexBuffer());
  pDev->SetVertexDeclaration(pMesh->GetVertDecl());
  pDev->DrawIndexedPrimitive(
      pMesh->GetMode(),
      0,
      0,
      pMesh->GetVertCount(),
      0,
      pMesh->GetTriangleCount());
}

// Draw Particles
void dx_render_shape::RenderParticlesHelper(DXMesh* pMesh) {
  // Get Device
  LPDIRECT3DDEVICE9 pDev = Direct3DManager::GetInstance()->GetDirect3DDevice();

  // Render Indexed Verts
  pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
  pDev->SetIndices(pMesh->GetIndexBuffer());
  pDev->SetVertexDeclaration(pMesh->GetVertDecl());

  // Get Blend Modes
  DWORD dwPrevSrc;
  DWORD dwPrevDest;
  // Direct3DManager* pD3DMan = Direct3DManager::GetInstance();
  pDev->GetRenderState(D3DRS_SRCBLEND, &dwPrevSrc);
  pDev->GetRenderState(D3DRS_DESTBLEND, &dwPrevDest);

  // Set Blend Modes
  // CParticleEmitter* pPE = (CParticleEmitter*)pMesh;
  pDev->SetRenderState(
      D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR); // pPE->GetSourceBlendMode());
  pDev->SetRenderState(
      D3DRS_DESTBLEND,
      D3DBLEND_INVSRCALPHA); // pPE->GetDestinationBlendMode());

  pDev->DrawIndexedPrimitive(
      pMesh->GetMode(),
      0,
      0,
      pMesh->GetVertCount(),
      0,
      pMesh->GetTriangleCount());

  // Set Blend Modes Back
  pDev->SetRenderState(D3DRS_SRCBLEND, dwPrevSrc);
  pDev->SetRenderState(D3DRS_DESTBLEND, dwPrevDest);
}

////////////////////////////////////////////////////////////////////////////////
//	SET SHADER VALUES FUNCS
////////////////////////////////////////////////////////////////////////////////

// Set Model View Projection Matrix
void dx_render_shape::SetMVPHelper(dx_render_shape& shapeNode) {
  // Get Context
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();

  // Set MVP
  CCamera* pCam = CCameraManager::GetInstance()->GetCam();
  scd::transform mMVP = shapeNode.GetFrame()->GetWorldMatrix();
  mMVP = mMVP * pCam->GetViewProjectionMatrix();
  D3DXVECTOR3 vViewPos = D3DXVECTOR3(
      pCam->GetViewMatrix()._31,
      pCam->GetViewMatrix()._32,
      pCam->GetViewMatrix()._33);
  vViewPos *= -1.0f;

  D3DXVec3Normalize(&vViewPos, &vViewPos);
  pContextNode->SetViewPosition(vViewPos);
  pContextNode->SetMVP(mMVP);
}

// Set View Projection Matrix
void dx_render_shape::SetVPHelper(dx_render_shape& shapeNode) {
  // Get Context
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();

  // Set MVP
  scd::transform mVP =
      CCameraManager::GetInstance()->GetCam()->GetViewProjectionMatrix();
  pContextNode->SetMVP(mVP);
}

// Set World Matrix
void dx_render_shape::SetWorldHelper(dx_render_shape& shapeNode) {
  // Get Context
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();

  // Set World
  scd::transform mWorld = shapeNode.GetFrame()->GetWorldMatrix();
  D3DXMatrixInverse(&mWorld, 0, &mWorld);
  D3DXMatrixTranspose(&mWorld, &mWorld);
  pContextNode->SetWorld(mWorld);
}

// Set Light
void dx_render_shape::SetLightHelper(DXRenderContext* pContextNode) {
  pContextNode->SetAmbientColor(AMBIENT_COLOR);
  pContextNode->SetDiffuseColor(LIGHT_COLOR);
  pContextNode->SetLightDirection(LIGHT_DIR);
  pContextNode->SetDiffuseMtrl(LIGHT_COLOR);
}

// Set Textures
void dx_render_shape::SetTextureHelper(DXRenderContext* pContextNode) {
  ID3DXEffect* pEffect = pContextNode->GetShader();

  for (unsigned int tex = 0; tex < DXRenderContext::MAX_TEXTURES; ++tex) {
    LPDIRECT3DTEXTURE9 refTexture = pContextNode->GetTexture(tex);

    if (NULL != refTexture) {
      pEffect->SetTexture(pContextNode->GetTexUniform(tex).c_str(), refTexture);
    }
  }
}

// Set MVP Bones
void dx_render_shape::SetMVPBoneHelper(dx_render_shape& shapeNode, CFrame& frame) {
  // Get Context
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();

  // Set MVP
  scd::transform mMVP = frame.GetWorldMatrix();
  mMVP =
      mMVP * CCameraManager::GetInstance()->GetCam()->GetViewProjectionMatrix();
  pContextNode->SetMVP(mMVP);
}

////////////////////////////////////////////////////////////////////////////////
//	RENDER FUNCS
////////////////////////////////////////////////////////////////////////////////

// Vertex Render Func
void dx_render_shape::VertsRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Indexed Vertex Render Func
void dx_render_shape::IndexedVertsRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Textured Vertex Render Func
void dx_render_shape::VertsTextureRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Lit Textured Vertex Render Func
void dx_render_shape::VertsLitTextureRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Textured Indexed Vertex Render Func
void dx_render_shape::IndexedVertsTextureRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Lit Textured Indexed Vertex Render Func
void dx_render_shape::IndexedVertsLitTexturedRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Particle Render Func
void dx_render_shape::ParticleRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
    pEffect->BeginPass(i);

    // Set Values
    SetVPHelper(shapeNode);

    // Set Textures
    SetTextureHelper(pContextNode);

    // Commit Changes
    pEffect->CommitChanges();

    // Render Verts
    RenderParticlesHelper(pMesh);

    pEffect->EndPass();
  }
}

// Bone Spheres Render Func
void dx_render_shape::BonesRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  // DXRenderContext * pContextNode = shapeNode.GetRenderContext();
  // DXMesh * pMesh = shapeNode.GetMesh();
  // ID3DXEffect * pEffect = pContextNode->GetShader();

  LPD3DXMESH sphere;
  D3DXCreateSphere(
      Direct3DManager::GetInstance()->GetDirect3DDevice(),
      0.1f,
      13,
      13,
      &sphere,
      NULL);

  RenderBonesHelper(shapeNode, *shapeNode.GetFrame(), sphere);
}
void dx_render_shape::RenderBonesHelper(
    dx_render_shape& shapeNode,
    CFrame& frame,
    LPD3DXMESH sphere) {
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
    pEffect->BeginPass(i);

    // Set Values
    SetMVPBoneHelper(shapeNode, frame);
    // SetWorldBoneHelper(shapeNode, frame);

    // Commit Changes
    pEffect->CommitChanges();

    // Render Sphere
    sphere->DrawSubset(0);

    pEffect->EndPass();
  }

  list<CFrame*, scd::allocator<CFrame*>>::iterator childIter =
      frame.GetChildren()->begin();

  while (childIter != frame.GetChildren()->end()) {
    RenderBonesHelper(shapeNode, *(*childIter), sphere);

    childIter++;
  }
}

// TEST Render Func
void dx_render_shape::AnimatedCharacterRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
    pEffect->BeginPass(i);

    // Set Values
    SetVPHelper(shapeNode);
    // SetWorldHelper(shapeNode);

    pContextNode->SetFlashing(
        pMesh->IsFlashingYellow(),
        pMesh->IsFlashingWhite(),
        pMesh->IsFlashingBlue());
    pContextNode->SetFlashRate(pMesh->GetFlashRate());

    // Set Textures
    SetTextureHelper(pContextNode);

    // Commit Changes
    pEffect->CommitChanges();

    // Render Verts
    RenderIndexedVertsHelper(pMesh);

    pEffect->EndPass();
  }

  // BonesRenderFunc(node);
}

void dx_render_shape::TextureChangeRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
    pEffect->BeginPass(i);

    // Set Values
    SetMVPHelper(shapeNode);

    // Set Textures
    SetTextureHelper(pContextNode);

    pContextNode->SetTextureIdx(pContextNode->GetTexIdx());

    // Commit Changes
    pEffect->CommitChanges();

    // Render Verts
    RenderIndexedVertsHelper(pMesh);

    pEffect->EndPass();
  }
}

void dx_render_shape::TextureChangeInstRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_shape& shapeNode = (dx_render_shape&)node;
  DXRenderContext* pContextNode = shapeNode.GetRenderContext();
  DXMesh* pMesh = shapeNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
    pEffect->BeginPass(i);

    // Set Values
    SetMVPHelper(shapeNode);

    // Set Textures
    SetTextureHelper(pContextNode);

    pContextNode->SetTextureIdx(shapeNode.GetTexIdx());

    // Commit Changes
    pEffect->CommitChanges();

    // Render Verts
    RenderIndexedVertsHelper(pMesh);

    pEffect->EndPass();
  }
}

} // namespace scd
