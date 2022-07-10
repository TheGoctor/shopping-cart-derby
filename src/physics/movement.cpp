///////////////////////////////////////////////////////////////////////////////
//  File			:	"CMovement.cpp"
//
//  Author			:	Malcolm Smith (MS)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	7/27/11
//
//  Purpose			:	Handles player movement.
///////////////////////////////////////////////////////////////////////////////


#include "CMovement.h"
#include "scd::object.h"

#include "Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "Managers\Component Managers\CMovementManager.h"

//Sound Hack//
#include "Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CHUDManager.h"


#define REFLECT_DOT_MIN .7f

// Minimum angle dot value for ramming someone
#define RAM_HEADING_DIFFERENCE .8f

#define ACCEL_SOUNDLENGTH 0.4f
#define BRAKE_SOUNDLENGTH 5.0f

///Sound
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
m_nPlayerNumber(1), m_fRammedCooldown(3.0f), m_fTimeSinceRammedLast(0.0f),
m_eAccelerateBehavior(COAST), m_fDeltaTime(0.0f), m_fSoundPlayingTime(0.0f),
m_fSpeed(0.01f), m_eWeight(MWEIGHT), m_fTurnRate(2.0f), m_fSlipDuration(0.0f),
m_vSlipDirection(0.0f, 0.0f, 0.01f), m_fSlipTurnSpeed(3.0f), m_bIsSlipping(false),
m_bIsSlowed(false), m_fTimerCounter(0.0f), m_fDtThreshold(1/MOVEMENT_UPDATE_FRAMERATE),
m_bIsDrifting(false), m_fBounceOffTimeLeft(0.0f), m_fTimeToDrift(0.0f), m_fInvertedControlTimeLeft(0.0f),
m_nFramesDrifting(0), m_vPrevVel(1.0f, 0.0f, 0.0f), m_bEndgameSlowingDown(false), m_fOriginalTurnRate(0.1f)
{
	CEventManager* pEM = CEventManager::GetInstance();


	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	pEM->RegisterEvent(szEventName, this, Update);

	pEM->RegisterEvent("CartRam", this, HandleCollision);
	pEM->RegisterEvent("ShoveRight", this, HandleShoveLeft); // HACK? reverse input for left and right since the world is inverted
	pEM->RegisterEvent("ShoveLeft", this, HandleShoveRight);

	pEM->RegisterEvent("Accelerate", this, HandleInputAccelerate);
	pEM->RegisterEvent("Decelerate", this, HandleInputBrake);
	pEM->RegisterEvent("SteerRight", this, HandleInputLeft); // HACK: Swapped l/r because the world is  backwards
	pEM->RegisterEvent("SteerLeft", this, HandleInputRight);
	pEM->RegisterEvent("Drift", this, HandleInputDrift);
	pEM->RegisterEvent("SetWeight", this, SetWeightCallback);

	pEM->RegisterEvent("Slip", this, SlipStatusCallback);
	pEM->RegisterEvent("Stun", this, StunStatusCallback);
	pEM->RegisterEvent("Freeze", this, FreezeStatusCallback);
	pEM->RegisterEvent("Slow", this, SlowStatusCallback);
	pEM->RegisterEvent("Invulnerable", this, InvulnerableStatusCallback);
	pEM->RegisterEvent("Boost", this, Boost);
	pEM->RegisterEvent("Blind", this, BlindStatusCallback);
	pEM->RegisterEvent("Steal", this, StealCallback);

	pEM->RegisterEvent("CartCollision", this, BounceCallbackPlayer);
	pEM->RegisterEvent("EnvironmentHit", this, BounceCallbackEnvironment);

	pEM->RegisterEvent("DisableMovement", this, DisableMovementInput);
	pEM->RegisterEvent("EnableMovement", this, EnableMovementInput);

	szEventName = "InitObjects";
	szEventName += GAMEPLAY_STATE;
	event_manager.register_event(szEventName, this, StartOfGame);

	event_manager.register_event("WonGame", this, GameWonCallback);


	// Needed by AI
	pEM->RegisterEvent("AIInited", this, SendMovementInformation);

	CalculateValuesFromWeight();
}

void CMovement::StoreStartMatrix()
{
	m_mStartMatrix = m_pObject->GetTransform()->GetWorldMatrix();
}


void CMovement::SetStartMatrix(scd::transform mMat)
{
	m_mStartMatrix = mMat;
}

void CMovement::Init()
{
	m_mStartMatrix = m_pObject->GetTransform()->GetWorldMatrix();
}

