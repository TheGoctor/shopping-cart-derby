////////////////////////////////////////////////////////////////////////////////
//	File			:	CEffectComponent.h
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Lets an Object have a Effect
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Header Protection
///////////////////////////////////////////////////////////////////////////////
#ifndef _CEFFECTCOMPONENT_H_
#define _CEFFECTCOMPONENT_H_

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
#include <list>
#include <map>
using namespace std;
#include "..\\..\\IComponent.h"
#include "..\\..\\Managers\\Global Managers\\Memory Manager\\CAllocator.h"
#include "..\\..\\Managers\\Global Managers\\Rendering Managers\\CParticleEmitter.h"

///////////////////////////////////////////////////////////////////////////////
// Foward Declares
///////////////////////////////////////////////////////////////////////////////
class CRenderComponent;
class IEvent;
class CObject;
class CFrame;

///////////////////////////////////////////////////////////////////////////////
// Enumerations
///////////////////////////////////////////////////////////////////////////////
enum EEmitterCompType	  { EC_TYPE_MIN,					// 0
							EC_TYPE_GOAL_ITEM,				// 1
							EC_TYPE_GOAL_ITEM_COLLECTION,	// 2
							EC_TYPE_CART_COLLISION_CRASH,	// 3
							EC_TYPE_CART_SMOKE,				// 4
							EC_TYPE_CART_MUD,				// 5
							EC_TYPE_BOOST,					// 6
							EC_TYPE_TURKEY_STUN,			// 8
							EC_TYPE_TURKEY_TRAIL,			// 9
							EC_TYPE_PIE_TRAIL,				// 10
							EC_TYPE_CHICKEN_INV,			// 11
							EC_TYPE_BANANA_SLIP,			// 12
							EC_TYPE_PEANUT_SQUISH,			// 13
							EC_TYPE_JAM_USE,				// 14
							EC_TYPE_JAM_HIT_FALL,			// 15
							EC_TYPE_JAM_HIT_FALL_B,			// 16
							EC_TYPE_JAM_HIT_POPUP,			// 17
							EC_TYPE_DONUT_USE,				// 18
							EC_TYPE_PIE_HIT,				// 19
							EC_TYPE_FIREWORK_TRAIL,			// 20
							EC_TYPE_FIREWORK_BURST,			// 21
							EC_TYPE_COL_BURST,				// 22
							EC_TYPE_CONFETTI,				// 23
							EC_TYPE_CAUTION,				// 24
							EC_TYPE_MAX };					// 25

///////////////////////////////////////////////////////////////////////////////
// Type Definitions
///////////////////////////////////////////////////////////////////////////////
typedef map<CParticleEmitter*, CRenderComponent*, less<CParticleEmitter*>,
		CAllocator<pair<CParticleEmitter*, CRenderComponent*>>> EmitterRenderCompMap;

///////////////////////////////////////////////////////////////////////////////
// Effect Structure
///////////////////////////////////////////////////////////////////////////////
struct TEffect
{
	///////////////////////////////////////////////////////////////////////////
	// Data Members
	///////////////////////////////////////////////////////////////////////////

		EmitterRenderCompMap m_cEmitterRenderComps; // Map of RenderComps (Key = Emitter, Data = Render Comps)
		float m_fCooldown;							// Cooldown Timer
		float m_fLifespan;							// Lifespane (Switch to Burst)
		float m_fTimer;								// Timer to Turn On
		float m_fDeadTimer;							// Dead Timer
		int m_nEmitterCount;						// Count
		EEmitterCompType m_eType;					// Type
		bool m_bOn;									// On

	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	TEffect(void);

	///////////////////////////////////////////////////////////////////////////
	// Destructor
	///////////////////////////////////////////////////////////////////////////
	~TEffect(void);

	///////////////////////////////////////////////////////////////////////////
	// Function: “Update”
	//
	// Return: void
	//
	// Parameters: float fDT - The time since the last Frame
	//
	// Purpose: This Functions is used to Handle Updating the Effects by
	//			Updating Timers, Emitters and Rendering the Effects.
	///////////////////////////////////////////////////////////////////////////
	void Update(float fDT);

	///////////////////////////////////////////////////////////////////////////
	// Function: “Shutdown”
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: This Functions is used to Handle Shuting down Emitter Objects
	//			by Destroying the Objects associated with each Sub-Emitter on
	//			Destruction of Parent Objects.
	///////////////////////////////////////////////////////////////////////////
	void Shutdown(void);

