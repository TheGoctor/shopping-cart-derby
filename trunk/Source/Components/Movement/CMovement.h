// file header
#ifndef _CMOVEMENT_H_
#define _CMOVEMENT_H_

#include <D3dx9math.h>
#include "..\..\IComponent.h"

#include "..\..\Managers\Component Managers\CMovementManager.h"


class CObject;
class IEvent;

class CMovement : public IComponent
{
private:
	CObject*			m_pObject;
	float				m_fSpeed;
	EAccelerateBehavior	m_eAccelerateBehavior;
	EAccelerateBehavior	m_eAccelerateBehaviorLastFrame;
	float				m_fAccelerateAmount;
	ECartWeight			m_eWeight;

	bool				m_bInputEnabled;
	
	int					m_nPlayerNumber;

	float				m_fMaxSpeed;
	float				m_fMaxReverseSpeed;
	float				m_fTurnRate;
	float				m_fOriginalTurnRate;
	float				m_fSlowMovingTurnCoefficient;
	
	float				m_fTurnAmount;
	float				m_fAccelerateCoefficient;
	float				m_fBrakeCoefficient;
	float				m_fReverseCoefficient;
	float				m_fCoastCoefficient;
	float				m_fTranslateThreshold;
	
	float				m_fRammingSpeed;
	float				m_fTimeSinceRammedLast;
	float				m_fRammedCooldown;

	float				m_fShoveTimeLeft;
	float				m_fShoveDuration;
	float				m_fShovePower;
	float				m_fShoveCooldown;
	int					m_nShoveDirection;

	D3DXMATRIX			m_mStartMatrix;

	bool				m_bIsBackingUp;

	float				m_fDeltaTime;
	float				m_fTimerCounter;
	float				m_fDtThreshold;
	float				m_fSoundPlayingTime;

	// Status effect variables
	D3DXVECTOR3			m_vSlipDirection;
	bool				m_bIsSlipping;
	float				m_fSlipTurnAmt;
	float				m_fSlipDuration;
	float				m_fSlipTurnSpeed;

	float				m_fStunDuration;
	
	float				m_fSlowDuration;
	bool				m_bIsSlowed;
	float				m_fSlowAmount;
	
	float				m_fInvulnerableDuration;

	float				m_fInvertedControlTimeLeft;

	// Drifting stuff
	D3DXVECTOR3			m_vPrevVel;
	D3DXVECTOR3			m_vCurVel;
	int					m_nDriftDirection;
	bool				m_bIsDrifting;
	int					m_nFramesDrifting;
	float				m_fTimeToDrift;

	// Bouncing off walls stuff
	D3DXVECTOR3			m_vBounceVec;
	float				m_fBounceOffTimeLeft;
	float				m_fBounceOffWallDuration;
	float				m_fBounceOffWallSpeed;

	bool				m_bEndgameSlowingDown;

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
	inline void SetObject(CObject* pObj)
	{
		m_pObject = pObj;
	}

	inline CObject* GetObject()
	{
		return m_pObject;
	}

	inline void SetPlayerNumber(int nNum)
	{
		m_nPlayerNumber = nNum;
	}
	
	inline int GetPlayerNumber()
	{
		return m_nPlayerNumber;
	}
	inline float GetPlayerSpeed()
	{
		return m_fSpeed;
	}
	inline bool GetPlayerInvulnerable()	
	{
		return m_fInvulnerableDuration > 0.0f ? true : false;
	}
	
	void UpdateTimers(float fDt);
	void ApplyShove(float fDt, D3DXVECTOR3& translateVec);
	void PlayAccelerateOrBrakeSound(EAccelerateBehavior eInput);

	void ApplyStun(float fDuration);

	void StoreStartMatrix();
	void SetStartMatrix(D3DXMATRIX mMat);

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
	static void Update(IEvent* cEvent, IComponent* cCenter);
	
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
	void HandleInput(EAccelerateBehavior eInput, float fAmount);
	
	static void HandleCollision(IEvent* cEvent, IComponent* cCenter);
	static void HandleShoveLeft(IEvent* cEvent, IComponent* cCenter);
	static void HandleShoveRight(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputAccelerate(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputBrake(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputRight(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputLeft(IEvent* cEvent, IComponent* cCenter);
	static void HandleInputDrift(IEvent* cEvent, IComponent* cCenter);
	
	static void Boost(IEvent* cEvent, IComponent* cCenter);
	static void SetWeightCallback(IEvent* cEvent, IComponent* cCenter);
	
	static void SlipStatusCallback(IEvent* cEvent, IComponent* cCenter);
	static void StunStatusCallback(IEvent* cEvent, IComponent* cCenter);
	static void FreezeStatusCallback(IEvent* cEvent, IComponent* cCenter);
	static void SlowStatusCallback(IEvent* cEvent, IComponent* cCenter);
	static void BlindStatusCallback(IEvent* cEvent, IComponent* cCenter);
	static void InvulnerableStatusCallback(IEvent* cEvent, IComponent* cCenter);
	static void LobsteredStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	static void BounceCallbackPlayer(IEvent* cEvent, IComponent* cCenter);
	static void BounceCallbackEnvironment(IEvent* cEvent, IComponent* cCenter);
	
	static void StartOfGame(IEvent* cEvent, IComponent* cCenter);
	
	static void DisableMovementInput(IEvent* cEvent, IComponent* cCenter);
	static void EnableMovementInput(IEvent* cEvent, IComponent* cCenter);

	// AI needs things
	inline void PostSpeed();
	static void SendMovementInformation(IEvent*, IComponent*);
	
	static void GameWonCallback(IEvent*, IComponent*);
};

#endif // _CMOVEMENT_H_