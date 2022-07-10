#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/math_types.h"

namespace scd::component {

class banana : public scd::base_component {
public:
  banana(scd::object& owner, scd::event_manager& event_manager);
  ~banana();

  void spawn();
  void despawn();

  bool is_spawned() const { return _is_spawned; }
  collider* area_of_effect() const { return _area_of_effect; }

  void is_spawned(bool bSpawned) { _is_spawned = bSpawned; }
  void area_of_effect(collider* pAOE) { _area_of_effect = pAOE; }

  void on_update(float dt);
  void on_player_collision(const scd::event::impact& data);
  void on_item_collision(const scd::event::impact& data);

private:
  bool _is_spawned{true};
  collider* _area_of_effect{nullptr};
  int _audio_id{-1};
};

} // namespace scd::component
