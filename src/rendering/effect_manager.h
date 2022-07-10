////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectManager.h
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Loading and Spawning of
// Effects
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "components/rendering/effects/effect.h"
#include "components/rendering/effects/hud_effect.h"
#include "components/rendering/effects/victory_effect.h"
#include "components/rendering/effects/wind_sprite_effect.h"
#include "components/rendering/skid_marks.h"
#include "core/containers.h"
#include "core/object_manager.h"
#include "events/events.h"
#include "rendering/blind_sprite_effect.h"
#include "rendering/jam_sprite_effect.h"
#include "rendering/particle_emitter.h"
#include "rendering/renderer.h"
#include "rendering/steal_effect.h"

namespace scd {

using object_map = scd::map<unsigned int, scd::object*>;

// Effect Manager
class effect_manager {
private:
  // Map of Emitters (Key = ID of Emitter Name, Data = Emitter)
  scd::map<unsigned int, CParticleEmitter*> m_cEmitterTemplates;

  // Map of Clone Effects (Key = ID of Emitter Obj, Data = Clone Effects)
  scd::map<unsigned int, CParticleEmitter*> m_cClonedEffect;

  // Map of Emmiter Comps (Key = ID of Object Name, Data = Emitter Comp)
  scd::map<unsigned int, CEffectComponent*> m_cEmitterComps;


  // Skid
  CEffectComponent* pSkidEmitter;
  CEffectComponent* pDustEmitter;
  CEffectComponent* pBoostEmitter;

  // Bolts and Sparks
  scd::map<scd::object*, CEffectComponent*> m_cBoltsAndSparks;

  // Jam
  CJamSpriteEffect m_cJamEffect;

  // Blind
  CBlindSpriteEffect m_cBlindEffect;

  // Steal
  CStealEffect m_cStealEffect;

  // Boost
  CWindSpriteEffect m_cBoostEffect;

  // Goal Items
  int m_nGoalItemIconFrames[MAX_GOAL_ITEMS];

  CObjectManager* pOM;
  Renderer* pRenMan;

  // Helper Funcs
  void CreateCartCollisionComponent(CObject* pPlayerObj);
  void CreateBoostComponent(CObject* pPlayerObj);
  void CreateSlowComponent(CObject* pPlayerObj);
  void CreateCautionEffectComponent(CObject* pParentObj);
  void CreateGoalItemEffectComponent(CObject* pItemObj, EGoalItemType eType);
  void CreateTurkeyEffectComponent(CObject* pTurkeyObj);
  void CreatePieTrailEffectComponent(CObject* pPieObj);
  void SetupPlayerEmitters(void);
  void SetupJamSprite(void);
  void SetupBlindSprite(void);
  void SetupStealSprite(void);

  CEffectComponent* CreateBoltsAndSparksEffect(D3DXVECTOR3 vColPt);

public:
  effect_manager();
  ~effect_manager();

  // Init
  void InitObjects(void);

  void LoadEmitters(void);

  // Load Emitter
  void LoadEmitter(char* szFileName);

  // Get Emitter
  CParticleEmitter* GetCloneEmitter(string szEmitterName, CObject* pParentObj);

  // Factory
  CEffectComponent* CreateInvulnerableEffectComponent(CObject* pParent);
  CEffectComponent* CreateFireworkEffectComponent(CObject* pPlayerObj);
  CEffectComponent* CreateConfettiEffectComponent(CObject* pParentObj);
  CEffectComponent* CreateDustEffectComponent(CObject* pParentObj);
  CEffectComponent* CreateDonutUseComponent(CObject* pPlayerObj);
  CEffectComponent* CreateSlipDripComp(CObject* pParentObj, bool bLeft);
  CEffectComponent* CreateSlowMudComp(CObject* pParentObj);
  CEffectComponent* CreateStunComponent(CObject* pPlayerObj);
  CEffectComponent* CreatePieHitComponent(CObject* pParentObj);
  CEffectComponent* CreateSlipComponent(CObject* pPlayerObj);

  // Factory
  static CEffectComponent* CreateEffectComp(
      CObject* pParent,
      CParticleEmitter* pParticleEmitter,
      CRenderComponent* pRenderComp,
      EEmitterCompType eType,
      EParticleEmitterType eEmitterType);

  // Goal Item Init
  void GoalItemInit(TGoalItemEvent* pcObjEvent);
  static void GoalItemInitCallback(IEvent* pEvent, IComponent* pComp);

  // Goal Item Spawned
  void GoalItemSpawned(TGoalItemEvent* pcObjEvent);
  static void GoalItemSpawnedCallback(IEvent* pEvent, IComponent* pComp);

