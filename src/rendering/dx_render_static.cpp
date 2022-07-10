////////////////////////////////////////////////////////////////////////////////
//	File			:	dx_render_static.cpp
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Data needed to Render a
// Static Mesh
////////////////////////////////////////////////////////////////////////////////
#include "dx_render_static.h"

#include "core/camera_manager.h"
#include "rendering/direct3d_manager.h"
#include "rendering/dx_mesh.h"
#include "rendering/dx_render_context.h"

namespace scd {

// Constructor
dx_render_static::dx_render_static(void)
    : render_node()
    , m_pRenderContext(NULL)
    , m_pMesh(NULL) {
  m_Color[0] = 1.0f;
  m_Color[1] = 1.0f;
  m_Color[2] = 1.0f;
  m_Color[3] = 0.5f;
}

// Destructor
dx_render_static::~dx_render_static(void) {
  // MMDEL(m_pRenderContext);
  // MMDEL(m_pMesh);
}

////////////////////////////////////////////////////////////////////////////////
//	RENDER FUNCS
////////////////////////////////////////////////////////////////////////////////

// Vertex Render Func
void dx_render_static::VertsRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_static& staticNode = (dx_render_static&)node;
  DXRenderContext* pContextNode = staticNode.GetRenderContext();
  DXMesh* pMesh = staticNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  // Get Singletons
  CCamera* pCam = CCameraManager::GetInstance()->GetCam();
  Direct3DManager* pD3DMan = Direct3DManager::GetInstance();
  LPDIRECT3DDEVICE9 pDev = pD3DMan->GetDirect3DDevice();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
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

// Indexed Vertex Render Func
void dx_render_static::IndexedVertsRenderFunc(RenderNode& node) {
  // Get Nodes
  dx_render_static& staticNode = (dx_render_static&)node;
  DXRenderContext* pContextNode = staticNode.GetRenderContext();
  DXMesh* pMesh = staticNode.GetMesh();
  ID3DXEffect* pEffect = pContextNode->GetShader();

  // Get Singletons
  CCamera* pCam = CCameraManager::GetInstance()->GetCam();
  Direct3DManager* pD3DMan = Direct3DManager::GetInstance();
  LPDIRECT3DDEVICE9 pDev = pD3DMan->GetDirect3DDevice();

  UINT passes = 0;
  pEffect->Begin(&passes, 0);
  for (UINT i = 0; i < passes; i++) {
    pEffect->BeginPass(i);

    // Set Values
    pContextNode->SetMVP(pCam->GetViewProjectionMatrix());

    // TODO Set LightMVP

    // Render Verts
    pDev->SetVertexDeclaration(pMesh->GetVertDecl());
    pDev->SetStreamSource(0, pMesh->GetVertBuffer(), 0, pMesh->GetVertSize());
    pDev->SetIndices(pMesh->GetIndexBuffer());
    pDev->DrawIndexedPrimitive(
        pMesh->GetMode(),
        0,
        0,
        pMesh->GetVertCount(),
        0,
        pMesh->GetTriangleCount());

    pEffect->EndPass();
  }
}

} // namespace scd
