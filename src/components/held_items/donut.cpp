#include "donut.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

namespace scd::component {

donut* donut::create(scd::object* pObj) {
  donut* pComp = MMNEW(donut(pObj));
  pComp->DONUT_ID = -1;
  pComp->FirstInit();
  pObj->AddComponent(pComp);
  return pComp;
}
void donut::FirstInit(void) {
  m_fDuration = DONUTTIME;
  m_fTimeRemaining = m_fDuration;
  DONUT_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
  CEventManager::GetInstance()->RegisterEvent(
      "CartCollision", this, PlayerCollision);
}
void donut::ReInit() {
  SetIsSpawned(true);
  DONUT_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  SetTimeRemaining(GetDuration());

  // m_pAttachedObject is already filled out before this reinit is called inside
  // HeldItemMgr
  SendStatusEffectEvent("DonutEffect", this, m_pAttachedObject, m_fDuration);
  // goober 2
  // SendStatusEffectEvent("DonutDespawn", this, m_pAttachedObject, 0.0f);
  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      DONUT_ID, m_pAttachedObject->GetTransform()->GetWorldPosition(), 0.3f);
  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_DOUGHNUT_USE, DONUT_ID);
}

void donut::Despawn() {
  m_fTimeRemaining = 0.0f;
  CWwiseSoundManager::GetInstance()->PlayTheSound(
      ITEM_DOUGHNUT_AURA_STOP, DONUT_ID);

  m_bIsSpawned = false;
  m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
  m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
  m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;

  CWwiseSoundManager::GetInstance()->UnregisterObject(DONUT_ID);
  // goober 1
}

// call backs
void donut::Update(IEvent* cEvent, scd::base_component* cCenter) {
  donut* pComp = (donut*)cCenter;
  TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
  float fDt = eEvent->m_fDeltaTime;

  if (!pComp->GetIsSpawned()) {
    return;
  }

  pComp->SetTimeRemaining(pComp->GetTimeRemaining() - fDt);
  if (pComp->GetTimeRemaining() <= 0.0f) {
    pComp->Despawn();
    return;
  }

  pComp->SetPosition(
      pComp->m_pAttachedObject->GetTransform()->GetWorldPosition());
  pComp->m_pParent->GetTransform()->GetLocalMatrix()._42 = .5f;

  // SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);

  pComp->SetPosition(pComp->GetParent()->GetTransform()->GetWorldPosition());
}
void donut::PlayerCollision(IEvent* cEvent, scd::base_component* cCenter) {
  donut* pComp = (donut*)cCenter;
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

  if (pComp->GetIsSpawned()
      && tEvent->m_pcCollider == pComp->m_pAttachedObject) {
    // send the collider as the one who gets the item
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        ITEM_DOUGHNUT_IMPACT, pComp->DONUT_ID);
    SendImpactEvent(
        "Steal",
        pComp,
        tEvent->m_pcCollider,
        tEvent->m_pCollidedWith,
        scd::vector3(0.0f, 0.0f, 0.0f));
    SendStatusEffectEvent(
        "DonutDespawn", pComp, pComp->m_pAttachedObject, 0.0f);
    pComp->Despawn();
  }
}

} // namespace scd::component
