#pragma once

#include "core/base_component.h"
#include "core/object.h"

namespace scd::component {

class pie : public scd::base_component {
public:
  pie(scd::object& owner, scd::event_manager& event_manager);

  static std::shared_ptr<pie> create(
      scd::object& owner,
      scd::event_manager& event_manager,
      const scd::vector3& direction);

  void on_update(float dt);
  void on_player_collision(const scd::event::impact& data);
  void on_environment_collision(const scd::event::impact& data);
  void on_item_collision(const scd::event::impact& data);
  void on_pause_update();

  void reinit();
  void despawn();

  bool is_spawned() const { return _is_spawned; }
  const scd::vector3& direction() const { return _direction; }
  float duration() const { return _duration; }
  float time_remaining() const { return _time_remaining; }

  void is_spawned(bool value) { _is_spawned = value; }
  void set_position(const scd::vector3& position) {
    _owner.local_position({position.x, 0.5f, position.z});
  }
  void direction(const scd::vector3& value) { _direction = value; }
  void duration(float value) { _duration = value; }
  void time_remaining(float seconds) { _time_remaining = seconds; }

private:
  static constexpr float blind_effect_duration{3.0f};

  bool _is_spawned{true};
  scd::vector3 _direction{};
  float _duration{10.0f};
  float _time_remaining{_duration};
  float _speed{28.0f};
  int _audio_id{-1};
};

} // namespace scd::component
