////////////////////////////////////////////////////////////////////////////////
//	File			:	CPeanutButterVFXComp.h
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Peanut Butter Effect
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Header Protection
///////////////////////////////////////////////////////////////////////////////
#ifndef _CPEANUTBUTTERVFXCOMP_H_
#define _CPEANUTBUTTERVFXCOMP_H_

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
#include "CSkidMarks.h"

///////////////////////////////////////////////////////////////////////////////
// Peanut Butter VFX Component Class
///////////////////////////////////////////////////////////////////////////////
class CPeanutButterVFXComp : public IComponent
{
private:

	///////////////////////////////////////////////////////////////////////////
	// Data Members
	///////////////////////////////////////////////////////////////////////////

		CObject* m_pParentObj;			   // Parent Object
		CObject* m_pLeftSkidEmitter;	   // Left Skid Emitter Object
		CObject* m_pRightSkidEmitter;	   // Right Skid Emitter Object
		CSkidMeshPair* m_pSkidMeshPair;    // Skid Mesh Pair
		CEffectComponent* m_pLeftMud;	   // Left Mud Effect
		CEffectComponent* m_pRightMud;	   // Right Mud Effect
		float m_fDuration;				   // Duration
		ECharacterSet m_eCharacterSetType; // Character Set Type

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
		// Purpose: This Functions is used to Handle Updating PB Effects
		//			and Check for end of Duration to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////
		void Update(float fDT);

		///////////////////////////////////////////////////////////////////////
		// Function: “Slow”
		//
		// Return: void
		//
		// Parameters: TStatusEffectEvent* ptSEEvent - Event Data with Duration
		//
		// Purpose: This Function is used to Handle activating the Slow
		//			Effects on the Player that hit the PB.
		///////////////////////////////////////////////////////////////////////
		void Slow(TStatusEffectEvent* ptSEEvent);

		///////////////////////////////////////////////////////////////////////
		// Function: “Invulnerable”
		//
		// Return: void
		//
		// Parameters: TStatusEffectEvent* ptSEEvent - Event Data with Duration
		//
		// Purpose: This Function is used to Handle deactivating the Slow
		//			Effects on the Player that became Invulnerability
		///////////////////////////////////////////////////////////////////////
		void Invulnerable(TStatusEffectEvent* ptSEEvent);

	///////////////////////////////////////////////////////////////////////////
	// Helper Funcs
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “FindWidth”
		//
		// Return: void
		//
		// Parameters: ECharacterSet eType - The Character Type Assoiated with
		//									 the Skid Marks
		//
		// Purpose: This Function is used Find the Appropriate Width for the PB
		//			Skide Marks for the Character Set.
		///////////////////////////////////////////////////////////////////////
		float FindWidth(ECharacterSet eType);

public:

	///////////////////////////////////////////////////////////////////////////
	// Function: “Init”
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: This Functions is used to Initalize the Component by
	//			Registering for Events, Getting and Transforming PB Emitter
	//			Objects as well as Binding them to the Parent Animation Object.
	///////////////////////////////////////////////////////////////////////////
	void Init(void);

	///////////////////////////////////////////////////////////////////////////
	// Function: “Shutdown”
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: This Functions is used to Shutdown the Component by
	//			Unregistering for Events and Destroying the Emitter Objects
	//			upon Destruction of the Parent Object.
	///////////////////////////////////////////////////////////////////////////
	void Shutdown(void);

	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	CPeanutButterVFXComp(CObject* pParent, ECharacterSet eType) : m_pParentObj(pParent),
																  m_eCharacterSetType(eType),
																  m_pLeftSkidEmitter(NULL),
																  m_pRightSkidEmitter(NULL),
																  m_pSkidMeshPair(NULL),
																  m_pLeftMud(NULL),
																  m_pRightMud(NULL),
																  m_fDuration(0.0f)
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
	~CPeanutButterVFXComp(void)
	{
		// Remove from Manager
		GetManager()->Remove(this);
	}

	///////////////////////////////////////////////////////////////////////////
	// Factory Funcs
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “CreatePBVFXComponent”
		//
		// Return: int - 0 to LUA
		//
		// Parameters: lua_State* ptLUA - LUA State of the Call
		//
		// Purpose: This Functions is used to Create an PB Effects Component
		//			and Attach it to an Object through LUA.
		///////////////////////////////////////////////////////////////////////
		static int CreatePBVFXComponent(lua_State* pLua);

		///////////////////////////////////////////////////////////////////////
		// Function: “CreatePBVFXComponent”
		//
		// Return: CPeanutButterVFXComp* - A New PB Effects Component 
		//								   connected to the Passed in Object
		//
		// Parameters: CObject* pcParent   - The Parent Animation Object
		//			   ECharacterSet eType - The Character Set Type
		//
		// Purpose: This Functions is used to Create an PB Effect Component
		//			and Attach it to an Object through C++.
		///////////////////////////////////////////////////////////////////////
		static CPeanutButterVFXComp* CreatePBVFXComponent(CObject* pParent, ECharacterSet eType)
		{
			return MMNEW(CPeanutButterVFXComp(pParent, eType));
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
		// Purpose: This Function is used to Handle Updating the Passed in
		//			Component's PB Effects and Check for end of Duration
		//			to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////////
		static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////
		// Function: “SlowCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Function is used to Handle activating the Slow
		//			Effects on the Player that hit the PB.
		///////////////////////////////////////////////////////////////////////
		static void SlowCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////
		// Function: “InvulnerableCallback”
		//
		// Return: void
		//
		// Parameters:  IEvent* pEvent	  - Event Data
		//			    IComponent* pComp - The Listening Component
		//
		// Purpose: This Function is used to Handle deactivating the Slow
		//			Effects on the Player that became Invulnerability
		///////////////////////////////////////////////////////////////////////
		static void InvulnerableCallback(IEvent* pEvent, IComponent* pComp);
		
		///////////////////////////////////////////////////////////////////////////
		// Function: “Shutdown”
		//
		// Return: void
		//
		// Parameters: void
		//
		// Purpose: This Functions is used to Shutdown the Component by
		//			Unregistering for Events and Destroying the Emitter Objects
		//			upon Destruction of the Parent Object.
		///////////////////////////////////////////////////////////////////////////
		static void DestroyObjectCallback(IEvent* pEvent, IComponent* pComp);

	///////////////////////////////////////////////////////////////////////////
	// Accessors
	///////////////////////////////////////////////////////////////////////////

		// Parent Object
		CObject* GetParent(void) { return m_pParentObj; }

		// Auto-Manager
		static CComponentManager<CPeanutButterVFXComp>* GetManager(void)
		{
			static CComponentManager<CPeanutButterVFXComp> m_cManager;

			return &m_cManager;
		}
};
#endif // _CPEANUTBUTTERVFXCOMP_H_