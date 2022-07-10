#include "energy_drink.h"

#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"

#include <functional>

namespace scd::component {

energy_drink::energy_drink(
    scd::object& owner,
    scd::event_manager& event_manager)
    : base_component(owner) {
  event_manager.register_event("UseBoost", std::bind(&on_use, this));
}

std::shared_ptr<energy_drink>
energy_drink::create(scd::object& owner, scd::event_manager& event_manager) {
  return owner.create_component<energy_drink>(event_manager);
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
  scd::event::status_effect::send("Boost", this, obj, _duration);
  is_spawned(true);
}

} // namespace scd::component
