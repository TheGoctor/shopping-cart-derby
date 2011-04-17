#include "CMovement.h"

#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CInputEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CHeadingEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CWeightClassEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CMoveCameraEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CRamEvent.h"

#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"

#include "..\..\Managers\Component Managers\CMovementManager.h"

//Sound Hack//
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

#define ACCEL_SOUNDLENGTH 0.4f
#define BRAKE_SOUNDLENGTH 0.2f

CMovement::CMovement()
{
	m_pObject = NULL;
	m_bIsPlayer = false;
	m_fTurnAmount = 0.0f;
	m_fTranslateThreshold = 0.1f;
	m_bIsBackingUp = false;
	m_nPlayerNumber = 1;
	
	m_eAccelerateBehavior = COAST;

	m_fDeltaTime = 0.0f;
	m_fSoundPlayingTime = 0.0f;

	m_vVelocity = D3DXVECTOR3(0.1f, 0.0f, 0.0f);

	CEventManager::GetInstance()->RegisterEvent("Ram", this, HandleCollision);

	CalculateValuesFromWeight();
}

void CMovement::Init()
{
	// Append [ObjID] onto the name for these:
	
	/**/
}

void CMovement::Shutdown()
{

}

void CMovement::UpdatePosition(float fDt)
{
	float fSpeed =  D3DXVec3Length(&m_vVelocity);

	if(fSpeed > m_fTranslateThreshold)
	{
		D3DXVECTOR3 vTranslateVec(
			m_pObject->GetTransform()->GetLocalMatrix()._31, 
			m_pObject->GetTransform()->GetLocalMatrix()._32, 
			m_pObject->GetTransform()->GetLocalMatrix()._33); // "forward" vec
		
		D3DXVec3Normalize(&vTranslateVec, &vTranslateVec);
		if(m_bIsBackingUp)
		{
			fSpeed *= -1.0f;
		}

		// modify vel to this frame's movement distance
		D3DXVec3Scale(&vTranslateVec, &vTranslateVec, fDt * fSpeed);

		// update the object's position
		m_pObject->GetTransform()->TranslateFrame(vTranslateVec);
	}

	if(m_fTurnAmount)
	{
		D3DXVECTOR3 vAxis(0.0f, 1.0f, 0.0f);
		m_pObject->GetTransform()->RotateFrame(vAxis, m_fTurnAmount);
	}

	// if we're the player (aka player 0)
	if(m_nPlayerNumber == 0)
	{

		D3DXVECTOR3 vObjPos(
			m_pObject->GetTransform()->GetLocalMatrix()._41, 
			m_pObject->GetTransform()->GetLocalMatrix()._42, 
			m_pObject->GetTransform()->GetLocalMatrix()._43);


		// Send pertinant data to Camera through event
		// Vel, pos, turn speed
		CMoveCameraEvent* pMoveCameraEvent = (CMoveCameraEvent*)CMemoryManager::GetInstance()
			->Allocate(sizeof(CMoveCameraEvent), HEAPID_EVENT);
		(*pMoveCameraEvent) = CMoveCameraEvent(CIDGen::GetInstance()->GetID(
			"MoveCamera"), NULL, vObjPos, fSpeed, m_fTurnAmount);
		CEventManager::GetInstance()->PostEvent(pMoveCameraEvent, PRIORITY_INPUT);

	}

	m_fTurnAmount = 0.0f; // reset the turn so it only is valued on frames it turns on
}

