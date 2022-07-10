#include "energy_drink.h"

#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"

namespace scd::component {

energy_drink::energy_drink(scd::object& owner)
    : base_component(owner) {
  event_manager::get()->register_event("UseBoost", this, on_use);
}

std::shared_ptr<energy_drink> energy_drink::create(scd::object& owner) {
  return owner.create_component<energy_drink>();
}

void energy_drink::on_update(float dt) {
  // if we're not spawned, dont do anything
  if (!is_spawned()) {
    return;
  }

  // decrement time
  _time_remaining -= dt;

  if (time_remaining() <= 0.0f) {
    despawn();
    return;
  }
}

void energy_drink::on_use(scd::object& obj) {
  SendStatusEffectEvent("Boost", this, obj, _duration);
  is_spawned(true);
}

} // namespace scd::component
