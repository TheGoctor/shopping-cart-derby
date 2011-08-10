////////////////////////////////////////////////////////////////////////////////
//	File			:	CAnimatedDonutComp.h
//	Date			:	7/26/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Animated Donut
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Header Protection
///////////////////////////////////////////////////////////////////////////////
#ifndef _CANIMATEDDONUTCOMP_H_
#define _CANIMATEDDONUTCOMP_H_

///////////////////////////////////////////////////////////////////////////////
// LUA
///////////////////////////////////////////////////////////////////////////////
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
#include "CAnimateComponent.h"
#include "CEffectComponent.h"
#include "..\\..\\IComponent.h"
#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Component Managers\\CComponentManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

///////////////////////////////////////////////////////////////////////////////
// Animated Donut Component Class
///////////////////////////////////////////////////////////////////////////////
class CAnimatedDonutComp : public IComponent
{
private:

	///////////////////////////////////////////////////////////////////////////
	// Data Members
	///////////////////////////////////////////////////////////////////////////

	CObject* m_pParentObj;				// Parent Object
	CAnimateComponent* m_pAnimComp;		// Render Comp
	CEffectComponent* m_pPuffEffect;	// Effect Comp
	float m_fDuration;					// Duration
	bool m_bActive;						// Active Flag

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
		// Purpose: This Functions is used to Handle Updating Donut Effects
		//			and Check for end of Duration to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////
		void Update(float fDT);	

		///////////////////////////////////////////////////////////////////////
		// Function: “UseDonut”
		//
		// Return: void
		//
		// Parameters: TStatusEffectEvent* ptSEEvent - Data relating to the 
		//			   Status Effect Event
		//
		// Purpose: This Functions is used to Handle activating the Donut
		//			Effects on the Player that Used it.
		///////////////////////////////////////////////////////////////////////
		void UseDonut(TStatusEffectEvent* ptSEEvent);

		///////////////////////////////////////////////////////////////////////
		// Function: “DespawnDonut”
		//
		// Return: void
		//
		// Parameters: TStatusEffectEvent* ptSEEvent - Data relating to the 
		//			   Status Effect Event
		//
		// Purpose: This Functions is used to Handle deactivating the Donut
		//			Effects on the Player that Used it.
		///////////////////////////////////////////////////////////////////////
		void DespawnDonut(TStatusEffectEvent* ptSEEvent);

	///////////////////////////////////////////////////////////////////////////
	// Helper Funcs
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “CheckForPlayerMatch”
		//
		// Return: bool - Whether the Parent Animation Object is associated
		//				  with the Passed in Object
		//
		// Parameters: CObject* pOtherObj - The Object to Check against
		//
		// Purpose: This Functions is used to Check Whether the Parent
		//			Animation Object is Associated with the Passed in Object.
		///////////////////////////////////////////////////////////////////////
		bool CheckForPlayerMatch(CObject* pOtherObj);

public:

	///////////////////////////////////////////////////////////////////////////
	// Function: “Init”
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: This Functions is used to Initalize the Component by
	//			Registering for Events, Getting and Transforming an
	//			Animated Donut Object and Creating a Puff Effect.
	///////////////////////////////////////////////////////////////////////////
	void Init(void);

	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	CAnimatedDonutComp(CObject* pParent) : m_pParentObj(pParent),
										   m_pAnimComp(NULL),
										   m_pPuffEffect(NULL),
										   m_fDuration(0.0f), m_bActive(false)
	{
		// Add to Auto Manager
		GetManager()->Add(this);

		// Add Component to Parent
		pParent->AddComponent(this);

		// Initalize
		Init();
	}

	///////////////////////////////////////////////////////////////////////////
	// Destructor
	///////////////////////////////////////////////////////////////////////////
	~CAnimatedDonutComp(void)
	{
		// Remove Component from the Manager
		GetManager()->Remove(this);
	}

	///////////////////////////////////////////////////////////////////////////
	// Factory Funcs
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “CreateAnimatedDonutComponent”
		//
		// Return: int - 0 to LUA
		//
		// Parameters: lua_State* ptLUA - LUA State of the Call
		//
		// Purpose: This Functions is used to Create an Animated Donut
		//			Component and Attach it to an Object through LUA.
		///////////////////////////////////////////////////////////////////////
		static int CreateAnimatedDonutComponent(lua_State* ptLUA);
		
		///////////////////////////////////////////////////////////////////////
		// Function: “CreateAnimatedDonutComponent”
		//
		// Return: CAnimatedDonutComp* - A New Animated Donut Component 
		//								 connected to the Passed in Object
		//
		// Parameters: CObject* pcParent - The Parent Animation Object
		//
		// Purpose: This Functions is used to Create an Animated Donut
		//			 Component and Attach it to an Object through C++.
		///////////////////////////////////////////////////////////////////////
		static CAnimatedDonutComp* CreateAnimatedDonutComponent(
			CObject* pcParent)
		{	
			return MMNEW(CAnimatedDonutComp(pcParent));
		}

	///////////////////////////////////////////////////////////////////////////////
	// Event Callbacks
	///////////////////////////////////////////////////////////////////////////////
	
		///////////////////////////////////////////////////////////////////////////
		// Function: “UpdateCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle Updating the Passed in
		//			Component's Donut Effects and Check for end of Duration
		//			to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////////
		static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////////
		// Function: “UseDonutCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle activating the Passed in
		//			Component's Donut Effects on the Player that Used it.
		///////////////////////////////////////////////////////////////////////////
		static void UseDonutCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////////
		// Function: “DespawnDonutCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent 	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle deactivating the Passed in
		//			Component's Donut Effects on the Player that Used it.
		///////////////////////////////////////////////////////////////////////////
		static void DespawnDonutCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////////
		// Function: “WinInitCallback”
		//
		// Return: void
		//
		// Parameters: IEvent*			 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Turn off the Passed in Component's
		//			Effects on Enter of Win State.
		///////////////////////////////////////////////////////////////////////////
		static void WinInitCallback(IEvent*, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////////
		// Function: “ShutdownGameplayCallback”
		//
		// Return: void
		//
		// Parameters: IEvent*			 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Unregister the Passed in Component
		//			for Events on Gameplay State Exit.
		///////////////////////////////////////////////////////////////////////////
		static void ShutdownGameplayCallback(IEvent*, IComponent* pComp);

	///////////////////////////////////////////////////////////////////////////
	// Accessors
	///////////////////////////////////////////////////////////////////////////
		
		// Parent Object
		CObject* GetParent(void) { return m_pParentObj; }

		// Active Flag
		bool IsActive(void)		 { return m_bActive;    }

		// Auto-Manager
		static CComponentManager<CAnimatedDonutComp>* GetManager(void)
		{
			// Auto-Manager
			static CComponentManager<CAnimatedDonutComp> m_cManager;

			return &m_cManager;
		}

	///////////////////////////////////////////////////////////////////////////
	// Mutators
	///////////////////////////////////////////////////////////////////////////
		
		// Active Flag
		void SetActive(bool bActive) { m_bActive = bActive; }
};

#endif