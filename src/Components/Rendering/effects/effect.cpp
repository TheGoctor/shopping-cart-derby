////////////////////////////////////////////////////////////////////////////////
//	File			:	effect.cpp
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Effect
////////////////////////////////////////////////////////////////////////////////

#include "effect.h"

#include "components/rendering/renderable.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/dx_render_context_manager.h"
#include "rendering/dx_render_shape.h"
#include "rendering/particle_emitter.h"

namespace scd {
effect_data::effect_data()
    : m_eType(EC_TYPE_MIN)
    , m_bOn(true)
    , m_fCooldown(0.0f)
    , m_fLifespan(0.0f)
    , m_fTimer(0.0f)
    , m_fDeadTimer(0.0f)
    , m_nEmitterCount(0) {}

effect_data::~effect_data(void) { m_cEmitterRenderComps.clear(); }

// Update Effect
void effect_data::Update(float fDT) {
  // Check for Timer
  if (m_fTimer > 0.0f) {
    m_fTimer -= fDT;
    if (m_fTimer <= 0.0f) {
      SwitchOnOffEmitters(true);
      m_fTimer = 0.0f;
    }
  }

  // Cooldown
  if (m_fCooldown > 0.0f) {
    m_fCooldown -= fDT;
  }

  // Check if its off
  if (m_bOn == false) {
    return;
  }

  // Lifespan
  bool bCutoff = false;
  if (m_fLifespan > 0.0f) {
    m_fLifespan -= fDT;

    if (m_fLifespan <= 0.0f) {
      bCutoff = true;
    }
  }

  // Update and Render all the Emitters
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  int nDeadCount = 0;
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;
    CRenderComponent* pRC = (*emitterIter).second;

    // Update
    if (pPE->IsOn()) {
      pPE->UpdateEmitter(fDT);

      if (m_fDeadTimer > 0.0f) {
        m_fDeadTimer -= fDT;
      } else {
        pRC->AddToRenderSet();
      }
    } else
      nDeadCount++;

    // Cutoff?
    if (bCutoff) {
      pPE->SetContinuous(false);
    }

    // On to the Next 1
    emitterIter++;
  }

  // Kill if all Dead
  if (nDeadCount == m_nEmitterCount && nDeadCount != 0)
    SwitchOnOffEmitters(false);
}

// Shutdown Effect
void effect_data::Shutdown(void) {
  // Get Singletons
  CObjectManager* pOM = CObjectManager::GetInstance();

  // Tell all emitter to shutdown
  while (!m_cEmitterRenderComps.empty()) {
    // Get a Pointer to the Emitter
    CRenderComponent* pRC = (*m_cEmitterRenderComps.begin()).second;

    // If we have a valid Parent
    if (pRC->GetParent() && pRC->GetParent()->GetID()) {
      // Destroy Emitter Objs
      pOM->DestroyObject(pRC->GetParent());
    }

    // Erase from Map
    m_cEmitterRenderComps.erase(m_cEmitterRenderComps.begin());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Effect Mutators
///////////////////////////////////////////////////////////////////////////////

// On/Off (All Emitters)
void effect_data::SwitchOnOffEmitters(bool bOn) {
  // Check for Cooldown
  if (bOn && m_fCooldown > 0.0f) {
    return;
  }

  m_bOn = bOn;

  // Turn on all Emitters
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;

    pPE->SetOn(bOn);

    emitterIter++;
  }
}

// On Target (All Emitters)
void effect_data::SetOnTarget(bool bOnTarget) {
  // Attach all Emitters to their Parent Frame
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;

    pPE->SetIsOnTarget(bOnTarget);

    emitterIter++;
  }
}

// Continuous (All Emitters)
void effect_data::SetContinuous(bool bContinuous) {
  // Tell all emitter to change continuosity
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;

    pPE->SetContinuous(bContinuous);

    emitterIter++;
  }
}

// Parent Frame (All Emitters)
void effect_data::SetParentFrame(CFrame* pFrame) {
  // Tell all emitter to change parent frame
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;

    pPE->SetParentFrame(pFrame);

    emitterIter++;
  }
}

// Context (Passed in Emitter)
void effect_data::ChangeContext(
    EParticleEmitterType eEmitterType,
    unsigned int uRenderContextIdx) {
  // Get Context
  DXRenderContext* pContext = DXRenderContextManager::GetInstance()->GetContext(
      (ERenderContext)uRenderContextIdx);

  // Tell all emitter to change continuosity
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;
    CRenderComponent* pRC = (*emitterIter).second;

    if (pPE->GetType() == eEmitterType) {
      pRC->GetRenderShape()->SetRenderContext(pContext);
    }

    emitterIter++;
  }
}

// Animation Frame (Passed in Emitter)
void effect_data::SetFrame(EParticleEmitterType eEmitterType, int nFrame) {
  // Tell all emitter to change frames
  EmitterRenderCompMap::iterator emitterIter;
  emitterIter = m_cEmitterRenderComps.begin();
  while (emitterIter != m_cEmitterRenderComps.end()) {
    // Get a Pointer to the Emitter
    CParticleEmitter* pPE = (*emitterIter).first;

    if (pPE->GetType() == eEmitterType) {
      pPE->SetFrame(nFrame);
    }

    emitterIter++;
  }
}

namespace component {
effect::effect(CObject* pParent)
    : m_pcParent(pParent) {
  // Register for Events
  CEventManager* pEM = CEventManager::GetInstance();

  // Update Gameplay
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, UpdateCallback);

  // Update Win State
  szEventName = "Update";
  szEventName += WIN_STATE;
  pEM->RegisterEvent(szEventName, this, WinStateCallback);

