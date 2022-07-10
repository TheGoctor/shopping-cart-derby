#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

#define SLOWTIME (2.0f)
#define SLOWRATE (0.3f)

namespace scd::component {

class peanut_butter : public scd::base_component {
private:
  float _duration{15.0f};
  float _time_remaining;
  float _slow_rate; // something to mult the speed by to gain the slowed effect
  bool _is_tossed_forward{false};
  bool _is_spawned{true};
  CCollideable* _area_of_effect;
  float _effect_duration{5.0f};
  float _sound_cooldown{0.0f};

  int PEANUT_BUTTER_ID;

public:
  peanut_butter(scd::object& pObj);
  static std::shared_ptr<peanut_butter> create(scd::object& owner);

  void on_update(float dt);
  void on_player_collision();
  void on_item_collision();
  void on_use();

  void despawn();

  CCollideable* aoe() const { return _area_of_effect; }
  float time_remaining() const { return _time_remaining; }
  float duration() const { return _duration; }
  bool is_tossed_forward() const { return _is_tossed_forward; }
  bool is_spawned() { return _is_spawned; }

  void time_remaining(float value) { _time_remaining = value; }
  void duration(float value) { _duration = value; }
  void aoe(CCollideable* value) { _area_of_effect = value; }
  void is_spawned(bool value) { _is_spawned = value; }
  void is_tossed_forward(bool value) { _is_tossed_forward = value; }

  void on_pause_update(float dt);
};

} // namespace scd::component
