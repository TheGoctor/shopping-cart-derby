////////////////////////////////////////////////////////////////////////////////
//	File			:	CPeanutButterVFXComp.cpp
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Peanut Butter Effect
////////////////////////////////////////////////////////////////////////////////
#include "peanut_butter_effect.h"

#include "core/object_manager.h"
#include "rendering/effect_manager.h"

#include <iostream>

namespace scd {
// Initalize
void CPeanutButterVFXComp::Init(void) {
  // Get Singletons
  CObjectManager* pOM = CObjectManager::GetInstance();
  CEventManager* pEM = CEventManager::GetInstance();
  CEffectManager* pEffMan = CEffectManager::GetInstance();

  // Left
  string szObjName = "LeftSkidMarkPBEmitter";
  szObjName += (char*)m_pParentObj->GetID();
  m_pLeftSkidEmitter = pOM->CreateObject(szObjName);
  pOM->BindObjects(m_pParentObj, m_pLeftSkidEmitter);
  m_pLeftSkidEmitter->GetTransform()->TranslateFrame(
      CSkidMarks::FindLeftEmitterPos(m_eCharacterSetType));

  m_pLeftMud = pEffMan->CreateSlowMudComp(m_pLeftSkidEmitter);

  // Right
  szObjName = "RightSkidMarkPBEmitter";
  szObjName += (char*)m_pParentObj->GetID();
  m_pRightSkidEmitter = pOM->CreateObject(szObjName);
  pOM->BindObjects(m_pParentObj, m_pRightSkidEmitter);
  m_pRightSkidEmitter->GetTransform()->TranslateFrame(
      CSkidMarks::FindRightEmitterPos(m_eCharacterSetType));

  m_pRightMud = pEffMan->CreateSlowMudComp(m_pRightSkidEmitter);

  // Pair
  m_pSkidMeshPair = MMNEW(CSkidMeshPair);
  m_pSkidMeshPair->SetWidth(FindWidth(m_eCharacterSetType));
  m_pSkidMeshPair->SetFrames(
      m_pLeftSkidEmitter->GetTransform(), m_pRightSkidEmitter->GetTransform());
  m_pSkidMeshPair->CreateMeshs(RC_PB_MARKS);

  // Register for Events

  // Update
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, UpdateCallback);

  // Slow
  pEM->RegisterEvent("SlowEffect", this, SlowCallback);

  // Inv
  pEM->RegisterEvent("Invulnerable", this, InvulnerableCallback);

  // Destroy Obj
  pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

// Shutdown
void CPeanutButterVFXComp::Shutdown(void) {
  // Get Singletons
  CEventManager* pEM = CEventManager::GetInstance();
  CObjectManager* pOM = CObjectManager::GetInstance();

  // Unregister for Events
  pEM->UnregisterEventAll(this);

  // Destroy Objs
  pOM->DestroyObject(m_pLeftSkidEmitter);
  pOM->DestroyObject(m_pRightSkidEmitter);

  // Clean up Skid Marks
  if (m_pSkidMeshPair) {
    MMDEL(m_pSkidMeshPair);
  }
}

// Factory
int CPeanutButterVFXComp::CreatePBVFXComponent(lua_State* pLua) {
  CObject* pParent = (CObject*)lua_topointer(pLua, -2);
  ECharacterSet eType = (ECharacterSet)lua_tointeger(pLua, -1);

  lua_pop(pLua, 2);

  CreatePBVFXComponent(pParent, eType);

  return 0;
}

// Update
void CPeanutButterVFXComp::UpdateCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Component from Event
  CPeanutButterVFXComp* pPBVFXC = (CPeanutButterVFXComp*)pComp;

  // Get Data from Event
  TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
  float fDT = pUpdateEvent->m_fDeltaTime;

  // Update Component
  pPBVFXC->Update(fDT);
}
void CPeanutButterVFXComp::Update(float fDT) {
  // We are Curenttly PBed
  if (m_fDuration > 0.0f) {
    // Decrease Duration
    m_fDuration -= fDT;

    // Just Stopped
    if (m_fDuration < 0.0f) {
      m_pSkidMeshPair->SetFading(true);
      m_fDuration = 0.0f;

      m_pLeftMud->SwitchOnOffEmitters(EC_TYPE_CART_MUD, false);
      m_pRightMud->SwitchOnOffEmitters(EC_TYPE_CART_MUD, false);
    } else // Still Slowed
    {
      m_pSkidMeshPair->GrowSkids();
    }
  }

  // Pair
  m_pSkidMeshPair->UpdateAlpha(fDT);

  if (m_pSkidMeshPair->IsActive()) {
    m_pSkidMeshPair->AddToSet();
  }
}

// Slow
void CPeanutButterVFXComp::SlowCallback(IEvent* pEvent, IComponent* pComp) {
  // Get the Values from the Event
  CPeanutButterVFXComp* pPBVFXC = (CPeanutButterVFXComp*)pComp;
  pPBVFXC->Slow((TStatusEffectEvent*)pEvent->GetData());
}
void CPeanutButterVFXComp::Slow(TStatusEffectEvent* ptSEEvent) {
  // Check if we are Slowing
  if (CSkidMarks::CheckForPlayerMatch(m_pParentObj, ptSEEvent->m_pObject)) {
    m_fDuration = ptSEEvent->m_fDuration;
    m_pSkidMeshPair->ResetMeshes();
    m_pLeftMud->SwitchOnOffEmitters(EC_TYPE_CART_MUD, true);
    m_pRightMud->SwitchOnOffEmitters(EC_TYPE_CART_MUD, true);
  }
}

// Invulnerable
void CPeanutButterVFXComp::InvulnerableCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get the Values from the Event
  CPeanutButterVFXComp* pPBVFXC = (CPeanutButterVFXComp*)pComp;
  pPBVFXC->Invulnerable((TStatusEffectEvent*)pEvent->GetData());
}
void CPeanutButterVFXComp::Invulnerable(TStatusEffectEvent* ptSEEvent) {
  // Check if we are Slowing
  if (CSkidMarks::CheckForPlayerMatch(m_pParentObj, ptSEEvent->m_pObject)
      && m_pSkidMeshPair->IsActive()) {
    m_pSkidMeshPair->GrowSkids();
    m_pSkidMeshPair->SetFading(true);
    m_fDuration = 0.0f;
    m_pLeftMud->SwitchOnOffEmitters(EC_TYPE_CART_MUD, false);
    m_pRightMud->SwitchOnOffEmitters(EC_TYPE_CART_MUD, false);
  }
}

// Destroy Object
void CPeanutButterVFXComp::DestroyObjectCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get Component from Event
  CPeanutButterVFXComp* pPBVFXC = (CPeanutButterVFXComp*)pComp;
  TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();

  // If our Parent is being Destroyed
  if (pPBVFXC->GetParent()->GetID() == pData->m_pcObj->GetID()) {
    // Shutdown Component
    pPBVFXC->Shutdown();
  }
}

// Find Width
float CPeanutButterVFXComp::FindWidth(ECharacterSet eType) {
  float fWidth;

  switch (eType) {
  case CS_BIKERS:
    fWidth = 0.1f;
    break;
  case CS_SCIENTISTS:
    fWidth = 0.2f;
    break;
  case CS_BANDITOS:
    fWidth = 0.05f;
    break;
  case CS_LARPERS:
    fWidth = 0.05f;
    break;
  case CS_SASHA:
    fWidth = 0.075f;
    break;
  default:
    fWidth = 0.1f;
    break;
  }

  return fWidth;
}

} // namespace scd
