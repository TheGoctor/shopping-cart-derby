#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

#define DONUTTIME 15.0f

namespace scd::component {

class donut : public scd::base_component {
private:
  float m_fTimeRemaining;
  float m_fDuration;
  scd::object* m_pParent;
  scd::object* m_pAttachedObject;
  bool m_bIsSpawned;
  int DONUT_ID;

public:
  donut(scd::object& owner)
      : base_component(owner) {}

  static donut* create(scd::object& owner);

  void first_init();

  // call backs
  static void update(event* cEvent, scd::base_component* cCenter);
  static void on_player_collision(event* cEvent, scd::base_component* cCenter);
  void reinit();
  void despawn();

  float time_remaining() const { return m_fTimeRemaining; }
  float duration() const { return m_fDuration; }
  bool is_spawned() const { return m_bIsSpawned; }

  void set_time_remaining(float fRemaining) { m_fTimeRemaining = fRemaining; }
  void set_duration(float fDuration) { m_fDuration = fDuration; }
  void set_spawned(bool bSpawned) { m_bIsSpawned = bSpawned; }
  void set_position(const scd::vector3& position) {
    _owner.set_local_position(position);
  }

  void set_attached_object(scd::object* pObj) { m_pAttachedObject = pObj; }
};

} // namespace scd::component
