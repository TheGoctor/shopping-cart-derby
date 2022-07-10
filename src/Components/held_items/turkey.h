#pragma once

#include "core/base_component.h"
#include "core/object.h"

namespace scd::component {

class turkey : public scd::component {
public:
  turkey(scd::object* owner);

  static turkey* create(scd::object* owner, const scd::vector3& move_direction);

  void first_init();

  void reinit();

  void set_position(const scd::vector3& position) {
    _owner->GetTransform()->GetLocalMatrix()._41 = vPos.x;
    _owner->GetTransform()->GetLocalMatrix()._42 = vPos.y;
    _owner->GetTransform()->GetLocalMatrix()._43 = vPos.z;
  }

  void set_direction(const scd::vector3& direction) {
    _move_direction = direction;
  }

  float stun_duration() const { return _stun_duration; }

  bool spawned() const { return _spawned; }

  void set_collider(CCollideable* collider) { _collider = collider; }

  void despawn();

  static void on_update(IEvent* cEvent, scd::base_component* cCenter);
  static void on_player_collision(IEvent* cEvent, scd::base_component* cCenter);
  static void
  on_environment_collision(IEvent* cEvent, scd::base_component* cCenter);
  static void on_item_collision(IEvent* cEvent, scd::base_component* cCenter);
  static void on_pause_update_callback(IEvent*, scd::base_component* pComp);

private:
  bool _spawned;

  scd::vector3 _move_direction;

  float _duration;
  float _time_left;
  float _stun_duration;
  int _id;

  float _speed;
  scd::components::collider* _collider;
};

} // namespace scd::component
