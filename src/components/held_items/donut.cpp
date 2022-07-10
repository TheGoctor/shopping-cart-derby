#include "donut.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

namespace scd::component {

donut::donut(scd::object& owner, scd::event_manager& event_manager)
    : scd::base_component(owner) {
  _audio_id = CWwiseSoundManager::GetInstance()->RegisterHeldObject();

  event_manager.register_event("UpdateGameplay", std::bind(&on_update, this));
  event_manager.register_event(
      "CartCollision", std::bind(&on_player_collision, this));
}

std::shared_ptr<donut> donut::create(scd::object& owner, scd::event_manager& event_manager) {
  return owner.create_component<donut>(event_manager);
}

void donut::reinit() {
  is_spawned(true);
  _audio_id = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  time_remaining(duration());

  // m_pAttachedObject is already filled out before this reinit is called inside
  // HeldItemMgr
  scd::event::status_effect::send(
      "DonutEffect", this, *_attached_object, _duration);

  // goober 2
  // SendStatusEffectEvent("DonutDespawn", this, m_pAttachedObject, 0.0f);
  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      _audio_id, _attached_object->world_position(), 0.3f);

  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_DOUGHNUT_USE, _audio_id);
}

void donut::despawn() {
  _time_remaining = 0.0f;

  CWwiseSoundManager::GetInstance()->PlayTheSound(
      ITEM_DOUGHNUT_AURA_STOP, _audio_id);

  _is_spawned = false;

  _owner.local_position({300, 300, 300});

  CWwiseSoundManager::GetInstance()->UnregisterObject(_audio_id);
  // goober 1
}

// call backs
void donut::on_update(float dt) {
  if (!is_spawned()) {
    return;
  }

  _time_remaining -= dt;

  if (time_remaining() <= 0.0f) {
    despawn();
    return;
  }

  set_position(_attached_object->world_position());

  // SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);

  set_position(_owner.world_position());
}

void donut::on_player_collision(const scd::event::impact& data) {
  if (is_spawned() && data._collider == _attached_object) {
    // send the collider as the one who gets the item
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        ITEM_DOUGHNUT_IMPACT, _audio_id);

    scd::event::impact::send(
        "Steal", this, data._collider, data._collidee, {0.0f, 0.0f, 0.0f});

    scd::event::status_effect::send(
        "DonutDespawn", this, *_attached_object, 0.0f);

    despawn();
  }
}

} // namespace scd::component
