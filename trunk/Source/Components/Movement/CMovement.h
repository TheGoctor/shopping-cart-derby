// file header
#ifndef _CMOVEMENT_H_
#define _CMOVEMENT_H_

// TODO: change vec3 to D3DXVECTOR3
#include <D3dx9math.h>
#include "..\..\IComponent.h"

#include "..\..\Managers\Component Managers\CMovementManager.h"
#include "..\..\Managers\Component Managers\CSteeringManager.h"

class CObject;
class IEvent;

class CMovement : public IComponent
{
private:
	CObject*			m_pObject;
	D3DXVECTOR3			m_vVelocity;
	EAccelerateBehavior	m_eAccelerateBehavior;
	EAccelerateBehavior	m_eAccelerateBehaviorLastFrame;
	bool				m_bIsPlayer;
	ECartWeight			m_eWeight;
	CMovementManager*	m_pcMovementManager;

	int					m_nPlayerNumber;

	float				m_fMaxSpeed;
	float				m_fMaxReverseSpeed;
	D3DXVECTOR3			m_vStoredHeading; // heading sent from steering

	float				m_fTurnAmount;
	float				m_fAccelerateCoefficient;
	float				m_fBrakeCoefficient;
	float				m_fReverseCoefficient;
	float				m_fCoastCoefficient;
	float				m_fTranslateThreshold;
	float				m_fRammingSpeed;

	bool				m_bIsBackingUp;

	float				m_fDeltaTime;
	float				m_fSoundPlayingTime;

	// HACK: until inventory handles it
	bool				m_bBoostAvailable;


public:

	///////////////////////////////////////////////////////////////////////////
	//	Name:			CMovement
	//	Parameters:		None
	//	Return:			Void
	//	Description:	Subscribes itself to the proper events
	///////////////////////////////////////////////////////////////////////////
	CMovement();
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			Init
	//	Parameters:		None
	//	Return:			Void
	//	Description:	Initializes all the values in the class. 
	//					 This must be called before using this class’s 
	//					 functionality.
	//					 Requests the weight and isPlayer from
	//					 an associated steering component.

	///////////////////////////////////////////////////////////////////////////
	void Init();
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			Shutdown
	//	Parameters:		None
	//	Return:			Void
	//	Description:	Deallocates any memory allocated 
	//					 by this class and releases any data currently held. 
	//					 Call this at the end of the life of the agent. 
	///////////////////////////////////////////////////////////////////////////
	void Shutdown();
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			UpdatePosition
	//	Parameters:		float		fDt	// Delta time from last frame
	//	Return:			void
	//	Description:	Updates the position based on velocity. 
	//					 This is called from the Update() callback function.
	///////////////////////////////////////////////////////////////////////////
	void UpdatePosition(float fDt);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			UpdateVelocity
	//	Parameters:		float		fDt	// Delta time from last frame
	//	Return:			void
	//	Description:	Calculates and updates the object’s velocity.
	//					 This is called from the Update() callback function.
	///////////////////////////////////////////////////////////////////////////
	void UpdateVelocity(float fDt);

	///////////////////////////////////////////////////////////////////////////
	//	Name:			CalculateCoefficientsFromWeight
	//	Parameters:		None
	//	Return:			void	
	//	Description:	Calculates the movement coefficient values
	//					 from the weight enum value.
	///////////////////////////////////////////////////////////////////////////
	void CalculateValuesFromWeight();


	///////////////////////////////////////////////////////////////////////////
	//	Name:			SetObject
	//	Parameters:		CObject*	pObj	// The object this component
	//											belongs to
	//	Return:			void	
	//	Description:	Sets the parent object member to the parameter passed in
	///////////////////////////////////////////////////////////////////////////
	void SetObject(CObject* pObj)
	{
		m_pObject = pObj;
	}

	void SetPlayerNumber(int nNum)
	{
		m_nPlayerNumber = nNum;
	}
	
	int GetPlayerNumber()
	{
		return m_nPlayerNumber;
	}



	
	///////////////////////////
	//
	//	Callback Functions
	//
	///////////////////////////
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			Update
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CUpdateEvent
	//											//Holds the frame's delta time
	//	Return:			void
	//	Description:	Calls all of the functions that need to 
	//					 be called every frame. This function is 
	//					 called by the event system. 
	//					 Additionally, Update will check to see 
	//					 if each object is registered as a player
	//					 and if so, sends an event to the HUD with the object.
	///////////////////////////////////////////////////////////////////////////
	void Update(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleInput
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Sets braking/accelerating flags inside 
	//					 the component. This event will be 
	//					 processed before Update()’s event so the 
	//					 flags will be properly set by the time Update() is called.
	///////////////////////////////////////////////////////////////////////////
	void HandleInput(EAccelerateBehavior eInput);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			RegisterPlayer
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Empty event
	//	Return:			void		
	//	Description:	Makes the m_bIsPlayer bool true which 
	//					 enables broadcasting the object’s 
	//					 location to the HUD.
	///////////////////////////////////////////////////////////////////////////
	void RegisterPlayer(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			UpdateHeading
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CHeadingEvent
	//											// Holds a normalized vector
	//	Return:			void	
	//	Description:	Updates the direction of the velocity 
	//					 while maintaining its magnitude.
	///////////////////////////////////////////////////////////////////////////
	void UpdateHeading(IEvent* cEvent, IComponent* cCenter);

	///////////////////////////////////////////////////////////////////////////
	//	Name:			SetWeight
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CWeightClassEvent
	//											// Holds a normalized vector
	//	Return:			void	
	//	Description:	Sets the weight value in the event and updates values
	//					 to match the weight
	///////////////////////////////////////////////////////////////////////////
	void SetWeight(IEvent* cEvent, IComponent* cCenter);
	
	
	static void HandleCollision(IEvent* cEvent, IComponent* cCenter);
	
	void Boost();




};


#endif // _CMOVEMENT_H_