void CMovement::UpdateVelocity(float fDt)
{
	float fSpeed = D3DXVec3Length(&m_vVelocity);
	
	// Update the heading
	m_vVelocity += m_vStoredHeading * fDt;
	D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
	m_vVelocity *= fSpeed;

	
	// Update the speed
	if(m_eAccelerateBehavior == ACCELERATE && fSpeed < m_fMaxSpeed)
	{
		if(m_bIsBackingUp)
		{
			D3DXVECTOR3 vDirection = m_vVelocity;
			D3DXVec3Normalize(&vDirection, &vDirection);
			vDirection *= m_fBrakeCoefficient * fDt;
			D3DXVECTOR3 vNewVel = m_vVelocity;//(m_vVelocity - vDirection);
			vNewVel -= vDirection;

			/// if they're facing opposite direction
			//(aka the change would reverse our direction)
			if(D3DXVec3Dot(&vNewVel, &m_vVelocity) < 0.0f)
			{
				// set the "is backing up" variable
				m_bIsBackingUp = false;
			}
			//else // it just lowers our speed, not reverses direction
			{
				m_vVelocity = vNewVel;
			}
		}
		else // !m_bIsBackingUp
		{
			D3DXVECTOR3 vDirection = m_vVelocity;
			D3DXVec3Normalize(&vDirection, &vDirection);
			m_vVelocity += vDirection * m_fAccelerateCoefficient * fDt;
		}
	}
	// Coast
	else if(m_eAccelerateBehavior != BRAKE && fSpeed > .1f)
	{
		D3DXVECTOR3 vDirection = m_vVelocity;
		D3DXVec3Normalize(&vDirection, &vDirection);
		m_vVelocity -= vDirection * m_fCoastCoefficient * fDt;
	}

	if(m_eAccelerateBehavior == BRAKE)
	{
		
		if(!m_bIsBackingUp)
		{
			D3DXVECTOR3 vDirection = m_vVelocity;
			D3DXVec3Normalize(&vDirection, &vDirection);
			vDirection *= m_fBrakeCoefficient * fDt;
			D3DXVECTOR3 vNewVel = m_vVelocity;//(m_vVelocity - vDirection);
			vNewVel -= vDirection;

			/// if they're facing opposite direction
			//(aka the change would reverse our direction)
			if(D3DXVec3Dot(&vNewVel, &m_vVelocity) < 0.0f)
			{
				// set the "is backing up" variable
				m_bIsBackingUp = true;
			}
			
			
			m_vVelocity = vNewVel;
		}
		else if(fSpeed < m_fMaxReverseSpeed) // m_bIsBackingUp
		{
			// move us backwards
			D3DXVECTOR3 vDirection = m_vVelocity;
			D3DXVec3Normalize(&vDirection, &vDirection);
			vDirection *= m_fReverseCoefficient * fDt;
			D3DXVECTOR3 vNewVel = m_vVelocity;//(m_vVelocity - vDirection);
			vNewVel += vDirection;
			
			m_vVelocity = vNewVel;
		}
	}

	// reset the value so input can catch it
	m_eAccelerateBehavior = COAST;
}

void CMovement::CalculateValuesFromWeight()
{
	m_fMaxSpeed = 10.0f;
	m_fMaxReverseSpeed = 5.0f;
	m_fAccelerateCoefficient = 4.0f;	
	m_fBrakeCoefficient = 5.0f;	
	m_fCoastCoefficient = 3.0f;	
	m_fReverseCoefficient = 1.5f;	
	m_fRammingSpeed = 7.0f;
}




///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CMovement::Update(IEvent* cEvent, IComponent* cCenter)
{
	CUpdateStateEvent* pEv = static_cast<CUpdateStateEvent*>(cEvent);
	// get dt here
	float fDt = pEv->GetDeltaTime();
	m_fDeltaTime = fDt;
	UpdateVelocity(fDt);
	UpdatePosition(fDt);
}