	///////////////////////////////////////////////////////////////////////////
	// Mutators
	///////////////////////////////////////////////////////////////////////////

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
		void ChangeContext(EParticleEmitterType eEmitterType,
			unsigned int uRenderContextIdx);
		
		// Animation Frame (Passed in Emitter)
		void SetFrame(EParticleEmitterType eEmitterType, int nFrame);
};

///////////////////////////////////////////////////////////////////////////////
// Type Definitions
///////////////////////////////////////////////////////////////////////////////
typedef map<EEmitterCompType, TEffect, less<EEmitterCompType>,
		CAllocator<pair<EEmitterCompType, TEffect>>> EffectMap;

///////////////////////////////////////////////////////////////////////////////
// Effect Component Class
///////////////////////////////////////////////////////////////////////////////
class CEffectComponent : public IComponent
{
private:

	///////////////////////////////////////////////////////////////////////////
	// Data Members
	///////////////////////////////////////////////////////////////////////////

		CObject* m_pcParent;	// Parent Object
		EffectMap m_cEffectMap;	// Map of Effects

	///////////////////////////////////////////////////////////////////////////
	// Event Handlers
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “Update”
		//
		// Return: void
		//
		// Parameters: float fDT - The time since the last Frame
		//
		// Purpose: This Functions is used to Handle Updating the Effects by
		//			Updating each Effect in the Map.
		///////////////////////////////////////////////////////////////////////
		void Update(float fDT);

		///////////////////////////////////////////////////////////////////////
		// Function: “WinState”
		//
		// Return: void
		//
		// Parameters: void
		//
		// Purpose: This Functions is used to Handle Updating during Win State.
		///////////////////////////////////////////////////////////////////////
		void WinState(void);

public:

	///////////////////////////////////////////////////////////////////////////
	// Function: “Shutdown”
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: This Functions is used to Handle Shuting down Emitter Objects
	//			by Destroying the Objects associated with each Effect and 
	//			Sub-Emitter on Destruction of Parent Objects.
	///////////////////////////////////////////////////////////////////////////
	void Shutdown(void);
	
	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	CEffectComponent(CObject* pParent);

	///////////////////////////////////////////////////////////////////////////
	// Destructor
	///////////////////////////////////////////////////////////////////////////
	~CEffectComponent(void);

	///////////////////////////////////////////////////////////////////////////
	// Function: “AddEmitter”
	//
	// Return: void
	//
	// Parameters: EEmitterCompType eType		     - Effect Type to Add
	//			   CParticleEmitter* pEmitter	     - Emitter to Add
	//			   CRenderComponent* pRenderComp     - Render Comp of Emitter
	//			   EParticleEmitterType eEmitterType - Emitter Type
	//
	// Purpose: This Functions is used to Add a Pair of Render Components and
	//			Emitters to the Map of Effects.
	///////////////////////////////////////////////////////////////////////////
	void AddEmitter(EEmitterCompType eType, CParticleEmitter* pEmitter,
		CRenderComponent* pRenderComp, CFrame* pParentFrame,
		EParticleEmitterType eEmitterType);

	///////////////////////////////////////////////////////////////////////////
	// Event Callbacks
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “UpdateCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle Updating the Passed in
		//			Component's Effects and Check for end to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////
		static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////
		// Function: “WinStateCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle Updating the Passed in
		//			Component's Effects during Win State.
		///////////////////////////////////////////////////////////////////////
		static void WinStateCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////
		// Function: “DestroyObjectCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle Shutting down all Effects
		//			associated with the Parent Object on Destruction.
		///////////////////////////////////////////////////////////////////////
		static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	///////////////////////////////////////////////////////////////////////////
	// Accessors
	///////////////////////////////////////////////////////////////////////////
	
		// Parent Object
		CObject* GetParent(void) { return m_pcParent; }

		// Cooldown Timer (Passed in Effect)
		float GetCooldown(EEmitterCompType eType)
		{ return m_cEffectMap[eType].m_fCooldown; }

		// Active Flag (Passed in Effect)
		bool GetIsActiveEmitter(EEmitterCompType eType)
		{ return m_cEffectMap[eType].m_bOn; }

	///////////////////////////////////////////////////////////////////////////
	// Mutators
	///////////////////////////////////////////////////////////////////////////
	
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
		void ChangeContext(EEmitterCompType eCompType, EParticleEmitterType eEmitterType, unsigned int uRenderContextIdx);
 
		// Animation Frame (Passed in Effect, Passed in Emitter)
		void SetFrame(EEmitterCompType eCompType, EParticleEmitterType eEmitterType, int nFrame);
};
#endif // _CEFFECTCOMPONENT_H_