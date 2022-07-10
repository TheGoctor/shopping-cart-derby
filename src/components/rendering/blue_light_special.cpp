////////////////////////////////////////////////////////////////////////////////
//	File			:	blue_light_special.cpp
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Blue Light Special for
//Held Items
////////////////////////////////////////////////////////////////////////////////
#include "blue_light_special.h"

#include "core/object_manager.h"
#include "rendering/dx_render_shape.h"
#include "rendering/model_manager.h"
#include "rendering/renderer.h"

///////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////
#define BLS_POS_OFFSET (D3DXVECTOR3(-0.494f, 3.282f, 0.059f))
#define BLS_ROT_AXIS (D3DXVECTOR3(0.0f, 1.0f, 0.0f))
#define BLS_ROT_RATE (3.14f)

namespace scd {

// Initalize
void blue_light_special::Init(void) {
  // Get Singletons
  CObjectManager* pOM = CObjectManager::GetInstance();
  Renderer* pRM = Renderer::GetInstance();
  ModelManager* pMM = ModelManager::GetInstance();
  CEventManager* pEM = CEventManager::GetInstance();

  // Create Blue Light Obj
  string szObjName = "BlueLightSpecial";
  szObjName += (char*)m_pParentObj->GetID();
  m_pBLSObj = pOM->CreateObject(szObjName);

  // Position Light Relative to Endcap
  pOM->BindObjects(m_pParentObj, m_pBLSObj);
  m_pBLSObj->GetTransform()->TranslateFrame(BLS_POS_OFFSET);

  // Get Blue Light Mesh
  int nMeshIdx =
      pMM->GetMeshIndexByName("FFP_3D_BlueLamp_FINShape", false, false);

  // Create Render Comp
  m_pRenComp = pRM->CreateRenderComp(
      m_pBLSObj, nMeshIdx, RC_BLUE_LIGHT_SPECIAL, RF_TEXTURE_CHANGE_INST);

  // Register for Events

  // Update Gameplay
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, UpdateCallback);

  // BLS
  pEM->RegisterEvent("BlueLightSpecialSpawned", this, ActivateCallback);
  pEM->RegisterEvent("BlueLightSpecialDespawned", this, DeactivateCallback);
}

///////////////////////////////////////////////////////////////////////////////
// Event Callbacks
///////////////////////////////////////////////////////////////////////////////

// Update Gameplay
void blue_light_special::UpdateCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Component from Event
  blue_light_special* pBLSComp = (blue_light_special*)pComp;

  // Get Data from Event
  TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
  float fDT = pUpdateEvent->m_fDeltaTime;

  // Update Component
  if (pBLSComp->IsActive())
    pBLSComp->Update(fDT);
}

// Active BLS
void blue_light_special::ActivateCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get Listener and Data from Event
  blue_light_special* pBLSComp = (blue_light_special*)pComp;
  TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();

  // If the Parents are the Same then we are related Components
  if (pBLSComp->GetParent() == pData->m_pcObj) {
    // Turn on Component and Change Texture to On
    pBLSComp->SetActive(true);
    pBLSComp->m_pEndcapRenComp->GetRenderShape()->SetTexIdx(2);
  }
}

// Deactivate BLS
void blue_light_special::DeactivateCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get Listener and Data from Event
  blue_light_special* pBLSComp = (blue_light_special*)pComp;
  TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();

  // If the Parents are the Same then we are related Components
  if (pBLSComp->GetParent() == pData->m_pcObj) {
    // Turn on Component and Change Texture to Off
    pBLSComp->SetActive(false);
    pBLSComp->m_pEndcapRenComp->GetRenderShape()->SetTexIdx(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Event Handlers
///////////////////////////////////////////////////////////////////////////////

// Update Gameplay
void blue_light_special::Update(float fDT) {
  // Get BLS Frame
  CFrame* pFrame = m_pBLSObj->GetTransform();

  // Translate and Rotate BLS
  pFrame->TranslateFrame(-BLS_POS_OFFSET);
  pFrame->RotateFrame(BLS_ROT_AXIS, BLS_ROT_RATE * fDT);
  pFrame->TranslateFrame(BLS_POS_OFFSET);

  // Render
  m_pRenComp->AddToRenderSet();
}
} // namespace scd
