#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/math_types.h"
#include "physics/physics_manager.h"

#define SLOW_TIME (2.3f)

namespace scd {
class collider;
class event;
class renderable;

class banana : public scd::base_component {
public:
  banana(scd::object& owner)
    : base_component(owner) {}

  static banana* create(scd::object& owner);

  void first_init();
  void reinit();

  static void update(IEvent* cEvent, scd::base_component* cCenter);
  static void on_player_collision(event* cEvent, scd::base_component* cCenter);
  static void on_item_collision(event* cEvent, scd::base_component* cCenter);
  void despawn();

  bool is_spawned() const { return _is_spawned; }
  const scd::vector3& position() const { return _position; }
  const scd::vector3& direction() const { return _direction; }
  collider* area_of_effect() const { return _area_of_effect; }

  // mutators
  void set_spawned(bool bSpawned) { _is_spawned = bSpawned; }
  void set_position(const scd::vector3& position) {
    _owner.set_local_position(position);
    _position = position;
  }
  void set_direction(const scd::vector3& direction) { _direction = direction; }
  void set_area_of_effect(collider* pAOE) { _area_of_effect = pAOE; }

  static void on_pause_update(event*, scd::base_component* pComp);

private:
  bool _is_spawned;
  scd::vector3 _position;
  scd::vector3 _direction;
  collider* _area_of_effect;
  int _id;
};

} // namespace scd
