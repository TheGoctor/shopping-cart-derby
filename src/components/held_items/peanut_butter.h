#pragma once

#include "core/base_component.h"
#include "core/object.h"
#include "physics/collider.h"
#include "physics/physics_manager.h"

#define SLOWTIME (2.0f)
#define SLOWRATE (0.3f)

namespace scd {
namespace component {

class peanut_butter : public scd::base_component {
private:
  float m_fDuration;
  float m_fTimeRemaining;
  float m_fSlowRate; // something to mult the speed by to gain the slowed effect
  bool m_bIsTossedForward;
  scd::object* m_pParent;
  bool m_bIsSpawned;
  scd::vector3 m_vPosition;
  CCollideable* m_pcAreaOfEffect;
  float m_fEffectDuration;
  float m_fSoundCooldown;
  int PEANUT_BUTTER_ID;

public:
  peanut_butter(scd::object* pObj)
      : m_pParent(pObj) {}
  static CPeanutButter* CreatePeanutButterComponent(scd::object* pObj);
  void FirstInit(void);
  // call backs
  static void Update(IEvent* cEvent, scd::base_component* cCenter);
  static void PlayerCollision(IEvent* cEvent, scd::base_component* cCenter);
  static void ItemCollision(IEvent* cEvent, scd::base_component* cCenter);

  void ReInit();
  void Despawn();
  // bananaccessors
  CCollideable* GetAOE(void) { return m_pcAreaOfEffect; }
  float GetTimeRemaining(void) { return m_fTimeRemaining; }
  float GetDuration(void) { return m_fDuration; }
  bool GetIsTossedForward(void) { return m_bIsTossedForward; }
  scd::vector3 GetPosition(void) { return m_vPosition; }
  scd::object* GetParent(void) { return m_pParent; }
  bool GetIsSpawned(void) { return m_bIsSpawned; }
  // mutananas
  void SetTimeRemaining(float fTime) { m_fTimeRemaining = fTime; }
  void SetPosition(scd::vector3 vPos) {
    m_pParent->GetTransform()->GetLocalMatrix()._41 = vPos.x;
    m_pParent->GetTransform()->GetLocalMatrix()._42 = vPos.y;
    m_pParent->GetTransform()->GetLocalMatrix()._43 = vPos.z;
  }
  void SetDuration(float fDuration) { m_fDuration = fDuration; }
  void SetAOE(CCollideable* pAOE) { m_pcAreaOfEffect = pAOE; }
  void SetParent(scd::object* pParent) { m_pParent = pParent; }
  void SetIsSpawned(bool bIsSpawned) { m_bIsSpawned = bIsSpawned; }
  void SetIsTossedForward(bool bForward) { m_bIsTossedForward = bForward; }

  static void PauseUpdateCallback(IEvent*, scd::base_component* pComp);
};

} // namespace component
} // namespace scd
