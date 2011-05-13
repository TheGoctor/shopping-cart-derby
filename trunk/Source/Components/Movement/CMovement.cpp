#include "CMovement.h"
#include "..\..\CObject.h"

#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Component Managers\CMovementManager.h"

//Sound Hack//
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

#define ACCEL_SOUNDLENGTH 0.4f
#define BRAKE_SOUNDLENGTH 0.45f
///Sound Hack
CWwiseSoundManager *pSound = CWwiseSoundManager::GetInstance();
bool isplaying = false;

// Frames per second to update movement simulation
#ifdef _DEBUG
#define MOVEMENT_UPDATE_FRAMERATE 30.0f
#else
#define MOVEMENT_UPDATE_FRAMERATE 60.0f
#endif

CMovement::CMovement() : m_pObject(NULL),  
	m_fTurnAmount(0.0f), m_fTranslateThreshold(0.1f), m_bIsBackingUp(false),
	m_nPlayerNumber(1), m_fRammedCooldown(1.0f), m_fTimeSinceRammedLast(0.0f),
	m_eAccelerateBehavior(COAST), m_fDeltaTime(0.0f), m_fSoundPlayingTime(0.0f),
	m_fSpeed(0.01f), m_eWeight(LWEIGHT), m_fTurnRate(2.0f), m_fSlipDuration(0.0f),
	m_vSlipDirection(0.0f, 0.0f, 0.01f), m_fSlipTurnSpeed(3.0f), m_bIsSlipping(false),
	m_bIsSlowed(false), m_fTimerCounter(0.0f), m_fDtThreshold(1/MOVEMENT_UPDATE_FRAMERATE)
{
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;

	CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);

	CEventManager::GetInstance()->RegisterEvent("CRamEvent", this, HandleCollision);
	CEventManager::GetInstance()->RegisterEvent("ShoveLeft", this, HandleShoveLeft);
	CEventManager::GetInstance()->RegisterEvent("ShoveRight", this, HandleShoveRight);

	CEventManager::GetInstance()->RegisterEvent("Accelerate", this, HandleInputAccelerate);
	CEventManager::GetInstance()->RegisterEvent("Decelerate", this, HandleInputBrake);
	CEventManager::GetInstance()->RegisterEvent("SteerLeft", this, HandleInputLeft);
	CEventManager::GetInstance()->RegisterEvent("SteerRight", this, HandleInputRight);
	CEventManager::GetInstance()->RegisterEvent("SetWeight", this, SetWeightCallback);
	
	CEventManager::GetInstance()->RegisterEvent("Slip", this, SlipStatusCallback);
	CEventManager::GetInstance()->RegisterEvent("Stun", this, StunStatusCallback);
	CEventManager::GetInstance()->RegisterEvent("Slow", this, SlowStatusCallback);
	CEventManager::GetInstance()->RegisterEvent("Invulnerable", this, InvulnerableStatusCallback);
	CEventManager::GetInstance()->RegisterEvent("Boost", this, Boost);
	
	szEventName = "InitObjects";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, StartOfGame);


	CalculateValuesFromWeight();
}

void CMovement::Init()
{
	m_mStartMatrix = m_pObject->GetTransform()->GetWorldMatrix();
}

void CMovement::CalculateValuesFromWeight()
{
	m_fMaxSpeed = (m_eWeight + 4.0f)*2.0f;
	m_fMaxReverseSpeed = m_fMaxSpeed * .5f;

	m_fAccelerateCoefficient = (10 - m_eWeight*1.5f) * .5f;	
	m_fBrakeCoefficient = m_fAccelerateCoefficient * 1.5f; // brake faster than accelerate	
	m_fCoastCoefficient = 2.0f + m_eWeight;	
	m_fReverseCoefficient = m_fAccelerateCoefficient;	

	m_fRammingSpeed = m_fMaxSpeed * ((5 - m_eWeight)/4.0f);

	m_fTurnRate = D3DXToRadian(200 - 45*m_eWeight);
	m_fSlipTurnSpeed = m_fTurnRate * 3.0f;

	m_fShoveDuration = .5f;
	m_fShoveTimeLeft = 0.0f;
	m_fShovePower = 5.0f;
	m_fShoveCooldown = 2.0f;
	m_fShoveCooldown *= -1.0f; // needed for check (since it decrements)
	m_nShoveDirection = 0;
}

