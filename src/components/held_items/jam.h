#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

#define JAM_TIME 10.0f

namespace scd::component {

class jam : public scd::base_component {
private:
  bool _is_spawned{true};
  float _duration{10.0f};
  float _time_remaining;

  int JAM_ID;

public:
  jam(scd::object& owner);

  static std::shared_ptr<jam> create(scd::object& pObj);

  void on_update(float dt);
  void on_use();

  void despawn();

  bool is_spawned() const { return _is_spawned; }
  float duration() const { return _duration; }
  float time_remaining() const { return _time_remaining; }

  void is_spawned(bool value) { _is_spawned = value; }
  void duration(float value) { _duration = value; }
  void time_remaining(float value) { _time_remaining = value; }
};

} // namespace scd::component
