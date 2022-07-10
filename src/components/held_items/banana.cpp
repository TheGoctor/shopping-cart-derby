#include "banana.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "physics/physics_manager.h"
#include "rendering/effect_manager.h"
#include "rendering/hud_manager.h"

scd::banana(scd::object& owner)
    : scd::base_component(owner) {
  std::string szEventName = "Update";
  BANANA_ID = 0;
  szEventName += GAMEPLAY_STATE;
  CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
  CEventManager::GetInstance()->RegisterEvent(
      "HeldItemInWorldPlayerCollision", this, PlayerCollision);
  CEventManager::GetInstance()->RegisterEvent(
      "BananaDestroyedByItem", this, ItemCollision);

  szEventName = "Update";
  szEventName += PAUSE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += CONSOLE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += CONSOLE_STATE;
  CEventManager::GetInstance()->RegisterEvent(
      szEventName, this, PauseUpdateCallback);
}

void scd::banana::spawn() {
  is_spawned(true);
  BANANA_ID = m_pSound->RegisterHeldObject();
  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CREAMPIE_USE, BANANA_ID);
}

void scd::banana::on_update() {
  if (!is_spawned()) {
    return;
  }

  m_pSound->SetObjectPosition(BANANA_ID, owner.world_position(), 0.25f);

  SendRenderEvent("AddToSet", pComp, pComp->m_pObject, PRIORITY_IMMEDIATE);
}

void scd::banana::despawn() {
  is_spawned(false);

  owner.translate({300, 300, 300});

  m_pSound->UnregisterObject(BANANA_ID);
}
void scd::banana::PlayerCollision(IEvent* cEvent) {
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
  if (&owner == tEvent->m_pCollidedWith && is_spawned()) {
    SendStatusEffectEvent("Slip", this, tEvent->m_pcCollider, 2.3f);
    despawn();
  }
}

void scd::banana::ItemCollision(IEvent* cEvent) {
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
  if (&owner == tEvent->m_pcCollider && is_spawned()) {
    // effect stuff
    scd::vector3 p2 = owner.world_position();
    scd::vector3 p1 =
        physics_manager::get().GetBananaDestroyObject()->world_position();
    scd::vector3 trans = p2 - p1; // vector to the turkeys position
    physics_manager::get().->GetBananaDestroyObject()->translate(trans);

    CEffectComponent* pEC = effect_manager::get().CreateSlipComponent(
        physics_manager::get().GetBananaDestroyObject());
    pEC->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
    pEC->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);

    pComp->Despawn();
  }
}
