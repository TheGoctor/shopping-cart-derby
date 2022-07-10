#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

namespace scd::component {

class donut : public scd::base_component {
private:
  float _time_remaining{_duration};
  float _duration{donut_effect_time};
  scd::object* _attached_object{nullptr};
  bool _is_spawned{true};
  int _audio_id{-1};

  static constexpr float donut_effect_time{15.0f};

public:
  donut(scd::object& owner, scd::event_manager& event_manager);

  static std::shared_ptr<donut>
  create(scd::object& owner, scd::event_manager& event_manager);

  void on_update(float dt);
  void on_player_collision(const scd::event::impact& data);
  void reinit();
  void despawn();

  float time_remaining() const { return _time_remaining; }
  float duration() const { return _duration; }
  bool is_spawned() const { return _is_spawned; }

  void time_remaining(float value) { _time_remaining = value; }
  void duration(float value) { _duration = value; }
  void is_spawned(bool value) { _is_spawned = value; }

  void set_position(const scd::vector3& position) {
    _owner.local_position({position.x, 0.5f, position.z});
  }

  void attach_object(scd::object& obj) { _attached_object = &obj; }
};

} // namespace scd::component
