////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectManager.h
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Loading and Spawning of Effects
////////////////////////////////////////////////////////////////////////////////

// Header Protexction
#ifndef _CEFFECTMANAGER_H_
#define _CEFFECTMANAGER_H_

// Includes
#include <map>
#include <list>
using namespace std;
#include "CParticleEmitter.h"
#include "CJamSpriteEffect.h"
#include "CBlindSpriteEffect.h"
#include "CStealEffect.h"
#include "..\Memory Manager\CAllocator.h"
#include "..\Event Manager\EventStructs.h"
#include "..\\..\\..\\Components\\Rendering\\CEffectComponent.h"
#include "..\\..\\..\\Components\\Rendering\\CSkidMarks.h"
#include "..\\..\\..\\Components\\Rendering\\CVictoryVFXComp.h"
#include "..\\..\\..\\Components\\Rendering\\CWindSpriteEffect.h"
#include "..\\..\\..\\Components\\Rendering\\CHUDEffectsComp.h"
#include "Renderer.h"
#include "..\\Object Manager\\CObjectManager.h"

using namespace EventStructs;

// Foward Declares
class CRenderComponent;
class IEvent;
class IComponent;
class CObject;
class CSpriteComponent;
class CHeldItemComponent;

// Type Definitions
typedef map<unsigned int, CParticleEmitter*, less<unsigned int>,
		CAllocator<pair<unsigned int, CParticleEmitter*>>> EmitterMap;

typedef map<unsigned int, CEffectComponent*, less<unsigned int>,
		CAllocator<pair<unsigned int, CEffectComponent*>>> EffectCompMap;

typedef map<CObject*, CEffectComponent*, less<CObject*>,
		CAllocator<pair<CObject*, CEffectComponent*>>> ObjEffectCompMap;

typedef map<unsigned int, CObject*, less<unsigned int>,
		CAllocator<pair<unsigned int, CObject*>>> ObjMap;

// Effect Manager
class CEffectManager
{
private:

	// Map of Emitters (Key = ID of Emitter Name, Data = Emitter)
	EmitterMap m_cEmitterTemplates;

	// Map of Clone Effects (Key = ID of Emitter Obj, Data = Clone Effects)
	EmitterMap m_cClonedEffect;

	// Map of Emmiter Comps (Key = ID of Object Name, Data = Emitter Comp)
	EffectCompMap m_cEmitterComps;

	// Singleton Functions
	CEffectManager();
	CEffectManager(CEffectManager &ref);
	CEffectManager &operator=(CEffectManager &ref);

	// First Init
	bool m_bFirstInit;

	// Skid
	CEffectComponent* pSkidEmitter;
	CEffectComponent* pDustEmitter;
	CEffectComponent* pBoostEmitter;

	// Bolts and Sparks
	ObjEffectCompMap m_cBoltsAndSparks; 

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

	// Get Instance
	static CEffectManager* GetInstance(void);

	// Init
	void Init(void);
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
	static CEffectComponent* CreateEffectComp(CObject* pParent, CParticleEmitter* pParticleEmitter, CRenderComponent* pRenderComp,
		EEmitterCompType eType, EParticleEmitterType eEmitterType);

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
	static void InitObjectsCallback(IEvent *pEvent, IComponent* pComp);

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

#endif // _CEFFECTMANAGER_H_