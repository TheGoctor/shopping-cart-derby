#pragma once

#include "core/base_component.h"
#include "core/object.h"

namespace scd::component {

class turkey : public scd::base_component {
public:
  turkey(scd::object& owner);

  static std::shared_ptr<turkey> create(scd::object& owner, const scd::vector3& move_direction);

  void reinit();

  void set_position(const scd::vector3& position) {
    _owner.local_position(position);
  }

  void set_direction(const scd::vector3& direction) {
    _move_direction = direction;
  }

  float stun_duration() const { return _stun_duration; }

  bool spawned() const { return _spawned; }

  void set_collider(scd::component::collider* collider) { _collider = collider; }

  void despawn();

  void on_update(float dt);
  void on_player_collision(IEvent* cEvent);
  void on_environment_collision(IEvent* cEvent);
  void on_item_collision(IEvent* cEvent);
  void on_pause_update(float dt);

private:
  bool _spawned;

  scd::vector3 _move_direction;

  float _duration{30.0f};
  float _time_left{_duration};
  float _stun_duration{2.0f};
  int _id;

  float _speed{28.0f};
  scd::component::collider* _collider;
};

} // namespace scd::component
