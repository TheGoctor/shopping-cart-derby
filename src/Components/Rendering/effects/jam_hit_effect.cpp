////////////////////////////////////////////////////////////////////////////////
//	File			:	CJamHitEffectComp.cpp
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Jam Hit Effect
////////////////////////////////////////////////////////////////////////////////
#include "jam_hit_effect.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "core/object_manager.h"
#include "rendering/effect_manager.h"
#include "rendering/hud_manager.h"
#include "rendering/renderer.h"

// Defines
#define JAM_HIT_POPUP_TIMER (1.5f)
#define JAM_HIT_FALL_TIMER (1.0f)
#define JHI_DOWN_RATE (-12.5f)

namespace scd {

// Initalize
void CJamHitEffectComp::Init(void) {
  // Get Singletons
  static CEventManager* pEM = CEventManager::GetInstance();
  static CObjectManager* pOM = CObjectManager::GetInstance();
  static CEffectManager* pEffMan = CEffectManager::GetInstance();
  static Renderer* pRenMan = Renderer::GetInstance();

  // Setup Effect Object
  unsigned int uID = m_pParentObj->GetID();
  string szEffectObjName = "JamTrailDown";
  szEffectObjName += (char*)uID;
  m_pDownEffect = pOM->CreateObject(szEffectObjName);
  szEffectObjName = "JamJarDown";
  szEffectObjName += (char*)uID;
  CObject* pJamEffect0 = pOM->CreateObject(szEffectObjName);

  // Bind the Fall to Player
  pOM->BindObjects(m_pParentObj, m_pDownEffect);

  szEffectObjName = "JamHitBlob";
  szEffectObjName += (char*)uID;
  CObject* pJamEffect1 = pOM->CreateObject(szEffectObjName);
  szEffectObjName = "JamSplurchPopup";
  szEffectObjName += (char*)uID;
  CObject* pJamEffect2 = pOM->CreateObject(szEffectObjName);
  szEffectObjName = "JamHitBurst";
  szEffectObjName += (char*)uID;
  CObject* pJamEffect3 = pOM->CreateObject(szEffectObjName);

  // Get Emitters
  CParticleEmitter* pPE0 =
      pEffMan->GetCloneEmitter("Jam_Trail.bin", m_pDownEffect);
  CParticleEmitter* pPE1 =
      pEffMan->GetCloneEmitter("Jam_Hit_Meteor_Down.bin", pJamEffect0);

  CParticleEmitter* pPE2 =
      pEffMan->GetCloneEmitter("Jam_Hit_Splat.bin", pJamEffect1);
  CParticleEmitter* pPE3 =
      pEffMan->GetCloneEmitter("Splurch_Popup.bin", pJamEffect2);
  CParticleEmitter* pPE4 =
      pEffMan->GetCloneEmitter("Jam_Hit_Burst.bin", pJamEffect3);

  // Create Render Comp
  CRenderComponent* pRenComp0 = pRenMan->CreateRenderComp(
      m_pDownEffect, (DXMesh*)pPE0, RC_JAM_HIT_EFFECT_BLOB, RF_PARTICLE);
  CRenderComponent* pRenComp1 = pRenMan->CreateRenderComp(
      pJamEffect0, (DXMesh*)pPE1, RC_JAM_USE_EFFECT_JAR, RF_PARTICLE);

  CRenderComponent* pRenComp2 = pRenMan->CreateRenderComp(
      pJamEffect1, (DXMesh*)pPE2, RC_JAM_HIT_EFFECT_BLOB, RF_PARTICLE);
  CRenderComponent* pRenComp3 = pRenMan->CreateRenderComp(
      pJamEffect2, (DXMesh*)pPE3, RC_JAM_HIT_EFFECT_SPLURCH_POPUP, RF_PARTICLE);
  CRenderComponent* pRenComp4 = pRenMan->CreateRenderComp(
      pJamEffect3, (DXMesh*)pPE4, RC_JAM_HIT_EFFECT_BLOB, RF_PARTICLE);

  // Create Effect Comps
  m_pDownVFX = pEffMan->CreateEffectComp(
      m_pDownEffect,
      pPE0,
      pRenComp0,
      EC_TYPE_JAM_HIT_FALL,
      PE_TYPE_JAM_TRAIL_DOWN);
  m_pDownBVFX = pEffMan->CreateEffectComp(
      m_pDownEffect,
      pPE1,
      pRenComp1,
      EC_TYPE_JAM_HIT_FALL_B,
      PE_TYPE_JAM_HIT_BLOB);
  m_pDownBVFX->SetOnTarget(EC_TYPE_JAM_HIT_FALL_B, true);

  m_pTargetVFX = pEffMan->CreateEffectComp(
      m_pParentObj,
      pPE2,
      pRenComp2,
      EC_TYPE_JAM_HIT_POPUP,
      PE_TYPE_JAM_HIT_BLOB);
  m_pTargetVFX->AddEmitter(
      EC_TYPE_JAM_HIT_POPUP,
      pPE3,
      pRenComp3,
      m_pParentObj->GetTransform(),
      PE_TYPE_JAM_HIT_POPUP);
  m_pTargetVFX->AddEmitter(
      EC_TYPE_JAM_HIT_POPUP,
      pPE4,
      pRenComp4,
      m_pParentObj->GetTransform(),
      PE_TYPE_JAM_BURST);
  m_pTargetVFX->SetOnTarget(EC_TYPE_JAM_HIT_POPUP, true);

  // Register for Events

  // Invulnerable
  pEM->RegisterEvent("Invulnerable", this, InvulnerableCallback);

  // Update Gameplay
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, UpdateCallback);

