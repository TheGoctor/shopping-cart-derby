#include "pie.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"

#include <functional>

namespace scd::component {
pie::pie(scd::object& owner, scd::event_manager& event_manager)
    : base_component(owner) {
  _audio_id = CWwiseSoundManager::GetInstance()->RegisterHeldObject();

  event_manager.register_event("UpdateGameplay", std::bind(&on_update, this));
  event_manager.register_event(
      "HeldItemInWorldCollision", std::bind(&on_environment_collision, this));
  event_manager.register_event(
      "HeldItemInWorldPlayerCollision", std::bind(&on_player_collision, this));
  event_manager.register_event(
      "PieDestroyedByItem", std::bind(&on_item_collision, this));

  event_manager.register_event("UpdatePause", std::bind(&on_pause_update, this));
  event_manager.register_event(
      "UpdateConsole", std::bind(&on_pause_update, this));

  event_manager.register_event(
      "UpdatePauseOptions", std::bind(&on_pause_update, this));
  event_manager.register_event(
      "UpdatePauseKeybinds", std::bind(&on_pause_update, this));
  event_manager.register_event(
      "UpdateQuitConfirmation", std::bind(&on_pause_update, this));

  scd::event::object_event::send(
      "PieCreated", this, _owner, scd::event_priority::immediate);
}

std::shared_ptr<scd::component::pie> pie::create(
    scd::object& owner,
    scd::event_manager& event_manager,
    const scd::vector3& direction) {
  std::shared_ptr<pie> component = owner.create_component<pie>(event_manager);

  component->_direction = direction.getNormalized();

  return component;
}

void pie::on_update(float dt) {
  if (!is_spawned()) {
    return;
  }

  set_position(_owner.world_position());

  _time_remaining -= dt;

  if (_time_remaining <= 0.0f) {
    despawn();
    return;
  }

  _owner.translate(_direction * dt * _speed);
  _owner.rotate({0.0f, 1.0f, 0.0f}, dt * 7.0f);

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      _audio_id, _owner.world_position(), 0.30f);

  scd::event::render::send(
      "AddToSet", this, _owner, scd::event_priority::immediate);
}

void pie::on_player_collision(const scd::event::impact& data) {
  // for player collision, the collider is the player
  if (_owner == data._collidee && is_spawned()) {
    scd::event::status_effect::send(
        "Blind", this, data._collider, blind_effect_duration);
    despawn();
  }
}

void pie::on_environment_collision(const scd::event::impact& data) {
  // hit a wall, it's a pie, it does not get to go on!
  // pComp->Despawn();

  // parent == collider was causing it to always think it collides
  // for environment collisions, the collider is the item
  if (_owner == data._collider && is_spawned()) {
    scd::vector3 MoveDir, ColNormal, ReflectDir;

    MoveDir = _owner.local_position().getNormalized();
    ColNormal = data._normal.getNormalized();

    // play sound
    if (ColNormal.x > 0.0f || ColNormal.x <= 0.0f && ColNormal.z == 0.0f) {
      _direction.x *= -1.0f;
    } else if (
        ColNormal.z > 0.0f || ColNormal.z < 0.0f && ColNormal.x == 0.0f) {
      _direction.z *= -1.0f;
    } else {
      _direction.x *= -1.0f;
      _direction.z *= -1.0f;
    }

    // pComp->Despawn(); // dont despawn anymore
    CWwiseSoundManager::GetInstance()->PlayTheSound(
        ITEM_CREAMPIE_USE, _audio_id);
  }
}
void pie::reinit() {
  _time_remaining = _duration;
  _is_spawned = true;
  _audio_id = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      _audio_id, _owner.world_position(), 0.3f);
  CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CREAMPIE_USE, _audio_id);

  scd::event::object_event::send(
      "PieEffect", this, _owner, scd::event_priority::immediate);
}
void pie::despawn() {
  _is_spawned = false;
  _owner.local_position({300, 300, 300});
  CWwiseSoundManager::GetInstance()->UnregisterObject(_audio_id);

  scd::event::object_event::send("PieDespawned", this, _owner);
}

void pie::on_pause_update() {
  if (_is_spawned) {
    scd::event::render::send(
        "AddToSet", this, _owner, scd::event_priority::update);
  }
}

void pie::on_item_collision(const scd::event::impact& data) {
  if (_owner == data._collider && is_spawned()) {
    // effect stuff
    scd::vector3 p1, p2, trans;
    p2 = _owner.world_position();

    p1 = CCollisionManager::GetInstance()
             ->GetPieDestroyObject()
             ->GetTransform()
             ->GetWorldPosition();

    trans = p2 - p1; // vector to the turkeys position

    CCollisionManager::GetInstance()
        ->GetPieDestroyObject()
        ->GetTransform()
        ->TranslateFrame(trans);

    scd::component::effect* pEC =
        CEffectManager::GetInstance()->CreatePieHitComponent(
            CCollisionManager::GetInstance()->GetPieDestroyObject());

    pEC->SetDeadTimer(EC_TYPE_PIE_HIT, 0.0f);
    pEC->SwitchOnOffEmitters(EC_TYPE_PIE_HIT, true);

    despawn();
  }
}

} // namespace scd::component
