////////////////////////////////////////////////////////////////////////////////
//	File			:	CHUDEffectsComp.h
//	Date			:	7/27/11
//	Mod. Date		:	7/27/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the HUD Item Effect
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Header Protection
///////////////////////////////////////////////////////////////////////////////
#ifndef _CHUDEFFECTSCOMP_H_
#define _CHUDEFFECTSCOMP_H_

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
#include <list>
#include "CSpriteComponent.h"
#include "..\\..\\CObject.h"
#include "..\\..\\Managers\\Global Managers\\Memory Manager\\CMemoryManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\CEventManager.h"
#include "..\\..\\Managers\\Global Managers\\Event Manager\\EventStructs.h"
using namespace EventStructs;

///////////////////////////////////////////////////////////////////////////////
// Enumerations
///////////////////////////////////////////////////////////////////////////////
enum EHUDEffectType { HUDE_MIN = -1, HUDE_GOAL_COLLECT, HUDE_PICKUP_COLLECT,
	HUDE_PICKUP_DROP, HUDE_HELD_STOLEN, HUDE_HELD_COLLECT, HUDE_HELD_USE,
	HUDE_MAX };

///////////////////////////////////////////////////////////////////////////////
// HUD Effect Structure
///////////////////////////////////////////////////////////////////////////////
struct THUDEffect
{
	///////////////////////////////////////////////////////////////////////////
	// Data Members
	///////////////////////////////////////////////////////////////////////////

	TSpriteData m_tOriginalSLIData;		   // Original SLI Sprite Data
	CSpriteComponent* m_pEffectSpriteComp; // Effect Sprite Comp
	CSpriteComponent* m_pSLISpriteComp;	   // SLI Sprtie Comp
	EHUDEffectType m_eType;				   // HUD Effect Type		   
	EGoalItemType m_eGIType;			   // SLI Goal Item Type
	float m_fDuration;					   // Duration
	float m_fAnimCounter;				   // Animation Counter
	float m_fScale;						   // Scale
	int m_nAnimFrame;					   // Animation Frames
	bool m_bGrowing;					   // Direction
	bool m_bActive;						   // Active Flag

	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	THUDEffect(void) : m_eType(HUDE_MIN), m_eGIType(NO_ITEM),
		m_pEffectSpriteComp(NULL), m_pSLISpriteComp(NULL),
		m_fDuration(0.0f), m_fAnimCounter(0.0f), m_nAnimFrame(0),
		m_bGrowing(false), m_bActive(false), m_fScale(0.0f)
	{
	}

	///////////////////////////////////////////////////////////////////////////
	// Updates
	///////////////////////////////////////////////////////////////////////////
	
		///////////////////////////////////////////////////////////////////////
		// Function: “Update”
		//
		// Return: void
		//
		// Parameters: float fDT - The time since the last Frame
		//
		// Purpose: This Functions is used to Handle Updating HUD Effects and
		//			Check for end of Duration to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////
		void Update(float fDT);

		///////////////////////////////////////////////////////////////////////
		// Function: “Update___Effect”
		//
		// Return: void
		//
		// Parameters: float fDT - The time since the last Frame
		//
		// Purpose: These Functions are used to Handle Updating the specific HUD
		//			Effect by modifing Sprite Data.
		///////////////////////////////////////////////////////////////////////
		void UpdateGICEffect(float fDT);
		void UpdatePIDEffect(float fDT);
		void UpdatePICEffect(float fDT);
		void UpdateHICEffect(float fDT);
		void UpdateHIUEffect(float fDT);

	///////////////////////////////////////////////////////////////////////////
	// Function: “Init”
	//
	// Return: void
	//
	// Parameters: EGoalItemType eGIType		 - Goal Item Type of the Icon
	//			   EHUDEffectType eHUDEType		 - The HUD Effect Type
	//			   CSpriteComponent* pSpriteComp - The HUD Icon to Use
	//
	// Purpose: This Functions is used to Initalize the HUD Effect by Creating
	//			an Sprite Object and Setting Types.
	///////////////////////////////////////////////////////////////////////////
	void Init(EGoalItemType eGIType, EHUDEffectType eHUDEType);
	void InitInv(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType );

	///////////////////////////////////////////////////////////////////////////
	// Function: “Reset”
	//
	// Return: void
	//
	// Parameters: EGoalItemType eGIType		 - Goal Item Type of the Icon
	//			   EHUDEffectType eHUDEType		 - The HUD Effect Type
	//			   CSpriteComponent* pSpriteComp - The HUD Icon to Use
	//
	// Purpose: This Functions is used to Reinitalize the HUD Effect by Setting
	//			Sprite Data and Types.
	///////////////////////////////////////////////////////////////////////////
	void Reset(EGoalItemType eGIType, EHUDEffectType eHUDEType);
	void ResetInv(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType);