  // Jam Use
  pEM->RegisterEvent("UseJam", this, JamCallback);

  // Shutdown Gameplay
  szEventName = "ShutdownObjects";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, ShutdownGameplayCallback);
}

// Update Gameplay
void CJamHitEffectComp::UpdateCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Component from Event
  CJamHitEffectComp* pJHVFXC = (CJamHitEffectComp*)pComp;

  // Get Data from Event
  TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
  float fDT = pUpdateEvent->m_fDeltaTime;

  // Update Invulnerability
  if (pJHVFXC->m_fInvDuration > 0.0f) {
    pJHVFXC->m_fInvDuration -= fDT;
    pJHVFXC->m_pDownVFX->SetDeadTimer(
        EC_TYPE_JAM_HIT_FALL, pJHVFXC->m_fInvDuration);
    pJHVFXC->m_pDownBVFX->SetDeadTimer(
        EC_TYPE_JAM_HIT_FALL_B, pJHVFXC->m_fInvDuration);
    pJHVFXC->m_pTargetVFX->SetDeadTimer(
        EC_TYPE_JAM_HIT_POPUP, pJHVFXC->m_fInvDuration);
  }

  // Update Component
  if (pJHVFXC->IsActive())
    pJHVFXC->Update(fDT);
}
void CJamHitEffectComp::Update(float fDT) {
  // Start Timer
  if (m_fStartTimer > 0.0f) {
    m_fStartTimer -= fDT;
    return;
  }

  // Get Frame
  CFrame* pFrame = m_pDownEffect->GetTransform();

  // Translate Down
  pFrame->TranslateFrame(D3DXVECTOR3(0.0f, JHI_DOWN_RATE * fDT, 0.0f));

  // Check for Dead
  if (pFrame->GetLocalPosition().y <= 0.0f) {
    m_bActive = false;
    m_pDownVFX->SwitchOnOffEmitters(EC_TYPE_JAM_HIT_FALL, false);
    m_pDownBVFX->SwitchOnOffEmitters(EC_TYPE_JAM_HIT_FALL_B, false);

    if (m_bDead == false) {
      CWwiseSoundManager::GetInstance()->PlayTheSound(
          ITEM_JAM_HIT,
          CHUDManager::GetInstance()->GetPlayerCharID(
              CHUDManager::GetInstance()->GetPlayerNum(m_pParentObj)));
    }
  }
}

// Invulnerable
void CJamHitEffectComp::InvulnerableCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get Data from Event
  CJamHitEffectComp* pJHVFXC = (CJamHitEffectComp*)pComp;
  TStatusEffectEvent* ptData = (TStatusEffectEvent*)pEvent->GetData();

  // Call Handler
  if (ptData->m_pObject == pJHVFXC->GetParent()) {
    pJHVFXC->m_fInvDuration = ptData->m_fDuration;
  }
}

// Jam Use
void CJamHitEffectComp::JamCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Data from Event
  CJamHitEffectComp* pJHVFXC = (CJamHitEffectComp*)pComp;
  TStatusEffectEvent* ptData = (TStatusEffectEvent*)pEvent->GetData();

  // Call Handler
  if (ptData->m_pObject != pJHVFXC->GetParent()) {
    pJHVFXC->Jam();
  }
}
void CJamHitEffectComp::Jam(void) {
  // Check for Invulnerablility
  if (m_fInvDuration > 0.0f)
    return;

  // Activate
  m_bActive = true;
  m_fStartTimer = JAM_HIT_FALL_TIMER;
  m_pDownEffect->GetTransform()->TranslateFrame(D3DXVECTOR3(0.0f, 9.0f, 0.0f));

  // Show jam firing from user
  m_pDownVFX->SetDeadTimer(EC_TYPE_JAM_HIT_FALL, 0.0f);
  m_pDownBVFX->SetDeadTimer(EC_TYPE_JAM_HIT_FALL_B, 0.0f);
  m_pTargetVFX->SetDeadTimer(EC_TYPE_JAM_HIT_POPUP, 0.0f);
  m_bDead = false;
  m_pDownVFX->SetEffectTimer(EC_TYPE_JAM_HIT_FALL, JAM_HIT_FALL_TIMER);
  m_pDownBVFX->SetEffectTimer(EC_TYPE_JAM_HIT_FALL_B, JAM_HIT_FALL_TIMER);
  m_pTargetVFX->SetEffectTimer(EC_TYPE_JAM_HIT_POPUP, JAM_HIT_POPUP_TIMER);
}

// Shutdown Gameplay
void CJamHitEffectComp::ShutdownGameplayCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  // Get Data from Event
  CJamHitEffectComp* pJHVFXC = (CJamHitEffectComp*)pComp;

  // Kill Effects
  CFrame* pFrame = pJHVFXC->m_pDownEffect->GetTransform();
  pJHVFXC->m_pDownVFX->SetDeadTimer(EC_TYPE_JAM_HIT_FALL, 2.0f);
  pJHVFXC->m_pDownBVFX->SetDeadTimer(EC_TYPE_JAM_HIT_FALL_B, 2.0f);
  pJHVFXC->m_pTargetVFX->SetDeadTimer(EC_TYPE_JAM_HIT_POPUP, 2.0f);
  pJHVFXC->m_bDead = true;
  pJHVFXC->m_fInvDuration = 0.0f;
}

} // namespace scd
