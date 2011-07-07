////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectComponent.h
//	Date			:	5/19/11
//	Mod. Date		:	5/19/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Effect
////////////////////////////////////////////////////////////////////////////////

// Header Protection
#ifndef _CEFFECTCOMPONENT_H_
#define _CEFFECTCOMPONENT_H_

// Includes
#include <list>
#include <map>
using namespace std;
#include "..\\..\\IComponent.h"
#include "..\\..\\Managers\\Global Managers\\Memory Manager\\CAllocator.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CParticleEmitter.h"

// Foward Declars
class CRenderComponent;
class IEvent;
class CObject;
class CFrame;

// Enumerations
enum EEmitterCompType	  { EC_TYPE_MIN,		    // 0
							EC_TYPE_GOAL_ITEM,	    // 1
							EC_TYPE_GOAL_ITEM_COLLECTION,	    // 1
							EC_TYPE_CART_COLLISION_CRASH, // 2
							//EC_TYPE_CART_COLLISION_POW, // 2
							EC_TYPE_CART_SMOKE,	    // 3
							EC_TYPE_CART_SKID_MARKS,// 4
							EC_TYPE_BOOST,			// 5
							EC_TYPE_HELD_ITEM,		// 6
							EC_TYPE_TURKEY_STUN,	// 7
							EC_TYPE_TURKEY_TRAIL,	// 8
							EC_TYPE_CHICKEN_INV,	// 9
							EC_TYPE_BANANA_SLIP,	// 10
							EC_TYPE_PEANUT_SQUISH,	// 11
							EC_TYPE_JAM_USE,		// 12
							EC_TYPE_JAM_HIT_FALL,	// 13
							EC_TYPE_JAM_HIT_POPUP,	// 14
							EC_TYPE_DONUT_USE,		// 15
							EC_TYPE_PIE_HIT,		// 16
							EC_TYPE_FIREWORK_TRAIL,		// 17
							EC_TYPE_FIREWORK_BURST,		// 18
							EC_TYPE_CONFETTI,		// 19
							EC_TYPE_CAUTION,		// 20
							EC_TYPE_SCIENTIST_GLOW,		// 21
							EC_TYPE_MAX };

// Type Definitions
typedef map<CParticleEmitter*, CRenderComponent*, less<CParticleEmitter*>,
		CAllocator<pair<CParticleEmitter*, CRenderComponent*>>> EmitterRenderCompMap;

// Structures
struct TEffect
{
	// Map of RenderComps (Key = Emitter, Data = Render Comps)
	EmitterRenderCompMap m_cEmitterRenderComps;

	// Type
	EEmitterCompType m_eType;

	// On
	bool m_bOn;

	// Cooldown Timer
	float m_fCooldown;

	// Lifespane (Switch to Burst)
	float m_fLifespan;

	// Timer to Turn On
	float m_fTimer;

	// Constructor
	TEffect(void);

	// Destructor
	~TEffect(void);

	// Update
	void Update(float fDT);

	// On/Off
	void SwitchOnOffEmitters(bool bOn);

	// Set on Target
	void SetOnTarget(bool bOnTarget);

	// Set Continuous
	void SetContinuous(bool bContinuous);

	// Set Parent Frame
	void SetParentFrame(CFrame* pFrame);

	// Timer
	void SetETimer(float fTimer);

	// Shutdown
	void Shutdown(void);

	// Change Context
	void ChangeContext(EParticleEmitterType eEmitterType, unsigned int uRenderContextIdx);
};

typedef map<EEmitterCompType, TEffect, less<EEmitterCompType>,
		CAllocator<pair<EEmitterCompType, TEffect>>> EffectMap;

// Effect Component
class CEffectComponent : public IComponent
{
private:

	// Parent Object
	CObject* m_pcParent;

	// List of Effects
	EffectMap m_cEffectMap;

	// Callback Handlers
	void Update(float fDT);

public:
	
	// Constructor
	CEffectComponent(CObject* pParent);

	// Destructor
	~CEffectComponent(void);

	// Callbacks
	static void UpdateCallback(IEvent* pEvent, IComponent* pComp);
	static void ActivateCallback(IEvent* pEvent, IComponent* pComp);
	static void WinStateCallback(IEvent* pEvent, IComponent* pComp);
	static void DeactivateCallback(IEvent* pEvent, IComponent* pComp);
	static void ShutdownCallback(IEvent* pEvent, IComponent* pComp);
	void Shutdown(void);
	void WinState(void);

	static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);


	// Add Emitter
	void AddEmitter(EEmitterCompType eType, CParticleEmitter* pEmitter,
		CRenderComponent* pRenderComp, CFrame* pParentFrame, EParticleEmitterType eEmitterType);

	// Switch Emiiter On/Off
	void SwitchOnOffEmitters(EEmitterCompType eType, bool bOn);
	void SetOnTarget(EEmitterCompType eType, bool bOnTarget);

	// Timer
	void SetEffectTimer(EEmitterCompType eType, float fTimer);

	// Change Context
	void ChangeContext(EEmitterCompType eCompType, EParticleEmitterType eEmitterType, unsigned int uRenderContextIdx);

	// Accessors
	CObject* GetParent(void) { return m_pcParent; }
	float GetCooldown(EEmitterCompType eType) { return m_cEffectMap[eType].m_fCooldown; }
	bool GetIsActiveEmitter(EEmitterCompType eType) { return m_cEffectMap[eType].m_bOn; }
	//EEmitterCompType GetType(void) { return m_eType; }
	//bool IsOn(void) { return m_bOn; }

	// Mutators
	void SetCooldown(EEmitterCompType eType, float fCooldown)
	{
		m_cEffectMap[eType].m_fCooldown = fCooldown;
	}
	void SetLifespan(EEmitterCompType eType, float fLifespan)
	{
		m_cEffectMap[eType].SetContinuous(true);
		m_cEffectMap[eType].m_fLifespan = fLifespan;
	}
	void SetContinuous(EEmitterCompType eType, bool bContinuous);
	void SetParentFrame(EEmitterCompType eType, CFrame* pFrame);
};

#endif // _CEFFECTCOMPONENT_H_