void CMovement::Shutdown()
{
}

void CMovement::UpdatePosition(float fDt)
{
	// if we're fast enough to translate AND we're not stunned
	if(	fabs(m_fSpeed) > m_fTranslateThreshold &&
		m_fStunDuration <= 0.0f)
	{
		// grab our current forward vector
		D3DXVECTOR3 vTranslateVec(
			m_pObject->GetTransform()->GetLocalMatrix()._31, 
			m_pObject->GetTransform()->GetLocalMatrix()._32, 
			m_pObject->GetTransform()->GetLocalMatrix()._33);

		// if we're slipping
		if(m_fSlipDuration > 0.0f)
		{
			// don't use the model's forward, use the stored one
			vTranslateVec = m_vSlipDirection;
		}

		
		
		// normalize so we can change based on speed, etc
		D3DXVec3Normalize(&vTranslateVec, &vTranslateVec);

		// modify vel to this frame's movement distance
		D3DXVec3Scale(&vTranslateVec, &vTranslateVec, fDt * m_fSpeed);

		// do the shove stuff to the translate vector if we are shoving
		ApplyShove(fDt, vTranslateVec);

		// if we are slowed
		if(m_fSlowDuration > 0.0f)
		{
			vTranslateVec *= .5f; // halve the amount we want to move by so our speed < maxspeed/2
		}

		// update the object's position
		m_pObject->GetTransform()->TranslateFrame(vTranslateVec);
	}
	else // not fast enough to translate or we're stunned
	{
		// so we don't continue a shove when we're not moving and start again
		m_fShoveTimeLeft = 0.0f;
	}	
}

void CMovement::UpdateVelocity(float fDt)
{
	// if we're stunned
	if(m_fStunDuration > 0.0f)
	{
		// set the speed to 0
		m_fSpeed = 0.0f;
		// and get out of here
		return;
	}

	// if we're slipping
	if(m_fSlipDuration > 0.0f)
	{
		// don't modify velocity
		return;
	}


	// if we just stopped slipping (time ran out and we haven't hit this if yet)
	if(m_fSlipDuration <= 0.0f && m_bIsSlipping)
	{
		m_fShoveTimeLeft = 0.0f; // say we're not shoving anymore

		// reset orientation to saved forward (TODO: Make this do it over time so it doesn't snap)
		m_pObject->GetTransform()->GetLocalMatrix()._31 = m_vSlipDirection.x;
		m_pObject->GetTransform()->GetLocalMatrix()._32 = m_vSlipDirection.y;
		m_pObject->GetTransform()->GetLocalMatrix()._33 = m_vSlipDirection.z;

		m_bIsSlipping = false; // so we don't constantly reset the forward vector
	}

	// if we just stopped being slowed
	if(m_fSlowDuration <= 0.0f && m_bIsSlowed)
	{
		m_bIsSlowed = false;
		
		// halve the speed so it doesn't jump back up to normal speed
		m_fSpeed *= .5f;
	}
	
	// Update the speed
	if(m_eAccelerateBehavior == ACCELERATE && m_fSpeed < m_fMaxSpeed)
	{
		
		float fSpeedModification = m_fAccelerateCoefficient * fDt;
		m_fSpeed += fSpeedModification;
	}
	// Coast
	else if(m_eAccelerateBehavior != BRAKE && fabs(m_fSpeed) > m_fTranslateThreshold)
	{
		// get the opposite direction so we can coast toward zero
		float fForwardBackwardDeterminator = (m_fSpeed < 0.0f) ? 1.0f : -1.0f;
		float fSpeedModification = m_fCoastCoefficient * fDt * fForwardBackwardDeterminator;
		
		m_fSpeed += fSpeedModification;
	}

	if(m_eAccelerateBehavior == BRAKE)
	{
		// if we're going forward, brake
		if(m_fSpeed > 0.0f)
		{
			float fSpeedModification = m_fBrakeCoefficient * fDt * -1.0f;
			m_fSpeed += fSpeedModification;
		}
		// else we're going backwards. if our speed's slower than the max reverse speed
		else if(fabs(m_fSpeed) < m_fMaxReverseSpeed)
		{
			// reverse at a different rate than braking affects speed
			float fSpeedModification = m_fReverseCoefficient * fDt * -1.0f;
			m_fSpeed += fSpeedModification;
		}
	}

	// if we're turning
	if(m_fTurnAmount)
	{
		// turn by that amount
		D3DXVECTOR3 vAxis(0.0f, 1.0f, 0.0f);
		m_pObject->GetTransform()->RotateFrame(vAxis, m_fTurnAmount);
	}

	m_fTurnAmount = 0.0f; // reset the turn so it only is valued on frames it turns on

	// reset the value so input can catch it
	m_eAccelerateBehavior = COAST;
}