  // Destroy Obj
  pEM->RegisterEvent("DestroyObject", this, DestroyObjectCallback);
}

// Win State Update
void effect::WinState(void) {
  // Tell all emitter to turn off except victory ones
  EffectMap::iterator effectIter = m_cEffectMap.begin();
  while (effectIter != m_cEffectMap.end()) {
    EEmitterCompType eType = (*effectIter).first;
    effect_data* pEffect = &(*effectIter).second;

    if (eType != EC_TYPE_CONFETTI && eType != EC_TYPE_FIREWORK_TRAIL
        && eType != EC_TYPE_FIREWORK_BURST) {
      pEffect->SwitchOnOffEmitters(false);
    }

    effectIter++;
  }
}

// Shutdown Component
void effect::Shutdown(void) {
  // Shutdown Each Effect
  EffectMap::iterator effectIter;
  while (!m_cEffectMap.empty()) {
    effectIter = m_cEffectMap.begin();
    effect_data* pEffect = &(effectIter->second);
    pEffect->Shutdown();

    m_cEffectMap.erase(effectIter);
  }

  // Unregister for all Events
  CEventManager::GetInstance()->UnregisterEventAll(this);
}

effect::~effect(void) { m_cEffectMap.clear(); }

// Add Emitter
void effect::AddEmitter(
    EEmitterCompType eType,
    CParticleEmitter* pEmitter,
    CRenderComponent* pRenderComp,
    CFrame* pParentFrame,
    EParticleEmitterType eEmitterType) {
  // Parent Frame and Type
  pEmitter->SetParentFrame(pParentFrame);
  pEmitter->SetType(eEmitterType);

  // Look for existing Effect
  EffectMap::iterator effectIter = m_cEffectMap.find(eType);
  if (effectIter != m_cEffectMap.end()) {
    (*effectIter)
        .second.m_cEmitterRenderComps.insert(make_pair(pEmitter, pRenderComp));
  } else {
    m_cEffectMap[eType].m_cEmitterRenderComps.insert(
        make_pair(pEmitter, pRenderComp));
  }
  m_cEffectMap[eType].m_nEmitterCount++;
}

// Update Component
void effect::UpdateCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Listener and Data from Events
  effect* pEC = (effect*)pComp;
  TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
  float fDT = pUpdateEvent->m_fDeltaTime;

  // Call Handler
  pEC->Update(fDT);
}

// Win State Update Component
void effect::WinStateCallback(IEvent* /*pEvent*/, IComponent* pComp) {
  effect* pEC = (effect*)pComp;
  pEC->WinState();
}

// Destroy Obj
void effect::DestroyObjectCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Listener and Data from Event
  effect* pEC = (effect*)pComp;
  TObjectEvent* pData = (TObjectEvent*)pEvent->GetData();

  // If its our Parent who is being Deleted
  if (pEC->GetParent()->GetID() == pData->m_pcObj->GetID()) {
    // Shutdown Component Handler
    pEC->Shutdown();
  }
}

// Update Component
void effect::Update(float fDT) {
  EffectMap::iterator effectIter = m_cEffectMap.begin();
  while (effectIter != m_cEffectMap.end()) {
    effect_data* pEffect = &(*effectIter).second;
    pEffect->Update(fDT);

    effectIter++;
  }
}

// Cooldown (Passed in Effect)
void effect::SetCooldown(EEmitterCompType eType, float fCooldown) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].m_fCooldown = fCooldown;
}

// Lifespan (Passed in Effect)
void effect::SetLifespan(EEmitterCompType eType, float fLifespan) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end()) {
    m_cEffectMap[eType].SetContinuous(true);
    m_cEffectMap[eType].m_fLifespan = fLifespan;
  }
}

// Coninuous (Passed in Effect)
void effect::SetContinuous(EEmitterCompType eType, bool bContinuous) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].SetContinuous(bContinuous);
}

// Parent Frame (Passed in Effect)
void effect::SetParentFrame(EEmitterCompType eType, CFrame* pFrame) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].SetParentFrame(pFrame);
}

// On/Off (Passed in Effect)
void effect::SwitchOnOffEmitters(EEmitterCompType eType, bool bOn) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].SwitchOnOffEmitters(bOn);
}

// On Target (Passed in Effect)
void effect::SetOnTarget(EEmitterCompType eType, bool bOnTarget) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].SetOnTarget(bOnTarget);
}

// Effect Timer (Passed in Effect)
void effect::Seeffect_dataTimer(EEmitterCompType eType, float fTimer) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].SetETimer(fTimer);
}

// Dead Timer (Passed in Effect)
void effect::SetDeadTimer(EEmitterCompType eType, float fTimer) {
  if (m_cEffectMap.find(eType) != m_cEffectMap.end())
    m_cEffectMap[eType].SetDeadTimer(fTimer);
}

// Context (Passed in Effect, Passed in Emitter)
void effect::ChangeContext(
    EEmitterCompType eCompType,
    EParticleEmitterType eEmitterType,
    unsigned int uRenderContextIdx) {
  if (m_cEffectMap.find(eCompType) != m_cEffectMap.end())
    m_cEffectMap[eCompType].ChangeContext(eEmitterType, uRenderContextIdx);
}

// Animation Frame (Passed in Effect, Passed in Emitter)
void effect::SetFrame(
    EEmitterCompType eCompType,
    EParticleEmitterType eEmitterType,
    int nFrame) {
  if (m_cEffectMap.find(eCompType) != m_cEffectMap.end())
    m_cEffectMap[eCompType].SetFrame(eEmitterType, nFrame);
}
} // namespace component
} // namespace scd
