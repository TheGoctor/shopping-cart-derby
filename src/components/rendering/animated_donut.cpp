////////////////////////////////////////////////////////////////////////////////
//	File			:	animated_donut.cpp
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Animated Donut
////////////////////////////////////////////////////////////////////////////////
#include "animated_donut.h"

#include "components/animation_manager.h"
#include "core/object_manager.h"
#include "rendering/effect_manager.h"

///////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////
#define DONUT_PUFF_LIFESPAN (0.1f)

namespace scd {
// Initalize
void animated_donut::Init(void) {
  // Register for Events
  CEventManager* pEM = CEventManager::GetInstance();

  // Donut Effect
  pEM->RegisterEvent("DonutEffect", this, UseDonutCallback);
  pEM->RegisterEvent("DonutDespawn", this, DespawnDonutCallback);

  // Shutdown Gameplay
  string szEventName = "ShutdownObjects";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, ShutdownGameplayCallback);

  // Enter Win State
  szEventName = "InitObjects";
  szEventName += WIN_STATE;
  pEM->RegisterEvent(szEventName, (IComponent*)this, WinInitCallback);

  // Update Gameplay
  szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  pEM->RegisterEvent(szEventName, this, UpdateCallback);

  // Transform the Donut to the Correct Values
  m_pParentObj->GetTransform()->ScaleFrame(1.25f, 1.25f, 1.25f);
  m_pParentObj->GetTransform()->TranslateFrame(D3DXVECTOR3(0.0f, 0.0f, -0.75f));

  // Get Animate Component
  m_pAnimComp =
      CAnimateManager::GetInstance()->GetComponent(m_pParentObj->GetID());
  m_pAnimComp->SetOn(false);
  m_pAnimComp->SetIsActive(false);

  // Create Puff Effect
  m_pPuffEffect =
      CEffectManager::GetInstance()->CreateDonutUseComponent(m_pParentObj);
}

///////////////////////////////////////////////////////////////////////////////
// Factory Funcs
///////////////////////////////////////////////////////////////////////////////

// LUA Creation
int animated_donut::CreateAnimatedDonutComponent(lua_State* ptLUA) {
  // Get Data from Lua
  CObject* pcObj = (CObject*)lua_topointer(ptLUA, -1);

  // Pop that Solda Boi!
  lua_pop(ptLUA, 1);

  // Create the Component
  CreateAnimatedDonutComponent(pcObj);

  // Do not Return anything to LUA
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Helper Funcs
///////////////////////////////////////////////////////////////////////////////

// Check for Player Match
bool animated_donut::CheckForPlayerMatch(CObject* pOtherObj) {
  // Get Object Names
  char* szEventObjName = (char*)pOtherObj->GetID();
  char* szCompObjName = (char*)m_pParentObj->GetID();

  // Check for a Name Match
  bool bMatch = true;
  for (int i = 0; i < 7; ++i) {
    if (szEventObjName[i] != szCompObjName[i]) {
      bMatch = false;
    }
  }

  return bMatch;
}

///////////////////////////////////////////////////////////////////////////////
// Event Callbacks
///////////////////////////////////////////////////////////////////////////////

// Update
void animated_donut::UpdateCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Component and Data from Event
  animated_donut* pDonutComp = (animated_donut*)pComp;
  TUpdateStateEvent* pUpdateEvent = (TUpdateStateEvent*)pEvent->GetData();
  float fDT = pUpdateEvent->m_fDeltaTime;

  // Update Component
  pDonutComp->Update(fDT);
}

// Use Effect
void animated_donut::UseDonutCallback(IEvent* pEvent, IComponent* pComp) {
  // Get Listener and Data from Event
  animated_donut* pADC = (animated_donut*)pComp;
  TStatusEffectEvent* pData = (TStatusEffectEvent*)pEvent->GetData();

  // Call Handler
  pADC->UseDonut(pData);
}

// Despawn Donut
void animated_donut::DespawnDonutCallback(
    IEvent* pEvent,
    IComponent* pComp) {
  animated_donut* pADC = (animated_donut*)pComp;
  TStatusEffectEvent* pData = (TStatusEffectEvent*)pEvent->GetData();

  pADC->DespawnDonut(pData);
}

// Win State Init
void animated_donut::WinInitCallback(IEvent*, IComponent* pComp) {
  // Get Listener from Event
  animated_donut* pDC = (animated_donut*)pComp;

  // Turn Off Animations
  pDC->m_pAnimComp->SetOn(false);
  pDC->m_pAnimComp->SetIsActive(false);
}

// Shutdown Gameplay
void animated_donut::ShutdownGameplayCallback(IEvent*, IComponent* pComp) {
  // Unregister the Listener for All Events
  CEventManager::GetInstance()->UnregisterEventAll(pComp);
}

///////////////////////////////////////////////////////////////////////////////
// Event Handlers
///////////////////////////////////////////////////////////////////////////////

// Update
void animated_donut::Update(float fDT) {
  // Check for Effect
  if (m_fDuration > 0.0f) {
    // Decrease Duration
    m_fDuration -= fDT;

    // Check for End of Effect
    if (m_fDuration <= 0.0f) {
      // Turn Effect and Animation Off
      m_pAnimComp->SetOn(false);
      m_pAnimComp->SetIsActive(false);
      m_pPuffEffect->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
      m_pPuffEffect->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
      m_fDuration = 0.0f;
    }
  }
}

// Use Donut
void animated_donut::UseDonut(TStatusEffectEvent* ptSEEvent) {
  // Check if our Player Used the Donut
  if (CheckForPlayerMatch(ptSEEvent->m_pObject)) {
    // Turn Animation Off
    m_pAnimComp->SetOn(true);
    m_pAnimComp->SetIsActive(true);

    // Check if the Effect should be turned On
    if (m_fDuration <= 0.0f) {
      m_pPuffEffect->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
      m_pPuffEffect->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
    }

    // Set Duration
    m_fDuration = ptSEEvent->m_fDuration;
  }
}

// Despawn Donut
void animated_donut::DespawnDonut(TStatusEffectEvent* ptSEEvent) {
  // Check if our Player's Donut has Despawned
  if (CheckForPlayerMatch(ptSEEvent->m_pObject)) {
    // Turn Off Animation and Effects
    m_pAnimComp->SetOn(false);
    m_pAnimComp->SetIsActive(false);
    m_pPuffEffect->SwitchOnOffEmitters(EC_TYPE_DONUT_USE, true);
    m_pPuffEffect->SetLifespan(EC_TYPE_DONUT_USE, DONUT_PUFF_LIFESPAN);
    m_fDuration = 0.0f;
  }
}
} // namespace scd
