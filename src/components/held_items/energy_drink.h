#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

namespace scd::component {

class energy_drink : public scd::base_component {
private:
  bool _is_spawned{true};
  float _duration{3.0f};
  float _time_remaining;
  float _boost_amount{2.0f};

  int ENERGY_DRINK_ID;

public:
  energy_drink(scd::object& owner);

  static std::shared_ptr<energy_drink> create(scd::object& owner);

  void on_update(float dt);
  void on_use(scd::object& obj);

  void despawn() { _is_spawned = false; }

  bool is_spawned() const { return _is_spawned; }
  float duration() const { return _duration; }
  float time_remaining() const { return _time_remaining; }
  float boost_amount() const { return _boost_amount; }

  void is_spawned(bool value) { _is_spawned = value; }
  void duration(float value) { _duration = value; }
  void time_remaining(float value) { _time_remaining = value; }
  void boost_amount(float value) { _boost_amount = value; }
};

} // namespace scd::component