///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CMovement::Update(IEvent* cEvent, IComponent* cCenter)
{
	TUpdateStateEvent* pEv = static_cast<TUpdateStateEvent*>(cEvent->GetData());
	CMovement* pComp = (CMovement*)cCenter;
	// get dt here
	float fDt = pEv->m_fDeltaTime;

	// increment my delta time counter
	pComp->m_fTimerCounter += fDt;

	pComp->m_fDeltaTime = pComp->m_fDtThreshold;

	// if my delta time is greater than the framerate amount we want for a frame
	if(pComp->m_fTimerCounter >= pComp->m_fDtThreshold) // while so if we're lower than a certain amount, it'll do the number of frames needed
	{
		// subtract by the framerate so time still is accurate
		pComp->m_fTimerCounter -= pComp->m_fDtThreshold;
		
		// update stuff with the constant dt
		pComp->UpdateTimers(pComp->m_fDtThreshold);
		pComp->UpdateVelocity(pComp->m_fDtThreshold);
		pComp->UpdatePosition(pComp->m_fDtThreshold);
	}
}

void CMovement::HandleInput(EAccelerateBehavior eInput)
{
	// if we're slipping or stunned
	if(m_fSlipDuration > 0.0f || m_fStunDuration > 0.0f)
	{
		// don't handle input
		return;
	}

	// Set the turn value
	if(eInput == GORIGHT)
	{
		m_fTurnAmount += m_fTurnRate * m_fDeltaTime;
	}
	else if(eInput == GOLEFT)
	{
		m_fTurnAmount -= m_fTurnRate * m_fDeltaTime;
	}
	else // accelerate or decelerate is the only input option left we care about
	{
		m_eAccelerateBehavior = eInput;

		// if we're the human player
		if(m_nPlayerNumber == 0)
		{
			// do sound logic for this input
			PlayAccelerateOrBrakeSound(eInput);
		}

		// record input from this frame
		m_eAccelerateBehaviorLastFrame = eInput; // store this value in something that isn't reset in update position
	}
}

