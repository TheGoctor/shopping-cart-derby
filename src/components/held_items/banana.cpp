#include "banana.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "physics/physics_manager.h"
#include "rendering/effect_manager.h"

namespace scd::component {
banana::banana(scd::object& owner, scd::event_manager& event_manager)
    : scd::base_component(owner) {
  event_manager.register_event("UpdateGameplay", std::bind(&on_update, this));
  event_manager.register_event(
      "HeldItemInWorldPlayerCollision", std::bind(&on_player_collision, this));
  event_manager.register_event(
      "BananaDestroyedByItem", std::bind(&on_item_collision, this));
}

void banana::spawn() {
  is_spawned(true);
  _audio_id = m_pSound->RegisterHeldObject();
  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CREAMPIE_USE, _audio_id);
}

void banana::on_update(float dt) {
  if (!is_spawned()) {
    return;
  }

  m_pSound->SetObjectPosition(_audio_id, _owner.world_position(), 0.25f);

  scd::event::render::send(
      "AddToSet", this, _owner, scd::event_priority::immediate);
}

void banana::despawn() {
  is_spawned(false);

  _owner.translate({300, 300, 300});

  m_pSound->UnregisterObject(_audio_id);
}

void banana::on_player_collision(const scd::event::impact& data) {
  if (_owner == data._collidee && is_spawned()) {
    scd::event::status_effect::send("Slip", this, data._collider, 2.3f);
    despawn();
  }
}

void banana::on_item_collision(const scd::event::impact& data) {
  if (_owner == data._collider && is_spawned()) {
    // effect stuff
    scd::vector3 p2 = _owner.world_position();
    scd::vector3 p1 =
        physics_manager::get().GetBananaDestroyObject()->world_position();
    scd::vector3 trans = p2 - p1; // vector to the turkeys position
    physics_manager::get().GetBananaDestroyObject()->translate(trans);

    CEffectComponent* pEC = effect_manager::get().CreateSlipComponent(
        physics_manager::get().GetBananaDestroyObject());

    pEC->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
    pEC->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);

    despawn();
  }
}

} // namespace scd::component