	///////////////////////////////////////////////////////////////////////////
	// Function: “CellAlgo”
	//
	// Return: RECT - The frame Rectangle within the Texture
	//
	// Parameters: int nID		   - The Current Frame
	//			   int nNumCols	   - The Number of Colounms in the Sheet
	//			   int nCellWidth  - The Width of each Cell in the Sheet
	//			   int nCellHeight - The Height of each Cell in the Sheet
	//
	// Purpose: This Function is used to find the Rect within a Sprite Sheet
	//			for a certain Frame.
	///////////////////////////////////////////////////////////////////////////
	RECT CellAlgo(int nID, int nNumCols, int nCellWidth, int nCellHeight);

	///////////////////////////////////////////////////////////////////////////
	// Function: “GetInitSpriteData”
	//
	// Return: TSpriteData - Initalize Sprite Data for a HUD Effect
	//
	// Parameters: EGoalItemType eGIType		 - Goal Item Type of the Icon
	//			   EHUDEffectType eHUDEType		 - The HUD Effect Type
	//			   CSpriteComponent* pSpriteComp - The HUD Icon to Use
	//
	// Purpose: This Function is used to Obtain template Sprite Data for a HUD
	//			Effect based on a HUD Icon.
	///////////////////////////////////////////////////////////////////////////
	TSpriteData GetInitSpriteData(EGoalItemType eGIType, EHUDEffectType eHUDEType);
	TSpriteData GetInitInvSpriteData(CSpriteComponent* pSpriteComp, EHUDEffectType eHUDEType);
};

///////////////////////////////////////////////////////////////////////////////
// Type Definitions
///////////////////////////////////////////////////////////////////////////////
typedef list<THUDEffect*, scd::allocator<THUDEffect*>> HUDEffectList;

///////////////////////////////////////////////////////////////////////////////
// HUD Effects Component Class
///////////////////////////////////////////////////////////////////////////////
class CHUDEffectsComp : public IComponent
{
private:

	///////////////////////////////////////////////////////////////////////////
	// Data Members
	///////////////////////////////////////////////////////////////////////////

		CObject* m_pParentObj;			// Parent Object
		HUDEffectList m_cActiveEffects;	// Active Effects
		HUDEffectList m_cDeadEffects;	// Dead Effects

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
		// Purpose: This Function is used to Handle Updating any Active HUD
		//			Effects	and Check for Dead Effects.
		///////////////////////////////////////////////////////////////////////
		void Update(float fDT);

		///////////////////////////////////////////////////////////////////////
		// Function: “GoalItemCollected”
		//
		// Return: void
		//
		// Parameters: TGoalItemCollectedEvent* ptGICEvent - Event Data with the GIT
		//
		// Purpose: This Function is used to Handle Creating a GIC HUD Effect
		///////////////////////////////////////////////////////////////////////
		void GoalItemCollected(TGoalItemCollectedEvent* ptGICEvent);

		///////////////////////////////////////////////////////////////////////
		// Function: “PickupItemDropped”
		//
		// Return: void
		//
		// Parameters: TGoalItemCollectedEvent* ptGICEvent - Event Data with the GIT
		//
		// Purpose: This Function is used to Handle Creating a PID HUD Effect
		///////////////////////////////////////////////////////////////////////
		void PickupItemDropped(TGoalItemCollectedEvent* ptGICEvent);

		///////////////////////////////////////////////////////////////////////
		// Function: “PickupItemCollected”
		//
		// Return: void
		//
		// Parameters: TPickupItemCollectedEvent* ptPICEvent - Event Data with the GIT
		//
		// Purpose: This Function is used to Handle Creating a PID HUD Effect
		///////////////////////////////////////////////////////////////////////
		void PickupItemCollected(TPickupItemCollectedEvent* ptPICEvent);

		///////////////////////////////////////////////////////////////////////
		// Function: “HeldItemStolen”
		//
		// Return: void
		//
		// Parameters: CSpriteComponent* pSpriteComp - Event Data with Sprite
		//											   Data
		// Purpose: This Function is used to Handle Creating a HIS HUD Effect
		///////////////////////////////////////////////////////////////////////
		void HeldItemStolen(CSpriteComponent* pSpriteComp);

	///////////////////////////////////////////////////////////////////////////
	// Helper Funcs
	///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Function: “GetHUDEffect”
		//
		// Return: THUDEffect* - The Created HUD Effect
		//
		// Parameters: EGoalItemType eGIType		 - Goal Item Type of the Icon
		//			   EHUDEffectType eHUDEType		 - The HUD Effect Type
		//			   CSpriteComponent* pSpriteComp - The HUD Icon to Use
		//
		// Purpose: This Function is used to Create a Specific HUD Effect and
		//			Recycle if possible.
		///////////////////////////////////////////////////////////////////////
		THUDEffect* GetHUDEffect(EGoalItemType eGIType,
			EHUDEffectType eHUDEType);
		THUDEffect* GetInvHUDEffect(CSpriteComponent* pSpriteComp,
			EHUDEffectType eHUDEType);

