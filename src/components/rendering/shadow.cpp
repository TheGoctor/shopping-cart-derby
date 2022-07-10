////////////////////////////////////////////////////////////////////////////////
//	File			:	CShadowComp.cpp
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Shadow
////////////////////////////////////////////////////////////////////////////////
#include "shadow.h"

#include "components/rendering/renderable.h"
#include "core/object_manager.h"
#include "rendering/dx_mesh.h"
#include "rendering/model_manager.h"
#include "rendering/renderer.h"

namespace scd {
// Get Mesh Index
DXMesh* CShadowComp::GetShadowMeshByCharacterSet(ECharacterSet eType) {
  string szMeshName = "";
  switch (eType) {
  case CS_BIKERS:
    szMeshName = "FFP_3D_BikerShadow_FIN";
    break;
  case CS_BANDITOS:
    szMeshName = "FFP_3D_BanditosShadow_FIN";
    break;
  case CS_LARPERS:
    szMeshName = "FFP_3D_LarperShadow_FIN";
    break;
  case CS_SCIENTISTS:
    szMeshName = "FFP_3D_ScientistShadow_FIN";
    break;
  case CS_SASHA:
    szMeshName = "FFP_3D_SashaShadow_FIN";
    break;
  default:
    szMeshName = "";
    break;
  }

  // Get Model Manager
  ModelManager* pMM = ModelManager::GetInstance();
  int nMeshIdx = pMM->GetMeshIndexByName(szMeshName.c_str(), false, false);
  return pMM->GetMesh(nMeshIdx);
}

// Initalize
void CShadowComp::Init(void) {
  // Get Singletons
  CObjectManager* pOM = CObjectManager::GetInstance();
  CEventManager* pEM = CEventManager::GetInstance();

  // Create Shadow Obj
  string szObjName = "Shadow";
  szObjName += (char)m_eType;
  szObjName += (char*)m_pParent->GetID();
  m_pShadowObj = pOM->CreateObject(szObjName);
  pOM->BindObjects(m_pParent, m_pShadowObj);
  // pShadowObj->GetTransform()->ScaleFrame(3.0f, 3.0f, 3.0f);
  m_pShadowObj->GetTransform()->TranslateFrame(D3DXVECTOR3(0.0f, 0.1f, 0.0f));

  // Get Mesh
  DXMesh* pShadowMesh = GetShadowMeshByCharacterSet(m_eType);

  // Create Render Comp
  m_pRenComp = Renderer::GetInstance()->CreateRenderComp(
      m_pShadowObj,
      pShadowMesh,
      RC_CHARACTER_SHADOW_BLOB,
      RF_INDEXED_VERT_TEX2);

  // Register for Events

  // Update
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, UpdateCallback);

  // Exit Gameplay
  szEventName = "ShutdownObjects";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, ShutdownCallback);
}

// Shutdown
void CShadowComp::ShutdownCallback(IEvent* pEvent, IComponent* pComp) {
  CShadowComp* pSC = (CShadowComp*)pComp;
  CObjectManager::GetInstance()->DestroyObject(pSC->m_pShadowObj);
  pSC->Shutdown();
}
void CShadowComp::Shutdown(void) {
  // Unregister for Events
  CEventManager::GetInstance()->UnregisterEventAll(this);
}

// Factory
int CShadowComp::CreateShadowComponent(lua_State* pLua) {
  CObject* pParent = (CObject*)lua_topointer(pLua, -2);
  ECharacterSet eType = (ECharacterSet)lua_tointeger(pLua, -1);

  lua_pop(pLua, 2);

  CreateShadowComponent(pParent, eType);

  return 0;
}

// Callbacks

// Update
void CShadowComp::UpdateCallback(IEvent* /*pEvent*/, IComponent* pComp) {
  // Get Component from Event
  CShadowComp* pSC = (CShadowComp*)pComp;
  pSC->Update();
}
void CShadowComp::Update(void) {
  // Render the Shadow
  m_pRenComp->AddToRenderSet();
}
} // namespace scd
