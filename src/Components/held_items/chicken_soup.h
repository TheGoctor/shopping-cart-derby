#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

namespace scd::component {

class chicken_soup : public scd::base_component {
private:
  float _duration{10.0f};
  float _time_remaining{_duration};
  bool _is_spawned{true};
  int _audio_id{-1};

  scd::object* _attached_object;

  static constexpr float soup_effect_duration{5.0f};

public:
  chicken_soup(scd::object& owner, scd::event_manager& event_manager);

  static std::shared_ptr<chicken_soup>
  create(scd::object& owner, scd::event_manager& event_manager);

  // call backs
  void on_update(float dt);
  void on_player_collision(IEvent* cEvent);
  void on_item_collision(IEvent* cEvent);

  void reinit();
  void despawn();

  float time_remaining() const { return _time_remaining; }
  float duration() const { return _duration; }
  bool is_spawned() const { return _is_spawned; }

  void time_remaining(float value) { _time_remaining = value; }
  void set_position(const scd::vector3& position) {
    _owner.local_position({position.x, 0.5f, position.z});
  }

  void attach_object(scd::object* obj) { _attached_object = obj; }
};

} // namespace scd::component
