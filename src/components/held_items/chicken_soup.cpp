#include "chicken_soup.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

namespace scd::component {
chicken_soup* chicken_soup::create(scd::object* pObj) {
  chicken_soup* pComp = MMNEW(chicken_soup(pObj));
  pComp->FirstInit();
  pComp->CHICKENSOUP_ID = -1;
  pObj->AddComponent(pComp);

  return pComp;
}
void chicken_soup::FirstInit(void) {
  m_fDuration = 10.0f;
  m_fTimeRemaining = m_fDuration;
  CHICKENSOUP_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  //(event)registration forms!
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
}
void chicken_soup::ReInit() {
  SetIsSpawned(true);
  SetTimeRemaining(GetDuration());
  CHICKENSOUP_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  // tell movement that it's invulnerable (since it has invulnerable checks in
  // all its status effects)
  SendStatusEffectEvent("Invulnerable", this, m_pAttachedObject, m_fDuration);
  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      CHICKENSOUP_ID,
      m_pAttachedObject->GetTransform()->GetWorldPosition(),
      0.3f);
  CWwiseSoundManager::GetInstance()->PlayTheSound(
      ITEM_CHICKENSOUP_USE, CHICKENSOUP_ID);
}
// call backs
void chicken_soup::Update(IEvent* cEvent, scd::base_component* cCenter) {
  chicken_soup* pComp = (chicken_soup*)cCenter;
  TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
  float fDt = eEvent->m_fDeltaTime;

  if (!pComp->GetIsSpawned()) {
    // not being used/spawned
    return;
  }

  pComp->SetTimeRemaining(pComp->GetTimeRemaining() - fDt);
  if (pComp->GetTimeRemaining() <= 0.0f) {
    pComp->Despawn();
    return;
  }
  pComp->SetPosition(
      pComp->m_pAttachedObject->GetTransform()->GetWorldPosition());
  pComp->m_pParent->GetTransform()->GetLocalMatrix()._42 = 0.5f;
  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      pComp->CHICKENSOUP_ID,
      pComp->GetParent()->GetTransform()->GetWorldPosition(),
      0.30f);

  // SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);
}
void chicken_soup::PlayerCollision(
    IEvent* /*cEvent*/,
    scd::base_component* /*cCenter*/) {
  // chicken_soup* pComp = (chicken_soup*) cCenter;
}
void chicken_soup::ItemCollision(
    IEvent* /*cEvent*/,
    scd::base_component* /*cCenter*/) {}
void chicken_soup::Despawn() {
  SendObjectEvent("InvulnerableEnd", this, m_pAttachedObject);

  m_fTimeRemaining = 0.0f;
  m_bIsSpawned = false;
  CWwiseSoundManager::GetInstance()->PlayTheSound(
      CHICKENSOUP_STOP, CHICKENSOUP_ID);
  CWwiseSoundManager::GetInstance()->UnregisterObject(CHICKENSOUP_ID);
  m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
  m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
  m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;
}
} // namespace scd::component