void CMovement::CalculateValuesFromWeight()
{
		m_fMaxSpeed = (m_eWeight + 6.0f)*2.0f;
		m_fMaxReverseSpeed = m_fMaxSpeed * .5f;

		m_fAccelerateCoefficient = (13 - m_eWeight*1.0f) * .5f;
		m_fBrakeCoefficient = m_fAccelerateCoefficient * 2.0f; // brake faster than accelerate
		m_fCoastCoefficient = 2.0f + m_eWeight;
		m_fReverseCoefficient = m_fAccelerateCoefficient;

		m_fRammingSpeed = m_fMaxSpeed * .7f;

		m_fTurnRate = D3DXToRadian(150 - 30*m_eWeight);
		m_fSlipTurnSpeed = m_fTurnRate * 3.0f;
		m_fSlowMovingTurnCoefficient = m_eWeight * .3f;

		m_fShoveDuration = .5f;
		m_fShoveTimeLeft = 0.0f;
		m_fShovePower = 7.0f-(m_eWeight*.5f);
		m_fShoveCooldown = 2.0f;
		m_fShoveCooldown *= -1.0f; // needed for check in handle shove (since it decrements)
		//(checks if it's less than the cooldown, which since the
		//time starts at, say .5f for the duration, and decrements,
		//if the cd is negative, we can see if, say, timeleft < -4.0f)
		m_nShoveDirection = 0;

		m_fBounceOffWallDuration = 0.1f;
		m_fBounceOffWallSpeed = 10.5f;

		m_fOriginalTurnRate = m_fTurnRate;
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
		scd::vector3 vTranslateVec(
			m_pObject->GetTransform()->GetLocalMatrix()._31,
			m_pObject->GetTransform()->GetLocalMatrix()._32,
			m_pObject->GetTransform()->GetLocalMatrix()._33);

		// if we're bouncing back off a wall
		if(m_fBounceOffTimeLeft > 0.0f)
		{
			vTranslateVec = m_vBounceVec;
		}
		// TODO: Put an else on this when bouncing is back in
		// if we're slipping
		else if(m_fSlipDuration > 0.0f)
		{
			// don't use the model's forward, use the stored one
			vTranslateVec = m_vSlipDirection;
		}
		// else if we're drifting
		else if(m_nFramesDrifting > 0 && m_fTurnAmount != 0.0f)
		{
			// calc right vector
			scd::vector3 vRightDir;
			D3DXVec3Cross(&vRightDir, &scd::vector3(0.0f ,1.0f, 0.0f), &m_vCurVel);
			D3DXVec3Normalize(&vRightDir, &vRightDir);

			if(m_fTurnAmount > 0.0f)
			{
				vRightDir *= -1; // make it face the correct drift direction
			}

			scd::vector3 vOutput = (1.0f*vRightDir + .8f*m_vPrevVel + m_vCurVel);
			//D3DXVec3Normalize(&vOutput, &vOutput);

			// store previous vel
			//m_vPrevVel = m_vCurVel;
			// update cur vel
			m_vCurVel = vOutput;

			vTranslateVec += m_vCurVel;/**/
			//D3DXVec3Normalize(&vTranslateVec, &vTranslateVec);
		}



		// normalize so we can change based on speed, etc
		D3DXVec3Normalize(&vTranslateVec, &vTranslateVec);

		// modify vel to this frame's movement distance
		D3DXVec3Scale(&vTranslateVec, &vTranslateVec, fDt * m_fSpeed);

		// if we're not bouncing off a wall (bounce time is less than zero)
		if(m_fBounceOffTimeLeft <= 0.0f)
		{
			// do the shove stuff to the translate vector if we are shoving (incorporates dt into its calculation)
			ApplyShove(fDt, vTranslateVec);
		}

		// if we are slowed
		if(m_fSlowDuration > 0.0f)
		{
			vTranslateVec *= m_fSlowAmount; // halve the amount we want to move by so our speed < maxspeed/2
		}

		// update the object's position
		m_pObject->GetTransform()->TranslateFrame(vTranslateVec);
	}
	else // not fast enough to translate or we're stunned
	{
		// so we don't continue a shove when we're not moving and start again
		m_fShoveTimeLeft = 0.0f;
	}

	// reset the drifting variable
	m_bIsDrifting = false;


	// Clamp us to the ground
	m_pObject->GetTransform()->GetLocalMatrix()._42 = 0.0f;

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

	// if it's the end of game, we wanna slow us down quickly so we don't run into the wall
	if(m_bEndgameSlowingDown)
	{
		m_fSpeed *= .94f;
	}

	// if we're slipping
	if(m_fSlipDuration > 0.0f)
	{
		// don't modify velocity
		m_fSlipTurnAmt += ((rand()%100)*.01f - .5f)*2.0f;
		if(fabs(m_fSlipTurnAmt) > 5.0f)
		{
			m_fSlipTurnAmt = 5.0f;
		}

		m_fTurnAmount += fDt * m_fTurnRate * m_fSlipTurnAmt;
		m_eAccelerateBehavior = ACCELERATE;
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
		m_fSpeed *= m_fSlowAmount;
	}

	// Update the speed
	// If we're drifting
	if(m_fTimeToDrift > 0.0f)
	{
		// and we're moving forward
		if(m_fSpeed > 0.0f)
		{
			// lower the speed by the average of the brake and coast coeff
			m_fSpeed -= (m_fBrakeCoefficient) * .3f * fDt; // scale the brake coeff by a constant
		}
	}
	else if(m_eAccelerateBehavior == ACCELERATE && m_fSpeed < m_fMaxSpeed)
	{

		float fSpeedModification = m_fAccelerateAmount * m_fAccelerateCoefficient * fDt;
		m_fSpeed += fSpeedModification;
	}
	// Coast
	else if(m_eAccelerateBehavior != BRAKE && fabs(m_fSpeed) > m_fTranslateThreshold)
	{
		// get the opposite direction so we can coast toward zero
		float fForwardBackwardDeterminator = (m_fSpeed < 0.0f) ? 1.0f : -1.0f;
		float fSpeedModification = m_fCoastCoefficient * fDt * fForwardBackwardDeterminator;

		m_fSpeed += fSpeedModification;

		// if we're not holding accel (since if we are and we're max it still hits the elseif)
		if(m_eAccelerateBehavior != ACCELERATE)
		{
			SendInputEvent("DecelerateEffect", this, this->m_pObject, m_fSpeed);
		}
	}

	if(m_eAccelerateBehavior == BRAKE)
	{
		// if we're going forward, brake
		if(m_fSpeed > 0.0f)
		{
			float fSpeedModification = m_fAccelerateAmount * m_fBrakeCoefficient * fDt * -1.0f;
			m_fSpeed += fSpeedModification;
		}
		// else we're going backwards. if our speed's slower than the max reverse speed
		else if(fabs(m_fSpeed) < m_fMaxReverseSpeed)
		{
			SendInputEvent("Reverse", this, m_pObject, 1.0f);
			// reverse at a different rate than braking affects speed
			float fSpeedModification = m_fAccelerateAmount * m_fReverseCoefficient * fDt * -1.0f;
			m_fSpeed += fSpeedModification;
		}
	}

	PostSpeed();

	// Modify our turn amount if we're drifting
	if(m_fTimeToDrift <= 0.0f)
	{
		m_fTurnRate = m_fOriginalTurnRate;
	}
	else // we're drifting
	{
		if(m_fTurnRate < m_fOriginalTurnRate + 3.0f)
		{
			m_fTurnRate +=  5.0f * fDt; // TODO: Make this number a variable (affected by weight?)
		}
	}


	// if we're turning and not drifting
	if(m_fTurnAmount)
	{
		scd::vector3 vFwd;
		vFwd.x = m_pObject->GetTransform()->GetLocalMatrix()._31;
		vFwd.y = m_pObject->GetTransform()->GetLocalMatrix()._32;
		vFwd.z = m_pObject->GetTransform()->GetLocalMatrix()._33;

		// translate backwards first

		// HEY MAC THIS IS WHAT YOU CHANGE FOR THE OFFSET FOR ROTATION CENTER!!!!!!!////////////////////////////////////////////////
		float fTranslateDistance = .5f;
		m_pObject->GetTransform()->TranslateFrame(vFwd * fTranslateDistance);

		// do the rotation
		scd::vector3 vAxis(0.0f, 1.0f, 0.0f);
		m_pObject->GetTransform()->RotateFrame(vAxis, m_fTurnAmount);

		// calculate new forward vec post rotate
		vFwd.x = m_pObject->GetTransform()->GetLocalMatrix()._31;
		vFwd.y = m_pObject->GetTransform()->GetLocalMatrix()._32;
		vFwd.z = m_pObject->GetTransform()->GetLocalMatrix()._33;

		// then translate back forward
		m_pObject->GetTransform()->TranslateFrame(vFwd * -fTranslateDistance);

		// Update curvel for drifting calculations
		m_vCurVel.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
		m_vCurVel.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
		m_vCurVel.z = m_pObject->GetTransform()->GetWorldMatrix()._33;

		// if we're slipping
		if(m_fSlipDuration > 0.0f)
		{
			m_vSlipDirection = m_vCurVel;
		}

	}

	float newVal = (((m_fSpeed) * (70.0f))/ (m_fMaxSpeed));


	pSound->SetObjectPosition(CHUDManager::GetInstance()->GetPlayerCharID(CHUDManager::GetInstance()->GetPlayerNum(m_pObject)),
		m_pObject->GetTransform()->GetWorldPosition(), 0.25f);
	pSound->SetRPMValueForSound(newVal,
		CHUDManager::GetInstance()->GetPlayerCharID(CHUDManager::GetInstance()->GetPlayerNum(m_pObject)));

	if(CHUDManager::GetInstance()->GetPlayerNum(m_pObject) == 0)
	{
		scd::vector3 posit;
		posit = m_pObject->GetTransform()->GetWorldPosition();
		posit.z = m_pObject->GetTransform()->GetWorldPosition().z;
		scd::vector3 forwardvec;
		forwardvec.x = m_pObject->GetTransform()->GetLocalMatrix()._31;
		forwardvec.y = m_pObject->GetTransform()->GetLocalMatrix()._32;
		forwardvec.z = m_pObject->GetTransform()->GetLocalMatrix()._33;
		pSound->SetListenerPosition(posit, forwardvec, 1.0f, 0);

	}

	if(m_bIsSlowed)
	{
		SendInputEvent("SlowAnimation", this, m_pObject, 1.0f);
	}
	// else if we're boosting (aka going more than max speed and we're not in a peanut butter)
	else if(m_fSpeed > m_fMaxSpeed * 1.1f) // since speed slightly goes above max speed occasionally, raise the max speed value tested against)
	{
		SendInputEvent("BoostAnimation", this, m_pObject, 1.0f);
	}
}

