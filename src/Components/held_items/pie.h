#pragma once

#include "core/base_component.h"
#include "core/object.h"

namespace scd {
class event;

#define BLIND_TIME (3.0f)

class pie : public scd::base_component {
public:
  pie(scd::object& owner)
    : base_component(owner) {}

  static pie* create(scd::object& owner, const scd::vector3& direction);

  void first_init();

  static void on_update(event* cEvent, scd::base_component* cCenter);
  static void on_player_collision(event* cEvent, scd::base_component* cCenter);
  static void on_environment_collision(event* cEvent,
                                       scd::base_component* cCenter);
  static void on_item_collision(event* cEvent, scd::base_component* cCenter);
  static void on_pause_update(event*, scd::base_component* pComp);

  void reinit();
  void despawn();

  bool is_spawned() const { return _is_spawned; }
  const scd::vector3& direction() const { return _direction; }
  float duration() const { return _duration; }
  float time_remaining() const { return _time_remaining; }

  void set_spawned(bool bIsSpawned) { _is_spawned = bIsSpawned; }
  void set_position(const scd::vector3& position) {
    _owner.set_local_position({position.x, 0.5f, position.z});
  }
  void set_direction(const scd::vector3& direction) { _direction = direction; }
  void set_duration(float duration) { _duration = duration; }
  void set_time_remaining(float seconds) { _time_remaining = seconds; }

private:
  bool _is_spawned;
  scd::vector3 _direction;
  float _duration;
  float _time_remaining;
  float _speed;
  int _id;
};

} // namespace scd