		///////////////////////////////////////////////////////////////////////
		// Function: “CheckForDupe”
		//
		// Return: bool - Whether there is an Active Effect on that Icon
		//
		// Parameters: EGoalItemType eGIType - Goal Item Type of the Icon
		//
		// Purpose: This Function is used to Check for a HUD Effect on the 
		//			passed in Goal Item Icon.
		///////////////////////////////////////////////////////////////////////
		bool CheckForDupe(EGoalItemType eGIType);

public:

	///////////////////////////////////////////////////////////////////////////
	// Function: “Init”
	//
	// Return: void
	//
	// Parameters: void
	//
	// Purpose: This Functions is used to Initalize the Component by
	//			Registering for Events.
	///////////////////////////////////////////////////////////////////////////
	void Init(void);
	
	///////////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////////
	CHUDEffectsComp(CObject* pParentObj) : m_pParentObj(pParentObj)
	{
		pParentObj->AddComponent(this);
		Init();
	}

	///////////////////////////////////////////////////////////////////////////
	// Factory Funcs
	///////////////////////////////////////////////////////////////////////////
	
		///////////////////////////////////////////////////////////////////////
		// Function: “CreateHUDEffectsComp”
		//
		// Return: CHUDEffectsComp* - A New HUD Effects Component 
		//							  connected to the Passed in Object
		//
		// Parameters: CObject* pParentObj - The Parent Object
		//
		// Purpose: This Functions is used to Create a HUD Effects Component
		//			and Attach it to an Object through C++.
		///////////////////////////////////////////////////////////////////////
		static CHUDEffectsComp* CreateHUDEffectsComp(CObject* pParentObj)
		{
			return MMNEW(CHUDEffectsComp(pParentObj));
		}

	///////////////////////////////////////////////////////////////////////////
	// Event Callbacks
	///////////////////////////////////////////////////////////////////////////
	
		///////////////////////////////////////////////////////////////////////////
		// Function: “UpdateCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Functions is used to Handle Updating the Passed in
		//			Component's HUD Effects and Check for end of Duration
		//			to Switch Effects Off.
		///////////////////////////////////////////////////////////////////////////
		static void UpdateCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////////
		// Function: “GoalItemCollectedCallback”
		//
		// Return: void
		//
		// Parameters: IEvent* pEvent	 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: These Functions are used to Handle Creating Specific HUD
		//			Effects on Specifc Events.
		///////////////////////////////////////////////////////////////////////////
		static void GoalItemCollectedCallback(IEvent* pEvent, IComponent* pComp);
		static void PickupItemDroppedCallback(IEvent* pEvent, IComponent* pComp);
		static void PickupItemCollectedCallback(IEvent* pEvent, IComponent* pComp);
		static void HeldItemStolenCallback(IEvent* pEvent, IComponent* pComp);
		static void HeldItemCollectedCallback(IEvent* pEvent, IComponent* pComp);
		static void HeldItemUsedCallback(IEvent* pEvent, IComponent* pComp);

		///////////////////////////////////////////////////////////////////////////
		// Function: “ShutdownCallback”
		//
		// Return: void
		//
		// Parameters: IEvent*			 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Function is used to Unregister the Passed in Component
		//			for Events on Shutdown and Delete and HUD Effects.
		///////////////////////////////////////////////////////////////////////////
		static void ShutdownCallback(IEvent* pEvent, IComponent* pComp);
	
		///////////////////////////////////////////////////////////////////////////
		// Function: “ShutdownGameplayCallback”
		//
		// Return: void
		//
		// Parameters: IEvent*			 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Function is used to Kill any Active Effects between 
		//			playthroughs.
		///////////////////////////////////////////////////////////////////////////
		static void ShutdownGameplayCallback(IEvent* pEvent, IComponent* pComp);
	
		///////////////////////////////////////////////////////////////////////////
		// Function: “Disable/EnableCallback”
		//
		// Return: void
		//
		// Parameters: IEvent*			 - Event Data
		//			   IComponent* pComp - The Listening Component
		//
		// Purpose: This Function is used to Show and Unshow any Active Effects
		//			within and coming from the pause menu.
		///////////////////////////////////////////////////////////////////////////
		static void DisableCallback(IEvent* pEvent, IComponent* pComp);
		static void EnableCallback(IEvent* pEvent, IComponent* pComp);
};
#endif // _CHUDEFFECTSCOMP_H_