///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CMovement::Update(IEvent* cEvent, scd::base_component* cCenter)
{
	TUpdateStateEvent* pEv = static_cast<TUpdateStateEvent*>(cEvent->GetData());
	CMovement* pComp = (CMovement*)cCenter;
	// get dt here
	float fDt = pEv->m_fDeltaTime;

	// increment my delta time counter
	pComp->m_fTimerCounter += fDt;

	// simulate average of last 120 frames
	pComp->m_fDtThreshold *= 29.0f;
	pComp->m_fDtThreshold += fDt;
	pComp->m_fDtThreshold /= 30.0f;

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

	pComp->m_nFramesDrifting--;

	pComp->m_fTurnAmount = 0.0f; // reset the turn so it only is valued on frames it turns on

	// reset the value so input can catch it
	pComp->m_eAccelerateBehavior = COAST;

	// if we're done drifting
	if(pComp->m_nFramesDrifting < 0)
	{
		// reset our drift dir sow e can drift
		pComp->m_nDriftDirection = ACC_ERR;
	}

	// output our position to the console window
	/*cout << pComp->m_pObject->GetTransform()->GetWorldPosition().x << "x " <<
	pComp->m_pObject->GetTransform()->GetWorldPosition().y << "y " <<
	pComp->m_pObject->GetTransform()->GetWorldPosition().z << "z " << "\n";/**/
}

void CMovement::HandleInput(EAccelerateBehavior eInput, float fAmount = 1.0f)
{
	// if we're slipping or stunned or input isn't enabled
	if(m_fSlipDuration > 0.0f || m_fStunDuration > 0.0f || !m_bInputEnabled)
	{
		// don't handle input
		return;
	}

	// Set the turn value
	if(eInput == GORIGHT)
	{
		SendInputEvent("SteerLeftAnim", this, m_pObject, 1.0f);
		// if we're drifting the other direction
		if(m_nDriftDirection == GOLEFT)
		{
			// don't turn
			return;
		}

		// get our speed turn modifier
		float percentOfMaxSpeed = m_fSpeed / m_fMaxSpeed;
		// invert it
		float invertedPercentMaxSpeed = 1.0f - percentOfMaxSpeed;
		// scale it by how much we want to increase our turn speed when we're going slower
		float scaledInvertedPercentMaxSpeed = invertedPercentMaxSpeed * m_fSlowMovingTurnCoefficient;

		// else we're drifting right, do our right stuff  and add our turn modifier that's based on speeed
		m_fTurnAmount += fAmount * m_fTurnRate * m_fDeltaTime + scaledInvertedPercentMaxSpeed*m_fTurnRate*m_fDeltaTime;

		// if we're drifting and turning
		if(m_nFramesDrifting > 0)
		{
			// set our drift direction
			m_nDriftDirection = GORIGHT;
		}
		else // we're not drifting but we did turn
		{
			// say we're not
			m_nDriftDirection = ACC_ERR;
		}
	}
	else if(eInput == GOLEFT)
	{
		SendInputEvent("SteerRightAnim", this, m_pObject, 1.0f);
		// if we're drifting the other direction
		if(m_nDriftDirection == GORIGHT)
		{
			// don't turn
			return;
		}

		// get our speed turn modifier
		float percentOfMaxSpeed = m_fSpeed / m_fMaxSpeed;
		// invert it
		float invertedPercentMaxSpeed = 1.0f - percentOfMaxSpeed;
		// scale it by how much we want to increase our turn speed when we're going slower
		float scaledInvertedPercentMaxSpeed = invertedPercentMaxSpeed * m_fSlowMovingTurnCoefficient;

		// else we're drifting right, do our right stuff   and add our turn modifier that's based on speeed
		m_fTurnAmount -= fAmount * m_fTurnRate * m_fDeltaTime  + scaledInvertedPercentMaxSpeed*m_fTurnRate*m_fDeltaTime;

		// if we're drifting and turning
		if(m_nFramesDrifting > 0)
		{
			// set our drift direction
			m_nDriftDirection = GOLEFT;
		}
		else // we're not drifting but we did turn
		{
			// say we're not
			m_nDriftDirection = ACC_ERR;
		}
	}
	// if we're not at the endgame win thingy
	else if( ! m_bEndgameSlowingDown)     // accelerate or decelerate is the only input option left we care about
	{
		m_eAccelerateBehavior = eInput;

		// if we're the human player
		if(m_nPlayerNumber == 0)
		{
			// do sound logic for this input
			PlayAccelerateOrBrakeSound(eInput);
		}

		if(m_eAccelerateBehavior == ACCELERATE)
		{
			SendInputEvent("AccelerateEffect", this, this->m_pObject, m_fSpeed);
		}

		// record input from this frame
		m_eAccelerateBehaviorLastFrame = eInput; // store this value in something that isn't reset in update position
		m_fAccelerateAmount = fAmount;
	}
}

