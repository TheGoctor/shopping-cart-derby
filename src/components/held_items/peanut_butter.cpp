#include "peanut_butter.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

namespace scd::component {
peanut_butter::peanut_butter(scd::object& owner)
    : base_component(owner) {
  _time_remaining = _duration;
  PEANUT_BUTTER_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  // register for events
  event_manager.register_event(
      "HeldItemInWorldPlayerCollision", this, PlayerCollision);
  event_manager.register_event(
      "HeldItemInWorldCollision", this, ItemCollision);
  string szEventName = "Update";
  szEventName += GAMEPLAY_STATE;
  event_manager.register_event(szEventName, this, Update);

  szEventName = "Update";
  szEventName += PAUSE_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += CONSOLE_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_OPTIONS_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += PAUSE_KEYBINDS_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);

  szEventName = "Update";
  szEventName += QUIT_CONFIRMATION_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += IN_GAME_HOW_TO_PLAY_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);
  szEventName = "Update";
  szEventName += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
  event_manager.register_event(
      szEventName, this, PauseUpdateCallback);
}

std::shared_ptr<peanut_butter> peanut_butter::create(scd::object& owner) {
  return owner.create_component<peanut_butter>();
}

void peanut_butter::on_use() {
  PEANUT_BUTTER_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      PEANUT_BUTTER_ID, _owner.world_position(), 0.3f);

  CWwiseSoundManager::GetInstance()->PlayTheSound(
      ITEM_PEANUTBUTTER_USE, PEANUT_BUTTER_ID);

  is_spawned(true);
  time_remaining(duration());
}
// call backs
void peanut_butter::on_update(float dt) {
  // if were not spawned, dont do antyhing
  if (!is_spawned()) {
    return;
  }

  _time_remaining -= dt;
  _sound_cooldown -= dt; // update our cooldown for sound playing

  SendRenderEvent("AddToSet", this, _owner, PRIORITY_IMMEDIATE);

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      PEANUT_BUTTER_ID, _owner.world_position(), 0.25f);

  if (time_remaining() <= 0.0f) {
    despawn();
  }
}
void peanut_butter::on_player_collision(
    IEvent* cEvent,
    scd::base_component* cCenter) {
  TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

  // CWwiseSoundManager::GetInstance()->PlayTheSound(soundIDENUM, ObjID)

  // if we're the ones that collided and we're spawned
  if (owner() == tEvent->m_pCollidedWith && is_spawned()) {
    SendStatusEffectEvent(
        "Slow", this, tEvent->m_pcCollider, _effect_duration);

    if (_sound_cooldown <= 0.0f) {
      CWwiseSoundManager::GetInstance()->PlayTheSound(
          STATUS_SLOWED, PEANUT_BUTTER_ID);
      _sound_cooldown = _effect_duration; // reset once it plays
    }
  }

  // dont despawn until our timer's out NEVERMIND LOL NEVERMIND THAT NEVERMIND
  // LOL
  // pComp->Despawn();
}

void peanut_butter::despawn() {
  CWwiseSoundManager::GetInstance()->UnregisterObject(PEANUT_BUTTER_ID);
  _time_remaining = 0.0f;
  _is_spawned = false;
  _owner.local_position({300, 300, 300});
}

void peanut_butter::on_pause_update(float dt) {
  if (is_spawned()) {
    SendRenderEvent("AddToSet", this, _owner, PRIORITY_UPDATE);
  }
}

} // namespace scd::component