  // Goal Item Despawned
  void GoalItemDespawned(TGoalItemEvent* pcObjEvent);
  static void GoalItemDespawnedCallback(IEvent* pEvent, IComponent* pComp);

  // Goal Item Collected
  void GoalItemCollected(TGoalItemCollectedEvent* pcObjEvent);
  static void GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp);

  // Pickup Item Spawned
  void PickupItemSpawned(TPickupItemEvent* pcObjEvent);
  static void PickupItemSpawnedCallback(IEvent* pEvent, IComponent* pComp);

  // Pickup Item Dropped
  void PickupItemDropped(TPickupItemEvent* pcObjEvent);
  static void PickupItemDroppedCallback(IEvent* pEvent, IComponent* pComp);

  // Pickup Item Despawn
  void PickupItemDespawn(TObjectEvent* pcObjEvent);
  static void PickupItemDespawnCallback(IEvent* pEvent, IComponent* pComp);

  // Pickup Item Collected
  void PickupItemCollected(TPickupItemCollectedEvent* pcEvent);
  static void PickupItemCollectedCallback(IEvent* pEvent, IComponent* pComp);

  // Has 8
  void HaveEight(TObjectEvent* pcObjEvent);
  static void HaveEightCallback(IEvent* pEvent, IComponent* pComp);

  // Lost Item
  void LostItem(TGoalItemCollectedEvent* pcEvent);
  static void LostItemCallback(IEvent* pEvent, IComponent* pComp);

  // Inv
  void Invulnerable(TStatusEffectEvent* pcEvent);
  static void InvulnerableCallback(IEvent* pEvent, IComponent* pComp);

  // Cart Collision
  void CartCollision(TImpactEvent* pcObjEvent);
  static void CartCollisionCallback(IEvent* pEvent, IComponent* pComp);

  // Boost
  void Boost(TStatusEffectEvent* pcObjEvent);
  static void BoostCallback(IEvent* pEvent, IComponent* pComp);

  // Stun
  void Stun(TStatusEffectEvent* pcObjEvent);
  static void StunCallback(IEvent* pEvent, IComponent* pComp);

  // Turkey
  void TurkeySpawn(TObjectEvent* pcObjEvent);
  static void TurkeySpawnCallback(IEvent* pEvent, IComponent* pComp);
  void TurkeyFire(TObjectEvent* pcObjEvent);
  static void TurkeyFireCallback(IEvent* pEvent, IComponent* pComp);
  void TurkeyDespawn(TObjectEvent* pcObjEvent);
  static void TurkeyDespawnCallback(IEvent* pEvent, IComponent* pComp);

  // Pie
  void PieSpawn(TObjectEvent* pcObjEvent);
  static void PieSpawnCallback(IEvent* pEvent, IComponent* pComp);
  void PieFire(TObjectEvent* pcObjEvent);
  static void PieFireCallback(IEvent* pEvent, IComponent* pComp);
  void PieDespawn(TObjectEvent* pcObjEvent);
  static void PieDespawnCallback(IEvent* pEvent, IComponent* pComp);

  // Slip
  void Slip(TStatusEffectEvent* pcObjEvent);
  static void SlipCallback(IEvent* pEvent, IComponent* pComp);

  // Radar Jam
  void Jam(TStatusEffectEvent* pcObjEvent);
  static void JamCallback(IEvent* pEvent, IComponent* pComp);

  // Blind
  void Blind(TStatusEffectEvent* pcObjEvent);
  static void BlindCallback(IEvent* pEvent, IComponent* pComp);

  // Slow
  void Slow(TStatusEffectEvent* pcObjEvent);
  static void SlowCallback(IEvent* pEvent, IComponent* pComp);

  // Steal
  void Steal(IEvent* pEvent);
  static void StealCallback(IEvent* pEvent, IComponent* pComp);

  // Update
  void Update(float fDT);
  static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

  // Initalize
  static void InitObjectsCallback(IEvent* pEvent, IComponent* pComp);

  // Shutdown
  static void ShutdownCallback(IEvent*, IComponent*);
  static void ShutdownObjectsCallback(IEvent*, IComponent*);
  void ShutdownObjects(void);
  void Shutdown(void);

  // Destroy Obj
  static void DestroyObjCallback(IEvent* pEvent, IComponent*);

  // Reset
  static void EnableCallback(IEvent*, IComponent*);
  void Enable(void);
  static void DisableCallback(IEvent*, IComponent*);
};

} // namespace scd