void CMovement::HandleCollision(IEvent* cEvent, IComponent* cCenter)
{
	TRamEvent* pRamEvent = (TRamEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if( pComp->m_pObject == pRamEvent->m_pcRammer && pComp->m_fTimeSinceRammedLast <= 0.0f &&
		pComp->m_fSpeed > pComp->m_fRammingSpeed)
	{
		// Post event: Rammed passing the players
		SendRamEvent("PlayerRammed", pComp, pRamEvent->m_pcRammer, pRamEvent->m_pcRammee);

		pComp->m_fTimeSinceRammedLast = pComp->m_fRammedCooldown;
	}
}

void CMovement::HandleShoveLeft(IEvent* cEvent, IComponent* cCenter)
{
	//TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;
	
	// if we're not shoving
	if(pComp->m_fShoveTimeLeft <= pComp->m_fShoveCooldown && fabs(pComp->m_fSpeed) > pComp->m_fTranslateThreshold)
	{
		pComp->m_fShoveTimeLeft = pComp->m_fShoveDuration;
		pComp->m_nShoveDirection = -1;
		///sound hack
		if (pComp->m_nPlayerNumber == 0)
		{
			pSound->PlayTheSound(BULLDOG_SHOVE, GLOBAL_ID);
		}
		// TODO: Send effect event now
	}
}

void CMovement::HandleShoveRight(IEvent* cEvent, IComponent* cCenter)
{
	//TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;
	
	// if we're not shoving
	if(pComp->m_fShoveTimeLeft <= pComp->m_fShoveCooldown && fabs(pComp->m_fSpeed) > pComp->m_fTranslateThreshold)
	{
		pComp->m_fShoveTimeLeft = pComp->m_fShoveDuration;
		pComp->m_nShoveDirection = 1;
		// sound hack
		if (pComp->m_nPlayerNumber == 0)
		{
			pSound->PlayTheSound(BULLDOG_SHOVE, GLOBAL_ID);
		}
		// TODO: Send effect event now
	}
}

void CMovement::Boost(IEvent* cEvent, IComponent* cCenter)
{
	// TODO: Change this to receive boost event rather than handling space bar
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_nPlayer == pComp->m_nPlayerNumber &&
		pComp->m_bBoostAvailable)
	{
		pComp->m_bBoostAvailable = false;
		pComp->m_fSpeed *= 2.0f;
		//sound hack
		if (pComp->m_nPlayerNumber == 0)
		{
			pSound->PlayTheSound(BULLDOG_CART_BOOST, GLOBAL_ID);
		}
		// HACK: Using boost to test status effects
		//SendStatusEffectEvent("Slow", pComp, pComp->m_pObject, 3.0f, PRIORITY_NORMAL);
	}
}


void CMovement::HandleInputAccelerate(IEvent* cEvent, IComponent* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;
	
	if(pEvent->m_nPlayer == pComp->m_nPlayerNumber)
	{
		pComp->HandleInput(ACCELERATE);
	}

}

void CMovement::HandleInputBrake(IEvent* cEvent, IComponent* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;
		
	if(pEvent->m_nPlayer == pComp->m_nPlayerNumber)
	{
		pComp->HandleInput(BRAKE);
	}
}

void CMovement::HandleInputRight(IEvent* cEvent, IComponent* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_nPlayer == pComp->m_nPlayerNumber)
	{
		pComp->HandleInput(GORIGHT);
	}

}

void CMovement::HandleInputLeft(IEvent* cEvent, IComponent* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_nPlayer == pComp->m_nPlayerNumber)
	{
		pComp->HandleInput(GOLEFT);
	}
}

void CMovement::SetWeightCallback(IEvent* cEvent, IComponent* cCenter)
{
	TWeightClassEvent* pEvent = (TWeightClassEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject)
	{
		pComp->m_eWeight = pEvent->m_eWeightClass;
		pComp->CalculateValuesFromWeight();
	}

}

void CMovement::SlipStatusCallback(IEvent* cEvent, IComponent* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// we're the one's slipped. Init slip data
		pComp->m_fSlipDuration = pEvent->m_fDuration == 0.0f ? 3.0f : pEvent->m_fDuration; // if duration is 0, use default of 3
		
		// set the slip direction to current heading so we can turn them while keeping the same velocity direction
		pComp->m_vSlipDirection.x = pComp->m_pObject->GetTransform()->GetWorldMatrix()._31;
		pComp->m_vSlipDirection.y = pComp->m_pObject->GetTransform()->GetWorldMatrix()._32;
		pComp->m_vSlipDirection.z = pComp->m_pObject->GetTransform()->GetWorldMatrix()._33;

		pComp->m_bIsSlipping = true;
		//sound hack
	pSound->PlayTheSound(BULLDOG_BIKE_SLIP, GLOBAL_ID);
		// TODO: Send Effect stuff here
	}

}

void CMovement::StunStatusCallback(IEvent* cEvent, IComponent* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// we're the one's stunned. Init stun data
		pComp->m_fStunDuration = pEvent->m_fDuration == 0.0f ? 3.0f : pEvent->m_fDuration; // if duration is 0, use default of 3

		// TODO: Send Effect stuff here
	}
}