void CMovement::HandleCollision(IEvent* cEvent, scd::base_component* cCenter)
{
	TRamEvent* pRamEvent = (TRamEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	CMovement* pRammeeComp = CMovementManager::GetInstance()->GetMovementComponent(pRamEvent->m_pcRammee);


	// SHOVE RAMMING
	// if we're the rammer and we're shoving (shovetime) and the ramEE isn't invincible
	if((pComp->GetObject() == pRamEvent->m_pcRammer) &&
		pComp->m_fShoveTimeLeft > 0.0f && pRammeeComp->m_fInvulnerableDuration <= 0.0f)
	{
		// The rammer is using shove



		// if we didn't get one back or what we did get back is invulnerable or the rammEE has a ram cd on him
		if(pRammeeComp == NULL || pRammeeComp->m_fInvulnerableDuration > 0.0f ||
			pRammeeComp->m_fTimeSinceRammedLast > 0.0f)
		{
			// gtfo, can't do stuff on it
			return;
		}

		// Treat it like a rammed event but don't check the speed
		SendRamEvent("PlayerRammed", pComp, pRamEvent->m_pcRammer,
			pRamEvent->m_pcRammee);
		pRammeeComp->m_fTimeSinceRammedLast = pRammeeComp->m_fRammedCooldown;
		return;
	}

	// SPEED RAMMING
	if( pComp->m_pObject == pRamEvent->m_pcRammer)/**/
	{
		// grab the component of the rammee

		// if we didn't get one back or what we did get is invulnerable or the rammEE has a cd on being rammed
		if(pRammeeComp == NULL || pRammeeComp->m_fInvulnerableDuration > 0.0f ||
			pRammeeComp->m_fTimeSinceRammedLast > 0.0f)
		{
			// gtfo, can't do stuff on it
			return;
		}

		// get the headings of each
		scd::vector3 vRammerHeading(pRamEvent->m_pcRammer->GetTransform()->GetWorldMatrix()._31,
			pRamEvent->m_pcRammer->GetTransform()->GetWorldMatrix()._32,
			pRamEvent->m_pcRammer->GetTransform()->GetWorldMatrix()._33);
		scd::vector3 vRammeeHeading(pRamEvent->m_pcRammee->GetTransform()->GetWorldMatrix()._31,
			pRamEvent->m_pcRammee->GetTransform()->GetWorldMatrix()._32,
			pRamEvent->m_pcRammee->GetTransform()->GetWorldMatrix()._33);

		// grab the vector between each object (to compare heading) (from rammer to ramee)
		scd::vector3 vVecBetween(pRamEvent->m_pcRammee->GetTransform()->GetWorldPosition() -
			pRamEvent->m_pcRammer->GetTransform()->GetWorldPosition());
		D3DXVec3Normalize(&vVecBetween, &vVecBetween);

		// makes it up to 1 for facing towards (or opposte, which wont trigger speed for ram) and down to -1 for facing same direction
		//float fAngleDifferenceInHeading = D3DXVec3Dot(&vRammeeHeading, &vRammerHeading) * -1.0f; // *-1 so facing towards each other is good and facing away is bad

		// get the difference of the heading vs the vector between the two to compare how headon it is
		float fHeadingDifferenceFromVecBetween = D3DXVec3Dot(&vVecBetween, &vRammerHeading);

		// scale the headings by their speed to make velocity
		D3DXVec3Scale(&vRammerHeading, &vRammerHeading, pComp->m_fSpeed);
		D3DXVec3Scale(&vRammeeHeading, &vRammeeHeading, pRammeeComp->m_fSpeed);

		// get the difference between the velocities
		scd::vector3 vDifferenceInHeadings = vRammeeHeading - vRammerHeading;

		// get the magnitude of the difference (converts to velocities then back to speed to get relative angles)
		float fSpeedDifference = D3DXVec3Length(&vDifferenceInHeadings);


		// if we're slowed by pb, since that doesn't affect the speed variable, affect it here
		if(pComp->m_bIsSlowed)
		{
			fSpeedDifference *= pComp->m_fSlowAmount;
		}

		// if the speed difference is greater than the threshold needed for the rammer to ram
		if(pComp->m_fSpeed > pComp->m_fRammingSpeed && fHeadingDifferenceFromVecBetween > RAM_HEADING_DIFFERENCE)
		{
			pRammeeComp->m_fTimeSinceRammedLast = pRammeeComp->m_fRammedCooldown;

			// Post event: Rammed passing the players
			SendRamEvent("PlayerRammed", pComp, pRamEvent->m_pcRammer, pRamEvent->m_pcRammee);

		}
		int charID = -1;
		charID = CHUDManager::GetInstance()->GetPlayerCharID(CHUDManager::GetInstance()->GetPlayerNum(pRamEvent->m_pcRammer));
		pSound->PlayTheSound(CART_PLAYER_COLLISION, charID);
	}
}

void CMovement::HandleShoveLeft(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	// if we're the player the input's for and we're not shoving and we're going fast enough to shove
	// and the time since last shove is decremented to be less than -1*cooldown (*-1 is done in init)
	if(pEvent->m_pPlayer == pComp->m_pObject && pComp->m_fShoveTimeLeft < pComp->m_fShoveCooldown &&
		pComp->m_fShoveTimeLeft <= 0.0f && fabs(pComp->m_fSpeed) > pComp->m_fTranslateThreshold)
	{
		pComp->m_fShoveTimeLeft = pComp->m_fShoveDuration;
		pComp->m_nShoveDirection = -1;

		// if we have inverted controls, make the direction opposite
		if(pComp->m_fInvertedControlTimeLeft > 0.0f)
		{
			pComp->m_nShoveDirection *= -1;
		}

		///sound
		pComp->PlayShoveSound(pComp->m_pObject);
		// TODO: Send effect event now
		SendInputEvent("ShoveLeftAnimation", pComp, pComp->m_pObject, 1.0f);
	}
}

void CMovement::HandleShoveRight(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	// if we're the player the input's for and we're not shoving and we're going fast enough to shove
	// and the time since last shove is decremented to be less than -1*cooldown (*-1 is done in init)
	if(pEvent->m_pPlayer == pComp->m_pObject && pComp->m_fShoveTimeLeft < pComp->m_fShoveCooldown &&
		pComp->m_fShoveTimeLeft <= 0.0f && fabs(pComp->m_fSpeed) > pComp->m_fTranslateThreshold)
	{
		pComp->m_fShoveTimeLeft = pComp->m_fShoveDuration;

		pComp->m_nShoveDirection = 1;

		// if we have inverted controls, make the direction opposite
		if(pComp->m_fInvertedControlTimeLeft > 0.0f)
		{
			pComp->m_nShoveDirection *= -1;
		}

		pComp->PlayShoveSound(pComp->m_pObject);
		//  Send effect event now
		SendInputEvent("ShoveRightAnimation", pComp, pComp->m_pObject, 1.0f);
	}
}

void CMovement::Boost(IEvent* cEvent, scd::base_component* cCenter)
{
	// Change this to receive boost event rather than handling space bar
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_pObject == pComp->m_pObject)
	{
		pComp->m_fSpeed = pComp->m_fMaxSpeed * 2.0f;

		//sound
		int charID = -1;
		charID = CHUDManager::GetInstance()->GetPlayerCharID(pComp->GetPlayerNumber());
		pSound->PlayTheSound(BULLDOG_CART_BOOST, charID);

	}
}


