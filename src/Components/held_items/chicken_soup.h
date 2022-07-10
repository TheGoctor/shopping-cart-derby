#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

#define SOUPTIME 5.0f

namespace scd::component {

class chicken_soup : public scd::base_component {
private:
  float m_fTimeRemaining;
  float m_fDuration;
  scd::object* m_pParent;
  bool m_bIsSpawned;
  scd::object* m_pAttachedObject;
  scd::vector3 m_vPosition; // world space position
  int CHICKENSOUP_ID;

public:
  chicken_soup(const scd::object& owner)
      : base_component(owner) {}

  static chicken_soup* create(const scd::object& owner);

  void first_init();

  // call backs
  static void on_update(IEvent* cEvent, scd::base_component* cCenter);
  static void on_player_collision(IEvent* cEvent, scd::base_component* cCenter);
  static void on_item_collision(IEvent* cEvent, scd::base_component* cCenter);

  void reinit();
  void despawn();

  float time_remaining() const { return m_fTimeRemaining; }
  float duration() const { return m_fDuration; }
  bool is_spawned() const { return m_bIsSpawned; }

  void set_time_remaining(float fRemaining) { m_fTimeRemaining = fRemaining; }
  void set_position(const scd::vector3& vPos) {
    _owner.set_local_position(vPos);
  }

  void set_attached_object(scd::object* pObj) { m_pAttachedObject = pObj; }

  // Warning, only call once
  void set_duration(float fDuration) {
    // only call this function once, you have been warned
    static bool bHasBeenCalled = false;

    if (!bHasBeenCalled) {
      m_fDuration = fDuration;
      bHasBeenCalled = true; // don't do it again, duration is not to be changed
    }
  }

  void set_spawned(bool bIsSpawned) { m_bIsSpawned = bIsSpawned; }
};

} // namespace scd::component