void CMovement::SlowStatusCallback(IEvent* cEvent, IComponent* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// we're the one's slowed. Init slow data
		pComp->m_fSlowDuration = pEvent->m_fDuration == 0.0f ? 3.0f : pEvent->m_fDuration; // if duration is 0, use default of 3
		pComp->m_bIsSlowed = true;

		// TODO: Send Effect stuff here
	}
}

void CMovement::BlindStatusCallback(IEvent* cEvent, IComponent* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// if we're the human
		if(pComp->m_nPlayerNumber == 0)
		{
			// spawn the cream pie effect on the screen
		}
		// else we're the AI
		else
		{
			// send event to tell AI to shorten their feelers
		}
	}
}

void CMovement::InvulnerableStatusCallback(IEvent* cEvent, IComponent* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject)
	{
		// we're the one's invulnerable. Init invulnerable data
		pComp->m_fInvulnerableDuration = pEvent->m_fDuration == 0.0f ? 5.0f : pEvent->m_fDuration; // if duration is 0, use default of 3

		// TODO: Send Effect stuff here
	}
}

void CMovement::UpdateTimers(float fDt)
{
	// decrement status timers
	m_fStunDuration -= fDt;
	m_fSlipDuration -= fDt;
	m_fSlowDuration -= fDt;
	m_fShoveTimeLeft -= fDt;
	m_fInvulnerableDuration -= fDt;
	m_fTimeSinceRammedLast -= fDt;

}

void CMovement::ApplyShove(float fDt, D3DXVECTOR3& vTranslateVec)
{
	// Apply shove
	// if we're shoving and not slipping
	if(m_fShoveTimeLeft > 0.0f)
	{
		D3DXVECTOR3 rightDir;
		D3DXVECTOR3 upDir(0.0f, 1.0f, 0.0f);
		D3DXVec3Cross(&rightDir, &upDir, &vTranslateVec);
		D3DXVec3Normalize(&rightDir, &rightDir);

		float fCoeff = m_fShovePower*fDt*m_nShoveDirection;

		// This check is to allow shoving while moving backwards
		// the way translate works, this is needed so it shoves the correct direction
		if(m_fSpeed < 0.0f) 
		{
			fCoeff *= -1.0f;
		}

		D3DXVec3Scale(&rightDir, &rightDir, fCoeff);

		vTranslateVec += rightDir;

		// so we keep moving in our shove if we slip during it
		if(m_fSlipDuration > 0.0f)
		{
			m_fShoveTimeLeft += fDt;
		}
	}
}


void CMovement::PlayAccelerateOrBrakeSound(EAccelerateBehavior eInput)
{
	// if there's a change in input (dif var 'cause m_eAccelBehav is reset in update
	if(m_eAccelerateBehaviorLastFrame != eInput)
	{
		m_fSoundPlayingTime = 100.0f; // some large number so it catches down below
	}

	m_fSoundPlayingTime += m_fDeltaTime;

	if (!isplaying && m_eAccelerateBehavior == ACCELERATE)
	{
		pSound->PlayTheSound(BULLDOG_SPEED_PLAY, GLOBAL_ID);
		isplaying = true;
	}

	if(m_fSoundPlayingTime > BRAKE_SOUNDLENGTH && 
		m_eAccelerateBehavior == BRAKE)
	{
		m_fSoundPlayingTime = 0.0f;
		// PLAY BRAKE SOUND HERE Sound Hack
		pSound->PlayTheSound(BULLDOG_SPEED_STOP, GLOBAL_ID);
		pSound->PlayTheSound(BULLDOG_CART_BRAKE, GLOBAL_ID);
		isplaying = false;
	}
	if (!isplaying && m_eAccelerateBehavior == COAST)
	{
		pSound->PlayTheSound(BULLDOG_SPEED_PLAY, GLOBAL_ID);

	}
}

void CMovement::StartOfGame(IEvent* cEvent, IComponent* cCenter)
{
	CMovement* pComp = (CMovement*)cCenter;

	pComp->m_pObject->GetTransform()->GetLocalMatrix() = pComp->m_mStartMatrix;

	pComp->m_fSpeed = 0.0f;
}