void CMovement::HandleInputAccelerate(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_pPlayer == pComp->m_pObject)
	{
		if(pComp->m_nFramesDrifting <= 0)
		{
			if(pComp->GetPlayerInvulnerable())
			{
				pComp->HandleInput(ACCELERATE, pEvent->m_fAmount*5);
			}
			else
			{
				pComp->HandleInput(ACCELERATE, pEvent->m_fAmount);
			}

			pComp->m_vPrevVel.x = pComp->m_pObject->GetTransform()->GetLocalMatrix()._31;
			pComp->m_vPrevVel.y = pComp->m_pObject->GetTransform()->GetLocalMatrix()._32;
			pComp->m_vPrevVel.z = pComp->m_pObject->GetTransform()->GetLocalMatrix()._33;
		}
	}

}

void CMovement::HandleInputBrake(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_pPlayer == pComp->m_pObject)
	{
		pComp->HandleInput(BRAKE, pEvent->m_fAmount);
	}
}

void CMovement::HandleInputRight(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_pPlayer == pComp->m_pObject)
	{
		if(pComp->m_fInvertedControlTimeLeft > 0.0f)
		{
			pComp->HandleInput(GOLEFT, pEvent->m_fAmount);
		}
		else
		{
			pComp->HandleInput(GORIGHT, pEvent->m_fAmount);
		}
	}

}

void CMovement::HandleInputLeft(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_pPlayer == pComp->m_pObject)
	{
		if(pComp->m_fInvertedControlTimeLeft > 0.0f)
		{
			pComp->HandleInput(GORIGHT, pEvent->m_fAmount);
		}
		else
		{
			pComp->HandleInput(GOLEFT, pEvent->m_fAmount);
		}
	}
}

void CMovement::SetWeightCallback(IEvent* cEvent, scd::base_component* cCenter)
{
	TWeightClassEvent* pEvent = (TWeightClassEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject)
	{
		pComp->m_eWeight = pEvent->m_eWeightClass;
		pComp->CalculateValuesFromWeight();
	}

}

void CMovement::SlipStatusCallback(IEvent* cEvent, scd::base_component* cCenter)
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

		SendStatusEffectEvent("SlipEffect", cEvent->GetSender(), pEvent->m_pObject, pEvent->m_fDuration);
		int playernum = CHUDManager::GetInstance()->GetPlayerNum(pComp->m_pObject);
		pSound->PlayTheSound(BULLDOG_BIKE_SLIP, CHUDManager::GetInstance()->GetPlayerCharID(playernum));
		pSound->PlayerHurtSound(playernum);
		pComp->m_fSlipTurnAmt = 0.0f;
		pComp->m_bIsSlipping = true;

	}

}

void CMovement::StunStatusCallback(IEvent* cEvent, scd::base_component* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// we're the one's stunned. Init stun data
		pComp->ApplyStun(pEvent->m_fDuration);
		SendStatusEffectEvent("StunEffect", cEvent->GetSender(), pEvent->m_pObject, pEvent->m_fDuration);

		// TODO: Send Effect stuff here
	}
}


void CMovement::FreezeStatusCallback(IEvent* cEvent, scd::base_component* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// we're the one's stunned. Init stun data
		pComp->ApplyStun(pEvent->m_fDuration);
		SendStatusEffectEvent("FreezeEffect", cEvent->GetSender(), pEvent->m_pObject, pEvent->m_fDuration);

		// TODO: Send Effect stuff here

	}

}

