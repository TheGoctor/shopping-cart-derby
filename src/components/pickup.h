#pragma once

#include "core/base_component.h"
#include "enums.h"

namespace scd {

class pickup : public scd::base_component {
private:
  float m_fTimeLeft;
  float m_fLifetime;
  float m_fFlyOutSpeed;
  float m_fTimeToFly;
  float m_fMaxArcHeight;

  EGoalItemType m_nItemType;

  bool m_bRendering;
  bool m_bSpawned;

  CRenderComponent* m_pRenderComp;

  scd::vector3 m_vDirectionToMove;

public:
  pickup(scd::object& owner);

  static int create(lua_State* pLua);
  static pickup* create(scd::object& pObj, scd::vector3 vDirectionToMove);

  void Init();

  static void DestroyPickupItem(CPickupItemComponent* toDestroy);

  bool ready_to_collide() const {
    return (m_fTimeLeft < m_fLifetime - m_fTimeToFly) && (m_fTimeLeft > 0.0f);
  }

  float arc_y() const {
    return 1.0f
           + std::sin(((m_fLifetime - m_fTimeLeft) / m_fTimeToFly) * 3.1415f)
               * m_fMaxArcHeight;
  }

  EGoalItemType type() const { return m_nItemType; }

  bool spawned() const { return m_bSpawned; }

  friend class pickup_manager;

  void event_init();

  void set_mesh(unsigned int mesh_id);

  void on_update();
  void on_collision();
  void on_pause();
};

} // namespace scd
