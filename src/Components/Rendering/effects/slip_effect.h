////////////////////////////////////////////////////////////////////////////////
//	File			:	CSlipVFXComp.h
//	Date			:	6/26/11
//	Mod. Date		:	6/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Slip VFX
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "components/component_manager.h"
#include "components/rendering/effects/effect.h"
#include "core/base_component.h"
#include "core/object.h"

namespace scd {

// Slip VFX
class CSlipVFXComp : public scd::base_component {
private:
  // Parent Object
  CObject* m_pParentObj;

  // Emitter Objs
  CObject* m_pLeftEmitter;
  CObject* m_pRightEmitter;

  // Effect Comps
  CEffectComponent* m_pLeftEffect;
  CEffectComponent* m_pRightEffect;

  // Cooldowns
  float m_fSlowCooldown;

  // Handlers
  void Slip(TStatusEffectEvent* pcEvent);
  void Slow(TStatusEffectEvent* pcEvent);
  void Update(float fDT);

  // Helper Funcs
  void CreateLeftDrip(void);
  void CreateRightDrip(void);

public:
  // Get Auto-Manager
  static CComponentManager<CSlipVFXComp>* GetManager(void) {
    // Auto-Manager
    static CComponentManager<CSlipVFXComp> m_cManager;

    return &m_cManager;
  }

  // Initalize
  void Init(void);

  // Shutdown
  void Shutdown(void);

  // Constructor
  CSlipVFXComp(CObject* pParent)
      : m_pParentObj(pParent)
      , m_pLeftEmitter(NULL)
      , m_pRightEmitter(NULL)
      , m_pLeftEffect(NULL)
      , m_pRightEffect(NULL)
      , m_fSlowCooldown(0.0f) {
    // Add to Auto Manager
    GetManager()->Add(this);

    // Add Component to Parent
    pParent->AddComponent(this);

    // Initalize
    Init();
  }

  // Destructor
  ~CSlipVFXComp(void) {
    // Shutdown();
    GetManager()->Remove(this);
  }

  // Factory
  static CSlipVFXComp* CreateSlipVFXComponent(CObject* pParent) {
    return MMNEW(CSlipVFXComp(pParent));
  }

  // Callbacks
  static void SlipCallback(IEvent* pEvent, IComponent* pComp);
  static void SlowCallback(IEvent* pEvent, IComponent* pComp);
  static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
  static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

  // Accessors
  CObject* GetParent(void) { return m_pParentObj; }
};
} // namespace scd