void CMovement::SlowStatusCallback(IEvent* cEvent, scd::base_component* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		// we're the one's slowed. Init slow data
		pComp->m_fSlowDuration = pEvent->m_fDuration == 0.0f ? 3.0f : pEvent->m_fDuration; // if duration is 0, use default of 3

		// if we're not slowed when we hit the PB, set the slow amount to the start of the decrement amount
		if( ! pComp->m_bIsSlowed)
		{
			pComp->m_fSlowAmount = 1.0f;
		}

		pComp->m_bIsSlowed = true;
		SendStatusEffectEvent("SlowEffect", cEvent->GetSender(), pEvent->m_pObject, pEvent->m_fDuration);
	}
}

void CMovement::BlindStatusCallback(IEvent* cEvent, scd::base_component* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		SendStatusEffectEvent("BlindEffect", cEvent->GetSender(), pEvent->m_pObject, pEvent->m_fDuration);
		int playernum = CHUDManager::GetInstance()->GetPlayerNum(pComp->m_pObject);
		pSound->PlayTheSound(STATUS_BLIND, CHUDManager::GetInstance()->GetPlayerCharID(playernum));
		pSound->PlayerHurtSound(playernum);
	}
}

void CMovement::InvulnerableStatusCallback(IEvent* cEvent, scd::base_component* cCenter)
{
	TStatusEffectEvent* pEvent = (TStatusEffectEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pComp->m_pObject == pEvent->m_pObject)
	{
		// we're the one's invulnerable. Init invulnerable data
		pComp->m_fInvulnerableDuration = pEvent->m_fDuration == 0.0f ? 5.0f : pEvent->m_fDuration; // if duration is 0, use default of 3
		SendStatusEffectEvent("InvulnerableEffect", cEvent->GetSender(), pEvent->m_pObject, pEvent->m_fDuration);

		pComp->m_fSlipDuration = .05f;
		pComp->m_fSlowDuration = .05f;

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
	m_fBounceOffTimeLeft -= fDt;
	m_fTimeToDrift -= fDt;
	m_fInvertedControlTimeLeft -= fDt;

	if(m_fSlowAmount > .5f)
	{
		// only slow if we're above the normal .5 slow amount
		m_fSlowAmount -= fDt;
		// since we start at 1, this means we'll have .5s of slow transition into slow
	}
}

void CMovement::ApplyShove(float fDt, scd::vector3& vTranslateVec)
{
	// Apply shove
	// if we're shoving and not slipping
	if(m_fShoveTimeLeft > 0.0f)
	{
		scd::vector3 rightDir;
		scd::vector3 upDir(0.0f, 1.0f, 0.0f);
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

	if(m_fSoundPlayingTime > BRAKE_SOUNDLENGTH &&
		(m_eAccelerateBehavior == BRAKE))
	{
		m_fSoundPlayingTime = 0.0f;
		// PLAY BRAKE SOUND HERE
		switch(CHUDManager::GetInstance()->GetPlayer1Char())
		{
		case BIKER_CHARACTER:
			{
				pSound->PlayTheSound(BULLDOG_CART_BRAKE, BIKER_ID);
				break;
			}
		case BANDITOS_CHARACTER:
			{
				pSound->PlayTheSound(WOODCART_BRAKE, BANDITOS_ID);
				break;
			}
		case LARPERS_CHARACTER:
			{
				pSound->PlayTheSound(WOODCART_BRAKE, LARPER_ID);
				break;
			}
		case SCIENTIST_CHARACTER:
			{
				pSound->PlayTheSound(SCIENTIST_BRAKE, SCIENTIST_ID);
				break;
			}
		case SASHA_CHARACTER:
			{
				pSound->PlayTheSound(SASHA_BRAKE, SASHA_ID);
				break;
			}
		case CRYGAME_CHARACTER:
			{
				pSound->PlayTheSound(SCIENTIST_BRAKE, CRYGAME_ID);
				break;
			}
		case STORYTIME_CHARACTER:
			{
				pSound->PlayTheSound(SCIENTIST_BRAKE, STORYTIME_ID);
				break;
			}
		}
	}
	else if (m_fSoundPlayingTime > BRAKE_SOUNDLENGTH && m_eAccelerateBehavior == DRIFTING)
	{
		m_fSoundPlayingTime = 0.0f;

		switch(CHUDManager::GetInstance()->GetPlayer1Char())
		{
		case BIKER_CHARACTER:
			{
				pSound->PlayTheSound(BULLDOG_CART_BRAKE, BIKER_ID);
				break;
			}
		case BANDITOS_CHARACTER:
			{
				pSound->PlayTheSound(WOODCART_DRIFT, BANDITOS_ID);
				break;
			}
		case LARPERS_CHARACTER:
			{
				pSound->PlayTheSound(WOODCART_DRIFT, LARPER_ID);
				break;
			}
		case SCIENTIST_CHARACTER:
			{
				pSound->PlayTheSound(SCIENTIST_DRIFT, SCIENTIST_ID);
				break;
			}
		case SASHA_CHARACTER:
			{
				pSound->PlayTheSound(SASHA_DRIFT, SASHA_ID);
				break;
			}
		case CRYGAME_CHARACTER:
			{
				pSound->PlayTheSound(SCIENTIST_DRIFT, CRYGAME_ID);
				break;
			}
		case STORYTIME_CHARACTER:
			{
				pSound->PlayTheSound(SCIENTIST_DRIFT, STORYTIME_ID);
				break;
			}
		}
	}


}

void CMovement::StartOfGame(IEvent* /*cEvent*/, scd::base_component* cCenter)
{
	CMovement* pComp = (CMovement*)cCenter;

	pComp->m_pObject->GetTransform()->GetLocalMatrix() = pComp->m_mStartMatrix;

	if (CHUDManager::GetInstance()->GetPlayerNum(pComp->m_pObject) == 0)
	{
		SendObjectEvent("AttachToCamera", pComp, pComp->m_pObject,
			PRIORITY_IMMEDIATE);

	}

	pComp->m_fSpeed = 0.0f;
	isplaying = false;

	pComp->m_bEndgameSlowingDown = false;

	// reset our debuff timers and stuff
	pComp->m_fInvulnerableDuration = 0.0f;
	pComp->m_fSlowDuration = 0.0f;
	pComp->m_fStunDuration = 0.0f;
	pComp->m_fSlipDuration = 0.0f;
	pComp->m_bIsSlipping =  false;/**/
	pComp->m_bIsSlowed =  false;/**/

}


void CMovement::BounceCallbackPlayer(IEvent* cEvent, scd::base_component* cCenter)
{
	CMovement* pComp = (CMovement*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

	/*
	if(tEvent->m_pcCollider == pComp->GetObject())
	{
	if(pComp->m_fInvulnerableDuration <= 0.0f)
	{
	SendImpactEvent("CartCollisionEffect", cEvent->GetSender(), tEvent->m_pcCollider, tEvent->m_pCollidedWith, scd::vector3(0.0f, 1.0f, 0.0f));
	}

	pComp->m_fSpeed *= .8f;
	return;
	}/*This was a hack*/

	if(tEvent->m_pcCollider == pComp->m_pObject && pComp->m_fBounceOffTimeLeft <= 0.0f)
	{
		// if we're going too slow, don't bounce
		if(pComp->m_fSpeed < 5.0f)
		{
			return;
		}


		scd::vector3 vForward(pComp->m_pObject->GetTransform()->GetLocalMatrix()._31,
			pComp->m_pObject->GetTransform()->GetLocalMatrix()._32,
			pComp->m_pObject->GetTransform()->GetLocalMatrix()._33);
		//D3DXVec3Normalize(&vForward, &vForward);


		// get the normal from the event
		scd::vector3 colNormal = tEvent->m_vNormal;
		D3DXVec3Normalize(&colNormal, &colNormal); // eventually this will be redundant. check with RAY in the future

		//its a wall, probably (  Vi - 2(Vi[DOT]N)N
		scd::vector3 reflectvec = vForward - (D3DXVec3Dot(&vForward, &colNormal) * colNormal * 2.0f);
		D3DXVec3Normalize(&reflectvec, &reflectvec);

		float fDot = D3DXVec3Dot(&vForward, &colNormal);
		pComp->m_fSpeed *= (1.0f - .2f*fabs(fDot)); // range .9f to 1.0f for scale speed

		if(fabs(fDot) > REFLECT_DOT_MIN)
		{
			float slide = .025f;

			pComp->m_vBounceVec = reflectvec * slide;
			pComp->m_vBounceVec.y = 0.0f;
			pComp->m_fBounceOffTimeLeft = pComp->m_fBounceOffWallDuration * pComp->m_fSpeed * slide;

			if(pComp->GetPlayerInvulnerable())
			{
				pComp->m_fSpeed *= .7f; // lower the speed by half since it was head on
			}
			else
			{
				pComp->m_fSpeed *= .5f; // lower the speed by half since it was head on
			}

			SendImpactEvent("CartCollisionEffect", cEvent->GetSender(), tEvent->m_pcCollider, tEvent->m_pCollidedWith, scd::vector3(0.0f, 1.0f, 0.0f));
		}
	}
}


void CMovement::BounceCallbackEnvironment(IEvent* cEvent, scd::base_component* cCenter)
{
	CMovement* pComp = (CMovement*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

	/*
	if(tEvent->m_pcCollider == pComp->GetObject())
	{
	pComp->m_fSpeed *= .8f;
	return;
	}/*Slowdown Hack*/
	scd::vector3 movedir;
	movedir.x = pComp->GetObjectA()->GetTransform()->GetLocalMatrix()._31;
	movedir.y = pComp->GetObjectA()->GetTransform()->GetLocalMatrix()._32;
	movedir.z = pComp->GetObjectA()->GetTransform()->GetLocalMatrix()._33;
	//float fDotRes = D3DXVec3Dot(&tEvent->m_vNormal, &movedir);
	float slide = .05f;

	//	if(fDotRes < 1.0f && fDotRes > 0.717f ||
	//	fDotRes > -1.0f && fDotRes < -0.717f)
	//		slide = 0.2f;
	//	if(pComp->m_fSpeed > pComp->m_fMaxSpeed)
	//		pComp->m_fSpeed = pComp->m_fMaxSpeed; // MOO

	if(tEvent->m_pcCollider == pComp->m_pObject && pComp->m_fBounceOffTimeLeft <= 0.0f)
	{
		scd::vector3 vForward(pComp->m_pObject->GetTransform()->GetLocalMatrix()._31,
			pComp->m_pObject->GetTransform()->GetLocalMatrix()._32,
			pComp->m_pObject->GetTransform()->GetLocalMatrix()._33);
		//D3DXVec3Normalize(&vForward, &vForward);


		// get the normal from the event
		scd::vector3 colNormal = tEvent->m_vNormal;
		D3DXVec3Normalize(&colNormal, &colNormal); // eventually this will be redundant. check with RAY in the future


		//its a wall, probably (  Vi - 2(Vi[DOT]N)N
		scd::vector3 reflectvec = vForward - (D3DXVec3Dot(&vForward, &colNormal) * colNormal * 2.0f);
		D3DXVec3Normalize(&reflectvec, &reflectvec);

		float fDot = D3DXVec3Dot(&vForward, &colNormal);
		pComp->m_fSpeed *= (1.0f - .1f*fabs(fDot)); // range .9f to 1.0f for scale speed

		if(fabs(fDot) > REFLECT_DOT_MIN && pComp->m_fSpeed > 5.0f)
		{
			// play collison sound only if speed is greater than 3 (and we're mostly head on)
			int charID = -1;
			charID = CHUDManager::GetInstance()->GetPlayerCharID(pComp->GetPlayerNumber());
			pSound->PlayTheSound(CART_WALL_COLLISION, charID);

			pComp->m_vBounceVec = reflectvec * slide * 2.0f;
			pComp->m_vBounceVec.y = 0.0f;
			pComp->m_fBounceOffTimeLeft = pComp->m_fBounceOffWallDuration * slide * pComp->m_fSpeed;


			if(pComp->GetPlayerInvulnerable())
			{
				pComp->m_fSpeed *= .7f; // lower the speed by half since it was head on
			}
			else
			{
				pComp->m_fSpeed *= .5f; // lower the speed by half since it was head on
			}


			SendObjectEvent("BouncebackEffect", pComp, pComp->m_pObject);
		}
		else
		{

		}


		// turn them a little bit if they're not actively turning and they're accelerating (so if you're still it doesn't break)
		if(pComp->m_fTurnAmount == 0.0f && pComp->m_eAccelerateBehavior == ACCELERATE)
		{
			// generate a right vector
			scd::vector3 vRight;
			D3DXVec3Cross(&vRight, &vForward, &scd::vector3(0.0f, 1.0f, 0.0f));
			// half space test on that to see right or left turn
			float fHalfSpaceValue = D3DXVec3Dot(&vRight, &colNormal);

			// increase or decrease the turn amount based on that
			// if it's on the right side
			if(fHalfSpaceValue < 0.0f)
			{
				// turn right
				pComp->m_fTurnAmount += pComp->m_fTurnRate * pComp->m_fDeltaTime * 1.0f; // *constant so it's a smaller value
			}
			else // otehr side
			{
				// turn left
				pComp->m_fTurnAmount -= pComp->m_fTurnRate * pComp->m_fDeltaTime * 1.0f; // *constant so it's a smaller value
			}
		}

		// play sideswipe sound (eventually)

	}
}


void CMovement::HandleInputDrift(IEvent* cEvent, scd::base_component* cCenter)
{
	TInputEvent* pEvent = (TInputEvent*)cEvent->GetData();
	CMovement* pComp = (CMovement*)cCenter;

	if(pEvent->m_pPlayer == pComp->m_pObject)
	{
		// This doesn't get set true every frame sadly so right now it's not used
		pComp->m_bIsDrifting = true;
		pComp->m_nFramesDrifting = 4;


		// if we're not turning or we're going slow enough that we can't really drift
		if(pComp->m_fTurnAmount == 0.0f || pComp->m_fSpeed < pComp->m_fTranslateThreshold)
		{
			// don't drift
			return;
		}


		pComp->m_fTimeToDrift = .3f;

		SendInputEvent("DriftEffect", pComp, pComp->m_pObject, 1.0f);

		pComp->HandleInput(DRIFTING);

		pComp->m_vCurVel.x = pComp->m_pObject->GetTransform()->GetLocalMatrix()._31;
		pComp->m_vCurVel.y = pComp->m_pObject->GetTransform()->GetLocalMatrix()._32;
		pComp->m_vCurVel.z = pComp->m_pObject->GetTransform()->GetLocalMatrix()._33;

		//pComp->m_vPrevVel = pComp->m_vCurVel;

	}
}

void CMovement::ApplyStun(float fDuration)
{
	m_fStunDuration = fDuration == 0.0f ? 3.0f : fDuration; // if duration is 0, use default of 3
}

void CMovement::PostSpeed()
{
	SendSpeedEvent("SpeedValue", this, m_pObject, m_fSpeed);
}

void CMovement::SendMovementInformation(IEvent*, scd::base_component* pComp)
{
	CMovement* pMovement = (CMovement*)pComp;
	SendSpeedEvent("ShowRamSpeed", pMovement, pMovement->m_pObject, pMovement->m_fRammingSpeed);
	SendSpeedEvent("ShowMaxSpeed", pMovement, pMovement->m_pObject, pMovement->m_fMaxSpeed);
	SendSpeedEvent("ShowTurnSpeed", pMovement, pMovement->m_pObject, pMovement->m_fTurnRate);
	SendSpeedEvent("ShowAcceleration", pMovement, pMovement->m_pObject, pMovement->m_fAccelerateCoefficient);
}


void CMovement::DisableMovementInput(IEvent* /*cEvent*/, scd::base_component* iComp)
{
	CMovement* pComp = (CMovement*)iComp;
	pComp->m_bInputEnabled = false;

}

void CMovement::EnableMovementInput(IEvent* /*cEvent*/, scd::base_component* iComp)
{
	CMovement* pComp = (CMovement*)iComp;
	pComp->m_bInputEnabled = true;
}

void CMovement::GameWonCallback(IEvent*, scd::base_component* iComp)
{
	CMovement* pComp = (CMovement*)iComp;

	pComp->m_bEndgameSlowingDown = true;

	// reset our debuff timers and stuff
	/*pComp->m_fInvulnerableDuration = 0.0f;
	pComp->m_fShoveDuration = 0.0f;
	pComp->m_fSlowDuration = 0.0f;
	pComp->m_fStunDuration = 0.0f;
	pComp->m_fSlipDuration = 0.0f;/**/
}

void CMovement::StealCallback(IEvent* iEvent, scd::base_component* iComp)
{
	TImpactEvent* pEvent = (TImpactEvent*)iEvent->GetData();
	CMovement* pComp = (CMovement*)iComp;

	if(pComp->m_pObject == pEvent->m_pCollidedWith  && pComp->m_fInvulnerableDuration <= 0.0f)
	{
		SendImpactEvent("StealItem", pComp, pEvent->m_pcCollider, pEvent->m_pCollidedWith, scd::vector3(0.0f, 0.0f, 0.0f));
	}
}
void CMovement::PlayShoveSound(scd::object* object)
{
	int playerNum = -1;
	playerNum = CHUDManager::GetInstance()->GetPlayerNum(object);

	switch(CHUDManager::GetInstance()->GetPlayerCharacter(playerNum))
	{
	case BIKER_CHARACTER:
		{
			pSound->PlayTheSound(BULLDOG_SHOVE, BIKER_ID);
			break;
		}
	case BANDITOS_CHARACTER:
		{
			pSound->PlayTheSound(BANDITOS_SHOVE, BANDITOS_ID);
			break;
		}
	case LARPERS_CHARACTER:
		{
			pSound->PlayTheSound(LARPER_SHOVE, LARPER_ID);
			break;
		}
	case SCIENTIST_CHARACTER:
		{
			pSound->PlayTheSound(SCIENTIST_SHOVE, SCIENTIST_ID);
			break;
		}
	case SASHA_CHARACTER:
		{
			pSound->PlayTheSound(SASHA_ITEMUSE, SASHA_ID);
			break;
		}
	case CRYGAME_CHARACTER:
		{
			pSound->PlayTheSound(CRYGAME_SHOVE, CRYGAME_ID);
			break;
		}
	case STORYTIME_CHARACTER:
		{
			pSound->PlayTheSound(STORYTIME_SHOVE, STORYTIME_ID);
			break;
		}
	}
}
