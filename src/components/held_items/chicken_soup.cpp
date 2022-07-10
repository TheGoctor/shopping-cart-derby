#include "chicken_soup.h"

#include "audio/wwise/wwise_audio_manager.h"
#include "components/held_item_manager.h"
#include "core/object_manager.h"
#include "events/event_manager.h"
#include "events/events.h"

#include <functional>

namespace scd::component {
chicken_soup::chicken_soup(scd::object& owner, scd::event_manager& event_manager)
    : scd::base_component(owner) {
  _audio_id = CWwiseSoundManager::GetInstance()->RegisterHeldObject();

  event_manager.register_event("UpdateGameplay", std::bind(&on_update, this));
}

std::shared_ptr<chicken_soup> chicken_soup::create(scd::object& owner, scd::event_manager& event_manager) {
  return owner.create_component<chicken_soup>(event_manager);
}

void chicken_soup::reinit() {
  is_spawned(true);
  time_remaining(duration());

  _audio_id = CWwiseSoundManager::GetInstance()->RegisterHeldObject();

  // tell movement that it's invulnerable (since it has invulnerable checks in
  // all its status effects)
  scd::event::status_effect::send(
      "Invulnerable", this, *_attached_object, _duration);

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      _audio_id, _attached_object->world_position(), 0.3f);

  CWwiseSoundManager::GetInstance()->PlayTheSound(
      ITEM_CHICKENSOUP_USE, _audio_id);
}

void chicken_soup::on_update(float dt) {
  if (!is_spawned()) {
    // not being used/spawned
    return;
  }

  _time_remaining -= dt;

  if (time_remaining() <= 0.0f) {
    despawn();
    return;
  }

  set_position(_attached_object->world_position());

  CWwiseSoundManager::GetInstance()->SetObjectPosition(
      _audio_id, _owner.world_position(), 0.30f);

  // SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);
}

void chicken_soup::despawn() {
  scd::event::object_event::send("InvulnerableEnd", this, *_attached_object);

  _time_remaining = 0.0f;
  _is_spawned = false;

  CWwiseSoundManager::GetInstance()->PlayTheSound(CHICKENSOUP_STOP, _audio_id);

  CWwiseSoundManager::GetInstance()->UnregisterObject(_audio_id);

  _owner.local_position({300, 300, 300});
}

} // namespace scd::component
