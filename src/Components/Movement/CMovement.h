///////////////////////////////////////////////////////////////////////////////
//  File			:	"CMovement.h"
//
//  Author			:	Malcolm Smith (MS)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	7/27/11
//
//  Purpose			:	Handles player movement.
///////////////////////////////////////////////////////////////////////////////

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
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			UpdateTimers
	//	Parameters:		float fDt - the delta time from last frame
	//	Return:			void	
	//	Description:	Updates timers used by this class
	///////////////////////////////////////////////////////////////////////////
	void UpdateTimers(float fDt);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			ApplyShove
	//	Parameters:		float fDt - the delta time from last frame
	//					D3DXVECTOR3 & translaveVec
	//	Return:			void	
	//	Description:	Code abstracted from update. This, using information
	//					filled out in handleshove(), moves the character
	//					when he shoves.
	///////////////////////////////////////////////////////////////////////////
	void ApplyShove(float fDt, D3DXVECTOR3& translateVec);

	///////////////////////////////////////////////////////////////////////////
	//	Name:			PlayAccelerateOrBrakeSound
	//	Parameters:		EAccelerateBehavior eInput - input type handled
	//	Return:			void	
	//	Description:	Plays the correct sound based on input type
	//					(e.g. engine sounds, brake, etc)
	///////////////////////////////////////////////////////////////////////////
	void PlayAccelerateOrBrakeSound(EAccelerateBehavior eInput);

	///////////////////////////////////////////////////////////////////////////
	//	Name:			ApplyStun
	//	Parameters:		float fDuration - input type handled
	//	Return:			void	
	//	Description:	Sets the stun timer to the value
	///////////////////////////////////////////////////////////////////////////
	void ApplyStun(float fDuration);

	///////////////////////////////////////////////////////////////////////////
	//	Name:			PlayShoveSound
	//	Parameters:		CObject* object - the object shoving
	//	Return:			void	
	//	Description:	Sees which character's sound the object should play
	//					and plays it
	///////////////////////////////////////////////////////////////////////////
	void PlayShoveSound(CObject* object);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			StoreStartMatrix
	//	Parameters:		void
	//	Return:			void	
	//	Description:	Grabs the current Cobject parent's local matrix and
	//					stores it in a member variable.
	///////////////////////////////////////////////////////////////////////////
	void StoreStartMatrix();
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			SetStartMatrix
	//	Parameters:		D3DXMATRIX mMat - the matrix to set to
	//	Return:			void	
	//	Description:	Sets the member variable start matrix to parameter
	///////////////////////////////////////////////////////////////////////////
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
	
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleCollision
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Does cart collision calculations
	///////////////////////////////////////////////////////////////////////////
	static void HandleCollision(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleShoveLeft
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Tells the component it should start shoving.
	///////////////////////////////////////////////////////////////////////////
	static void HandleShoveLeft(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleShoveRight
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Tells the component it should start shoving.
	///////////////////////////////////////////////////////////////////////////
	static void HandleShoveRight(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleInputAccelerate
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Calls HandleInput() passing accelerate.
	///////////////////////////////////////////////////////////////////////////
	static void HandleInputAccelerate(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleInputBrake
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Calls HandleInput() passing brake.
	///////////////////////////////////////////////////////////////////////////
	static void HandleInputBrake(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleInputRight
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Calls HandleInput() passing right.
	///////////////////////////////////////////////////////////////////////////
	static void HandleInputRight(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleInputLeft
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Calls HandleInput() passing left.
	///////////////////////////////////////////////////////////////////////////
	static void HandleInputLeft(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			HandleInputDrift
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Calls HandleInput() passing drift.
	///////////////////////////////////////////////////////////////////////////
	static void HandleInputDrift(IEvent* cEvent, IComponent* cCenter);
	
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			Boost
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Sets the speed for the boost and plays the sound.
	///////////////////////////////////////////////////////////////////////////
	static void Boost(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			SetWeightCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Sets the weight for this component.
	///////////////////////////////////////////////////////////////////////////
	static void SetWeightCallback(IEvent* cEvent, IComponent* cCenter);
	
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			SlipStatusCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know that slip has happened.
	///////////////////////////////////////////////////////////////////////////
	static void SlipStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			StunStatusCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know that stun has happened.
	///////////////////////////////////////////////////////////////////////////
	static void StunStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			FreezeStatusCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know that freeze has happened.
	///////////////////////////////////////////////////////////////////////////
	static void FreezeStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			SlowStatusCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know that slow has happened.
	///////////////////////////////////////////////////////////////////////////
	static void SlowStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			BlindStatusCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know that blind has happened.
	///////////////////////////////////////////////////////////////////////////
	static void BlindStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			InvulnerableStatusCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know that invulnerble has happened.
	///////////////////////////////////////////////////////////////////////////
	static void InvulnerableStatusCallback(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			StealCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Checks to see if we're invulnerable and sends a steal
	//					event to inventory.
	///////////////////////////////////////////////////////////////////////////
	static void StealCallback(IEvent* cEvent, IComponent* cCenter);
	

	///////////////////////////////////////////////////////////////////////////
	//	Name:			BounceCallbackPlayer
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Handles collision reaction bounce feel when running
	//					into another player.
	///////////////////////////////////////////////////////////////////////////
	static void BounceCallbackPlayer(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			BounceCallbackEnvironment
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Handles the collision reaction bounce feel when
	//					running into a collidable environment piece.
	///////////////////////////////////////////////////////////////////////////
	static void BounceCallbackEnvironment(IEvent* cEvent, IComponent* cCenter);
	
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			StartOfGame
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Attaches the camera to the human player and inits
	//					status effect variables.
	///////////////////////////////////////////////////////////////////////////
	static void StartOfGame(IEvent* cEvent, IComponent* cCenter);
	
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			DisableMovementInput
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Disable movement input for the player so he can't
	//					accelerate
	///////////////////////////////////////////////////////////////////////////
	static void DisableMovementInput(IEvent* cEvent, IComponent* cCenter);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			EnableMovementInput
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Enables movement input disabled by DisableMovementInput()
	///////////////////////////////////////////////////////////////////////////
	static void EnableMovementInput(IEvent* cEvent, IComponent* cCenter);

	///////////////////////////////////////////////////////////////////////////
	//	Name:			PostSpeed
	//	Parameters:		void
	//	Return:			void	
	//	Description:	Sends out the speed for anyone who wants.
	//					Sent every frame.
	///////////////////////////////////////////////////////////////////////////
	inline void PostSpeed();
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			SendMovementInformation
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	If someone requests movement information, this sends
	//					it out.
	///////////////////////////////////////////////////////////////////////////
	static void SendMovementInformation(IEvent*, IComponent*);
	
	///////////////////////////////////////////////////////////////////////////
	//	Name:			GameWonCallback
	//	Parameters:		IComponent*		cSender	//The sender of the event
	//					IEvent			cEvent	//Actually a CInputEvent
	//											//Holds actions input generated
	//	Return:			void	
	//	Description:	Lets the component know to start cutting your speed
	//					since someone has won.
	///////////////////////////////////////////////////////////////////////////
	static void GameWonCallback(IEvent*, IComponent*);
};

#endif // _CMOVEMENT_H_