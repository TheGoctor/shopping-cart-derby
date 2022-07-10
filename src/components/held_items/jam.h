#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

#define JAM_TIME 10.0f

namespace scd {
namespace component {

class jam : public scd::base_component {
private:
  scd::object* m_pParent;
  bool m_bIsSpawned;
  float m_fDuration;
  float m_fTimeLeft;
  int JAM_ID;

public:
  jam(scd::object* pObj)
      : m_pParent(pObj) {}
  static CRayJam* CreateRayJamComponent(scd::object* pObj);
  void FirstInit();
  // call backs
  static void Update(IEvent* cEvent, scd::base_component* cCenter);
  static void ReInit(IEvent* cEvent, scd::base_component* cCenter);
  void Despawn();
  // accessors
  scd::object* GetParent(void) { return m_pParent; }
  bool GetIsSpawned(void) { return m_bIsSpawned; }
  float GetDuration(void) { return m_fDuration; }
  float GetTimeLeft(void) { return m_fTimeLeft; }
  // mutators
  void SetParent(scd::object* pParent) { m_pParent = pParent; }
  void SetIsSpawned(bool bIsSpawned) { m_bIsSpawned = bIsSpawned; }
  void SetDuration(float fDuration) { m_fDuration = fDuration; }
  void SetTimeLeft(float fRemaining) { m_fTimeLeft = fRemaining; }
};

} // namespace component
} // namespace scd
