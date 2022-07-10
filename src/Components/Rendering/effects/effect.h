////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectComponent.h
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Effect
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/base_component.h"
#include "rendering/particle_emitter.h"

namespace scd {
enum EEmitterCompType {
  EC_TYPE_MIN,                  // 0
  EC_TYPE_GOAL_ITEM,            // 1
  EC_TYPE_GOAL_ITEM_COLLECTION, // 2
  EC_TYPE_CART_COLLISION_CRASH, // 3
  EC_TYPE_CART_SMOKE,           // 4
  EC_TYPE_CART_MUD,             // 5
  EC_TYPE_BOOST,                // 6
  EC_TYPE_TURKEY_STUN,          // 8
  EC_TYPE_TURKEY_TRAIL,         // 9
  EC_TYPE_PIE_TRAIL,            // 10
  EC_TYPE_CHICKEN_INV,          // 11
  EC_TYPE_BANANA_SLIP,          // 12
  EC_TYPE_PEANUT_SQUISH,        // 13
  EC_TYPE_JAM_USE,              // 14
  EC_TYPE_JAM_HIT_FALL,         // 15
  EC_TYPE_JAM_HIT_FALL_B,       // 16
  EC_TYPE_JAM_HIT_POPUP,        // 17
  EC_TYPE_DONUT_USE,            // 18
  EC_TYPE_PIE_HIT,              // 19
  EC_TYPE_FIREWORK_TRAIL,       // 20
  EC_TYPE_FIREWORK_BURST,       // 21
  EC_TYPE_COL_BURST,            // 22
  EC_TYPE_CONFETTI,             // 23
  EC_TYPE_CAUTION,              // 24
  EC_TYPE_MAX
}; // 25

typedef scd::map<CParticleEmitter*, CRenderComponent*> EmitterRenderCompMap;

struct effect_data {
  EmitterRenderCompMap m_cEmitterRenderComps; // Map of RenderComps (Key =
                                              // Emitter, Data = Render Comps)
  float m_fCooldown;                          // Cooldown Timer
  float m_fLifespan;                          // Lifespane (Switch to Burst)
  float m_fTimer;                             // Timer to Turn On
  float m_fDeadTimer;                         // Dead Timer
  int m_nEmitterCount;                        // Count
  EEmitterCompType m_eType;                   // Type
  bool m_bOn;                                 // On

  effect_data();
  ~effect_data();

  /**
   * @brief Updates the effect's timers and emitters, and renders the effects.
   *
   * @param dt The number of seconds since the last update.
   */
  void update(float dt);

  // On/Off (All Emitters)
  void SwitchOnOffEmitters(bool bOn);

  // On Target (All Emitters)
  void SetOnTarget(bool bOnTarget);

  // Continuous (All Emitters)
  void SetContinuous(bool bContinuous);

  // Parent Frame (All Emitters)
  void SetParentFrame(CFrame* pFrame);

  // Effect Timer (All Emitters)
  void SetETimer(float fTimer) { m_fTimer = fTimer; }

  // Dead Timer (All Emitters)
  void SetDeadTimer(float fTimer) { m_fDeadTimer = fTimer; }

  // Context (Passed in Emitter)
  void ChangeContext(
      EParticleEmitterType eEmitterType,
      unsigned int uRenderContextIdx);

  // Animation Frame (Passed in Emitter)
  void SetFrame(EParticleEmitterType eEmitterType, int nFrame);
};

typedef scd::map<EEmitterCompType, TEffect> EffectMap;

namespace component {

class effect : public scd::base_component {
private:
  EffectMap m_cEffectMap; // Map of Effects

  void on_update(float dt);

  void WinState();

public:
  effect(scd::object& owner);
  ~effect();

  ///////////////////////////////////////////////////////////////////////////
  // Function: �AddEmitter�
  //
  // Return: void
  //
  // Parameters: EEmitterCompType eType		     - Effect Type to Add
  //			   CParticleEmitter* pEmitter	     - Emitter to Add
  //			   CRenderComponent* pRenderComp     - Render Comp of
  // Emitter 			   EParticleEmitterType eEmitterType - Emitter
  // Type
  //
  // Purpose: This Functions is used to Add a Pair of Render Components and
  //			Emitters to the Map of Effects.
  ///////////////////////////////////////////////////////////////////////////
  void add_emitter(
      EEmitterCompType type,
      const std::shared_ptr<particle_emitter>& emitter,
      const std::shared_ptr<renderable>& render_component,
      CFrame* parent_frame,
      particle_emitter::emitter_type emitter_type);

  ///////////////////////////////////////////////////////////////////////
  // Function: �UpdateCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Updating the Passed in
  //			Component's Effects and Check for end to Switch Effects
  // Off.
  ///////////////////////////////////////////////////////////////////////
  void on_update(float dt);

  ///////////////////////////////////////////////////////////////////////
  // Function: �WinStateCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Updating the Passed in
  //			Component's Effects during Win State.
  ///////////////////////////////////////////////////////////////////////
  void on_win_state(scd::event& pEvent);

  ///////////////////////////////////////////////////////////////////////
  // Function: �DestroyObjectCallback�
  //
  // Return: void
  //
  // Parameters: IEvent* pEvent	 - Event Data
  //			   IComponent* pComp - The Listening Component
  //
  // Purpose: This Functions is used to Handle Shutting down all Effects
  //			associated with the Parent Object on Destruction.
  ///////////////////////////////////////////////////////////////////////
  void on_destroy_object(scd::event& pEvent);

  // Cooldown Timer (Passed in Effect)
  float cooldown(EEmitterCompType eType) const {
    return m_cEffectMap[eType].m_fCooldown;
  }

  // Active Flag (Passed in Effect)
  bool is_emitter_active(EEmitterCompType eType) {
    return m_cEffectMap[eType].m_bOn;
  }

  // Cooldown (Passed in Effect)
  void SetCooldown(EEmitterCompType eType, float fCooldown);

  // Lifespan (Passed in Effect)
  void SetLifespan(EEmitterCompType eType, float fLifespan);

  // Coninuous (Passed in Effect)
  void SetContinuous(EEmitterCompType eType, bool bContinuous);

  // Parent Frame (Passed in Effect)
  void SetParentFrame(EEmitterCompType eType, CFrame* pFrame);

  // On/Off (Passed in Effect)
  void SwitchOnOffEmitters(EEmitterCompType eType, bool bOn);

  // On Target (Passed in Effect)
  void SetOnTarget(EEmitterCompType eType, bool bOnTarget);

  // Effect Timer (Passed in Effect)
  void SetEffectTimer(EEmitterCompType eType, float fTimer);

  // Dead Timer (Passed in Effect)
  void SetDeadTimer(EEmitterCompType eType, float fTimer);

  // Context (Passed in Effect, Passed in Emitter)
  void ChangeContext(
      EEmitterCompType eCompType,
      EParticleEmitterType eEmitterType,
      unsigned int uRenderContextIdx);

  // Animation Frame (Passed in Effect, Passed in Emitter)
  void SetFrame(
      EEmitterCompType eCompType,
      EParticleEmitterType eEmitterType,
      int nFrame);
};

} // namespace component
} // namespace scd