void CMovement::HandleInput(EAccelerateBehavior eInput)
{

	// HACK
	if(eInput == GORIGHT || eInput == GOLEFT)
	{
		float fSpeed = D3DXVec3Length(&m_vVelocity);
		D3DXVECTOR3 rightDir;
		D3DXVec3Cross(&rightDir, &D3DXVECTOR3(0.0f, -1.0f, 0.0f), &m_vVelocity);
		D3DXVec3Normalize(&rightDir, &rightDir);
	
		m_vVelocity += rightDir*m_fDeltaTime; // *# for simulated dt. larger turn rad
		D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
		m_vVelocity *= fSpeed;

		// handle rotation
		if(eInput == GORIGHT)
		{
			m_fTurnAmount += 2.0f * m_fDeltaTime;
		}
		else // == GOLEFT
		{
			m_fTurnAmount -= 2.0f * m_fDeltaTime;
		}

	}
	else // accelerate or decelerate
	{
		m_eAccelerateBehaviorLastFrame = eInput;
		m_eAccelerateBehavior = eInput;

		if(m_nPlayerNumber == 0)
		{
			// if there's a change in input (dif var 'cause m_eAccelBehav is reset in update
			if(m_eAccelerateBehaviorLastFrame != eInput)
			{
				m_fSoundPlayingTime = 100.0f; // some large number so it catches down below
			}


			m_fSoundPlayingTime += m_fDeltaTime;

			if(m_fSoundPlayingTime > ACCEL_SOUNDLENGTH && 
				m_eAccelerateBehavior == ACCELERATE)
			{
				m_fSoundPlayingTime = 0.0f;
				// PLAY ACCELERATE SOUND HERE Sound Hack
				CWwiseSoundManager::GetInstance()->PlaySound(CART_MOVEMENT, 1);
			}
			else if(m_fSoundPlayingTime > BRAKE_SOUNDLENGTH && 
				m_eAccelerateBehavior == BRAKE)
			{
				m_fSoundPlayingTime = 0.0f;
				// PLAY BRAKE SOUND HERE
				CWwiseSoundManager::GetInstance()->PlaySound(CART_BRAKE , 1);
			}

		}

	}

	/*switch(eInput)
	{
	case ACCELERATE:
		{
			m_eAccelerateBehavior = ACCELERATE;
		}
		break;
	case BRAKE:
		{
			m_eAccelerateBehavior = BRAKE;
		}
		break;
	}/**/
}

void CMovement::RegisterPlayer(IEvent* cEvent, IComponent* cCenter)
{
	m_bIsPlayer = true;
}


void CMovement::UpdateHeading(IEvent* cEvent, IComponent* cCenter)
{
	//TODO:
	CHeadingEvent* pHeadingEvent = static_cast<CHeadingEvent*>(cEvent);
	
	m_vStoredHeading = pHeadingEvent->GetHeading();
}



void CMovement::SetWeight(IEvent* cEvent, IComponent* cCenter)
{
	//TODO:
	CWeightClassEvent* pWeightClassEvent = static_cast<CWeightClassEvent*>(cEvent);

	m_eWeight = pWeightClassEvent->GetWeightClass();

	CalculateValuesFromWeight();
}


void CMovement::HandleCollision(IEvent* cEvent, IComponent* cCenter)
{
	CRamEvent* pRamEvent = (CRamEvent*)cEvent;
	CMovement* pComp = ((CMovement*)cCenter);

	if( pComp->m_pObject == pRamEvent->GetRammer() && D3DXVec3LengthSq(&pComp->m_vVelocity) > pComp->m_fRammingSpeed*pComp->m_fRammingSpeed)
	{
		// Post event: Rammed passing the players
		CRamEvent* pPlayerRammedEvent = (CRamEvent*)CMemoryManager::GetInstance()
			->Allocate(sizeof(CRamEvent), HEAPID_EVENT);
		(*pPlayerRammedEvent) = CRamEvent(CIDGen::GetInstance()->GetID(
			"PlayerRammed"), pComp, pRamEvent->GetRammer(), pRamEvent->GetWhoWasRammed());
		CEventManager::GetInstance()->PostEvent(pPlayerRammedEvent, PRIORITY_NORMAL);

	}
}


void CMovement::Boost()
{
	if(m_bBoostAvailable)
	{
		m_bBoostAvailable = false;
		m_vVelocity *= 2;
	}
}