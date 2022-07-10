#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/math_types.h"
#include "physics/physics_manager.h"

namespace scd::component {
class collider;
class event;
class renderable;

class banana : public scd::base_component {
public:
  banana(scd::object& owner);
  ~banana();

  void spawn();
  void despawn();

  bool is_spawned() const { return _is_spawned; }
  collider* area_of_effect() const { return _area_of_effect; }

  void is_spawned(bool bSpawned) { _is_spawned = bSpawned; }
  void area_of_effect(collider* pAOE) { _area_of_effect = pAOE; }

  void on_update();
  void on_player_collision(event* cEvent);
  void on_item_collision(event* cEvent);

private:
  bool _is_spawned;
  collider* _area_of_effect;
  int _id;
};

} // namespace scd::component
