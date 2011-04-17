#include <string>
#include <iostream>

#include "..\..\Managers\Component Managers\CAIManager.h"
#include "CAIComponent.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CInputEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CObjectEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CRenderEvent.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"


CAIComponent::CAIComponent(CObject* pObj) : m_pObject(pObj)
{
}

CAIComponent::~CAIComponent()
{
}

void CAIComponent::Init()
{
	CAIManager* pcAIM = CAIManager::GetInstance();
	m_pfWeight[COLLECTIVE] = 0;
	m_pfWeight[AGGRESSIVE] = 0;
	m_pfWeight[DEFENSIVE] = 0;

	m_eCurrentState = NOT_INIT;

	m_tKnowledge.m_cGoalItemsInLevel.clear();
	m_tKnowledge.m_pchMyHeldItems[0] = -1;
	m_tKnowledge.m_pchMyHeldItems[1] = -1;
	m_tKnowledge.m_cNeededGoalItems.clear();
	m_tKnowledge.m_nDistanceFromGoal = 0;
	m_tKnowledge.m_nOpponentID = 0;

	for(unsigned nPlayers = 0; nPlayers < 3; ++nPlayers)
	{
		m_tKnowledge.m_cOpponents[nPlayers].m_nPlayer = -1;
		m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = false;
		m_tKnowledge.m_cOpponents[nPlayers].m_bTarget = false;
		m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.clear();
		m_tKnowledge.m_cOpponents[nPlayers].m_chAmtGoalItems = 0;
		m_tKnowledge.m_cOpponents[nPlayers].m_nItem = -1;
		m_tKnowledge.m_cOpponents[nPlayers].m_nAggression = 0;
	}

	m_fTimer = 1.0f;

	m_nFramesToAvoidObstacle = 0;
	m_vTargetPosition = D3DXVECTOR3(3.0f, 0.0f, 12.0f);
	m_vAvoidanceTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_nControllerNumber = 1;
	m_nFramesWeAreOnThisTarget = 500;
	m_nFramesToAccel = 3;
}

void CAIComponent::SetupOpponents(CObject* cPlayer)
{
	if(m_pObject->GetID() != cPlayer->GetID())
	{
		m_tKnowledge.m_cOpponents[m_tKnowledge.m_nOpponentID].m_nPlayer = cPlayer->GetID();
		++m_tKnowledge.m_nOpponentID;
	}
}

// Events Start
void CAIComponent::Update(const float fDT)
{
	m_fTimer += fDT;

	// AI Steering stuff
	// if we're in gameplay state (i.e. input isn't disabled)
	if(CInputManager::GetInstance()->GetState() == GAMEPLAY_STATE)
	{
		SteerTowardTarget();
		AvoidObstacles();
	}
	// HACK: Adds self to render set
	// Render
	CRenderEvent* pE = MMNEWEVENT(CRenderEvent) CRenderEvent(CIDGen::
			GetInstance()->GetID("AddToSet"), NULL, m_pObject);
	CEventManager::GetInstance()->PostEvent(pE, PRIORITY_NORMAL);

	if(m_tKnowledge.m_cNeededGoalItems.size() == 0)
	{
		m_vTargetPosition = D3DXVECTOR3(0, 0, -65535);
		return;
	}

	// Don't do this every frame
	//if(m_fTimer < 0.1f)
	//{
	//	return;
	//}

	m_fTimer = 0;
	
	EvaluateStateWeights();

	// This horrible thing is how you
	// call a member function pointer
	// that calls a member function
	((*this).*(m_pfUpdateState))();
}

void CAIComponent::UpdateCollective()
{
	EvaluateOpponentWeights();
	EvaluateItemWeights();
}

void CAIComponent::UpdateAggressive()
{
	CAIManager* pcAIManager = CAIManager::GetInstance();
	std::list<CObject*,	CAllocator<CObject*>>::iterator cOpponentIter;
	cOpponentIter = pcAIManager->m_cPlayers.begin();

	D3DXVECTOR3* cPlayerPos = NULL;
	D3DXVECTOR3 cTempPlayerPos;

	for(unsigned CurOpponent = 0; CurOpponent < 3; ++CurOpponent)
	{
		if(m_tKnowledge.m_cOpponents[CurOpponent].m_bHasItem == true)
		{
			// Do I have a suitable item for attacking?
			// Yes
			
			// No
				// Do I know of a suitable item for attacking nearby?

			if(cPlayerPos == NULL)
			{
				*cPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			}
			else
			{
				cTempPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

				if(*cPlayerPos > cTempPlayerPos)
				{
					*cPlayerPos = cTempPlayerPos;
				}
			}
		}
		else if(m_tKnowledge.m_cOpponents[CurOpponent].m_bTarget == true)
		{
			// Do I have a suitable item for attacking?
			// Yes
			
			// No
				// Do I know of a suitable item for attacking nearby

			if(cPlayerPos != NULL)
			{
				*cPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			}
			else
			{
				cTempPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

				if(*cPlayerPos > cTempPlayerPos)
				{
					*cPlayerPos = cTempPlayerPos;
				}
			}
		}
	}

	if(cPlayerPos != NULL)
	{
		m_cTargetPlayerPos = *cPlayerPos;
		m_vTargetPosition = *cPlayerPos;
		return;
	}

	// Nearby held items I can use?
	// Yes
		// Set goal as item
	
	// No
		// Nearby goal items I can use?
		EvaluateItemWeights();
		// Yes
			// Set goal as item
		// No
			// Set goal as random point
}

void CAIComponent::UpdateDefensive()
{
}

D3DXVECTOR3 CAIComponent::GetOpponentPos(unsigned nPlayerID)
{
	CAIManager* pCAIManager = CAIManager::GetInstance();
	list<CObject*,	CAllocator<CObject*>>::iterator OpponentIter = 
		pCAIManager->m_cPlayers.begin();

	D3DXVECTOR3 cPlayerPos;

	while(OpponentIter != pCAIManager->m_cPlayers.end())
	{
		if((*OpponentIter)->GetID() == nPlayerID)
		{
			cPlayerPos.x = (*OpponentIter)->GetTransform()->
				GetWorldMatrix()._41;

			cPlayerPos.y = (*OpponentIter)->GetTransform()->
				GetWorldMatrix()._42;

			cPlayerPos.z = (*OpponentIter)->GetTransform()->
				GetWorldMatrix()._43;

			return cPlayerPos;
		}

		OpponentIter++;
	}

	return cPlayerPos;
}

CObject* CAIComponent::GetClosestOpponent()
{
	CAIManager* pCAIManager = CAIManager::GetInstance();
	list<CObject*,	CAllocator<CObject*>>::iterator OpponentIter = 
		pCAIManager->m_cPlayers.begin();

	D3DXVECTOR3 cPos[3];
	unsigned index = 0;

	while(OpponentIter != pCAIManager->m_cPlayers.end())
	{
		cPos[index] = (*OpponentIter)->GetTransform()->GetWorldPosition();

		++index;
		OpponentIter++;
	}

	if((cPos[0] >= cPos[1]) && (cPos[0] >= cPos[2]))
	{
		return *(pCAIManager->m_cPlayers.begin());
	}
	else if(cPos[1] >= cPos[2])
	{
		OpponentIter = pCAIManager->m_cPlayers.begin();

		return (*(OpponentIter++));
	}
	else
	{
		OpponentIter = pCAIManager->m_cPlayers.begin();

		return (*((OpponentIter++)++));
	}
	return *OpponentIter;
}

void CAIComponent::GoalItemInit(CObject* pObj)
{
	m_tKnowledge.m_cNeededGoalItems.push_back(pObj->GetID());
}

void CAIComponent::GoalItemCollected(CObject* pGoalItem, CObject* pCollector)
{
	list<TGoalItem, CAllocator<TGoalItem>>::iterator cGoalItemIter;
		cGoalItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();

	// Look for which player found it
	if(m_pObject->GetID() != pCollector->GetID())
	{
		for(unsigned nPlayer = 0; nPlayer < 3; ++nPlayer)
		{
			if(m_tKnowledge.m_cOpponents[nPlayer].m_nPlayer == pCollector->GetID())
			{
				bool bDupe = false;
				list<unsigned, CAllocator<unsigned>>::iterator ItemIter = 
					m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.begin();

				while(ItemIter != 
					m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.end())
				{
					if((*ItemIter) == pGoalItem->GetID())
					{
						bDupe = true;
						break;
					}
					ItemIter++;
				}

				if(bDupe == false)
				{
					++m_tKnowledge.m_cOpponents[nPlayer].m_chAmtGoalItems;

					D3DXVECTOR3 cOpponentPos;
					cOpponentPos.x = pCollector->GetTransform()->GetWorldMatrix()._41;
					cOpponentPos.z = pCollector->GetTransform()->GetWorldMatrix()._43;

					// Look to see if it was in radar
					//if(WithinRadar(cOpponentPos))
					{
						m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.push_back(
							pGoalItem->GetID());
					}

					break;
				}
			}
		}
	}
	else//(m_pObject->GetID() == pCollector->GetID())
	{
		list<unsigned, CAllocator<unsigned>>::iterator cNeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

		while(cNeededIter != m_tKnowledge.m_cNeededGoalItems.end())
		{
			if((*cNeededIter) == pGoalItem->GetID())
			{
				m_tKnowledge.m_cNeededGoalItems.erase(cNeededIter);
				break;
			}

			cNeededIter++;
		}
	}

	// Remove Goal Item from list
	while(cGoalItemIter != m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		if(cGoalItemIter->m_nGoalItem == pGoalItem->GetID())
		{
			m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
			return;
		}

		cGoalItemIter++;
	}
}

void CAIComponent::PlayerAttacked(IEvent*, IComponent*)
{
}

void CAIComponent::GoalItemSpawned(CObject* pObj)
{
	TGoalItem tGoalItem;
	tGoalItem.m_nGoalItem = pObj->GetID();
	tGoalItem.m_pfWeight = 0;
	tGoalItem.m_cPos.x = pObj->GetTransform()->GetWorldMatrix()._41;
	tGoalItem.m_cPos.y = pObj->GetTransform()->GetWorldMatrix()._42;
	tGoalItem.m_cPos.z = pObj->GetTransform()->GetWorldMatrix()._43;
	m_tKnowledge.m_cGoalItemsInLevel.push_back(tGoalItem);
}

void CAIComponent::GoalItemDespawned(CObject* pObj)
{
	list<TGoalItem, CAllocator<TGoalItem>>::iterator cGoalItemIter;
	cGoalItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();

	while(cGoalItemIter != m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		if(cGoalItemIter->m_nGoalItem == pObj->GetID())
		{
			m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
			return;
		}

		cGoalItemIter++;
	}
}
// Events End

void CAIComponent::EvaluateStateWeights()
{
	CAIManager* pcAIM = CAIManager::GetInstance();
	m_pfWeight[COLLECTIVE] = 0;
	m_pfWeight[AGGRESSIVE] = 0;
	m_pfWeight[DEFENSIVE] = 0;

	list<TGoalItem, CAllocator<TGoalItem>>::iterator	ItemIter;
	list<unsigned, CAllocator<unsigned>>::iterator		NeededIter;
	list<unsigned, CAllocator<unsigned>>::iterator		OpponentIter;

	ItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();
	
	while(ItemIter != m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

		while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
		{
			if(*NeededIter == ItemIter->m_nGoalItem)
			{
				m_pfWeight[COLLECTIVE] += 0.15f;
			}

			NeededIter++;
		}

		ItemIter++;
	}

	if(m_pfWeight[COLLECTIVE] < 0.3f)
	{
		for(unsigned nPlayers = 0; nPlayers < 3; ++nPlayers)
		{
			OpponentIter = 
				m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.begin();

			while(OpponentIter != 
				m_tKnowledge.m_cOpponents[nPlayers].m_cGoalItems.end())
			{
				NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

				while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
				{
					if(*OpponentIter == *NeededIter)
					{
						m_pfWeight[COLLECTIVE] += 0.20f;
						m_tKnowledge.m_cOpponents[nPlayers].m_bHasItem = true;
						m_tKnowledge.m_cOpponents[nPlayers].m_nItem = 
							*NeededIter;
					}

					NeededIter++;
				}

				OpponentIter++;
			}
		}
	}

	// Aggressive Weight
	for(unsigned nPlayers = 0; nPlayers < 3; ++nPlayers)
	{
		m_pfWeight[AGGRESSIVE] += 
			m_tKnowledge.m_cOpponents[nPlayers].m_nAggression * 0.1f;

		if(m_tKnowledge.m_cOpponents[nPlayers].m_nAggression > 2)
		{
			m_tKnowledge.m_cOpponents[nPlayers].m_bTarget = true;
			m_pfWeight[AGGRESSIVE] += 0.15f;
		}
	}

	if(m_pfWeight[COLLECTIVE] < 0.15f)
	{
		m_pfWeight[AGGRESSIVE] += 0.15f;
		m_pfWeight[DEFENSIVE] += 0.10f;
	}

	// Get New State
	if((m_pfWeight[DEFENSIVE] > m_pfWeight[COLLECTIVE]) &&
		(m_pfWeight[DEFENSIVE] > m_pfWeight[AGGRESSIVE]))
	{
		m_eCurrentState = DEFENSIVE;
		m_pfUpdateState = &CAIComponent::UpdateDefensive;
		return;
	}
	else if(m_pfWeight[COLLECTIVE] >= m_pfWeight[AGGRESSIVE])
	{
		m_eCurrentState = COLLECTIVE;
		m_pfUpdateState = &CAIComponent::UpdateCollective;
		return;
	}
	else
	{
		m_eCurrentState = AGGRESSIVE;
		m_pfUpdateState = &CAIComponent::UpdateAggressive;
		return;
	}
}

void CAIComponent::EvaluateItemWeights()
{
	CAIManager* pcAIM = CAIManager::GetInstance();
	list<TGoalItem, CAllocator<TGoalItem>>::iterator	ItemIter;
	list<unsigned, CAllocator<unsigned>>::iterator		NeededIter;
	D3DXVECTOR3 VecTo;
	float fMyDist(0);
	float fToSquareX(0);
	float fToSquareZ(0);
	float fDist(0);

	ItemIter = m_tKnowledge.m_cGoalItemsInLevel.begin();
	
	TGoalItem* pcTarget = NULL;

	// Loop through each Goal Item which the AI knows is in the level.
	// {
	//		Loop through all Goal Items the AI currently needs and
	//		checks to see if any of the Goal Items in the level matches
	//		any of the Goal Items the AI needs.
	// }
	while(ItemIter != m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		NeededIter = m_tKnowledge.m_cNeededGoalItems.begin();

		while(NeededIter != m_tKnowledge.m_cNeededGoalItems.end())
		{
			if(*NeededIter == ItemIter->m_nGoalItem)
			{
				// This will take the distance from each goal item
				// the agent is and make it a value between 0 - 1,
				// where the closer the item is, the higher the
				// value is. This is then added to the item's weight
				D3DXVECTOR3 cMe;
				cMe.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
				cMe.y = m_pObject->GetTransform()->GetWorldMatrix()._42;
				cMe.z = m_pObject->GetTransform()->GetWorldMatrix()._43;
				fToSquareX = ItemIter->m_cPos.x - cMe.x;
				fToSquareZ = ItemIter->m_cPos.z - cMe.z;

				fMyDist = (fToSquareX * fToSquareX) + (fToSquareZ * fToSquareZ);
				float MyWeight = 1 - (fMyDist / MAXDIST);
				
				if(MyWeight < 0)
					MyWeight = 0;
				else if(MyWeight > 1)
					MyWeight = 1;
				ItemIter->m_pfWeight = (MyWeight * 1.5f);

				// This will loop through each opponent in your minimap
				// and find out their weight and then will subtract
				// from each of the item's weights
				list<CObject*,	CAllocator<CObject*>>::iterator iter;
				iter = CAIManager::GetInstance()->m_cPlayers.begin();

				D3DXVECTOR3 OpponentPos;
				D3DXVECTOR3 OpponentHeading;

				for(unsigned nCurPlayer = 0; nCurPlayer < 3; ++nCurPlayer)
				{
					// If the current player is not me
					if((*iter)->GetID() != m_pObject->GetID())
					{
						OpponentPos.x = (*iter)->GetTransform()->GetWorldMatrix()._41;
						OpponentPos.y = (*iter)->GetTransform()->GetWorldMatrix()._42;
						OpponentPos.z = (*iter)->GetTransform()->GetWorldMatrix()._43;

						OpponentHeading.x = (*iter)->GetTransform()->GetLocalMatrix()._31;
						OpponentHeading.y = (*iter)->GetTransform()->GetLocalMatrix()._32;
						OpponentHeading.z = (*iter)->GetTransform()->GetLocalMatrix()._33;
						
						if(WithinRadar(OpponentPos))
						{
							if(WithinRadar(ItemIter->m_cPos))
							{
								VecTo = ItemIter->m_cPos - OpponentPos;
							}
							else
							{
								VecTo = ItemIter->m_cPos - OpponentPos;
							}

							fDist = D3DXVec3Dot(&VecTo, &VecTo);

							//if(D3DXVec3Dot(&OpponentHeading, &VecTo) < 0)
							{
								float OpponentWeight = 1 - (fDist / MAXDIST);
								
								if(OpponentWeight < 0)
									OpponentWeight = 0;
								else if(OpponentWeight > 1)
									OpponentWeight = 1;

								ItemIter->m_pfWeight -= (OpponentWeight / 3);

								if(ItemIter->m_pfWeight < 0.0f)
								{
									ItemIter->m_pfWeight = 0.0f;
								}
							}
						}
					}
					iter++;
				}

				if(pcTarget == NULL)
				{
					pcTarget = &(*ItemIter);
				}
				else
				{
					if(pcTarget->m_pfWeight < ItemIter->m_pfWeight)
					{
						pcTarget = &(*ItemIter);
					}
				}
			}

			NeededIter++;
		}

		ItemIter++;
	}

	if((pcTarget != NULL) && (pcTarget->m_pfWeight > 0))
	{
		//m_cTargetItemPos = pcTarget->m_cPos;
		m_vTargetPosition = pcTarget->m_cPos;
	}
}

void CAIComponent::EvaluateOpponentWeights()
{
	D3DXVECTOR3* cPlayerPos = NULL;
	D3DXVECTOR3 cTempPlayerPos;

	for(unsigned CurOpponent = 0; CurOpponent < 3; ++CurOpponent)
	{
		if(m_tKnowledge.m_cOpponents[CurOpponent].m_bHasItem == true)
		{
			if(cPlayerPos == NULL)
			{
				cPlayerPos = &GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			}
			else
			{
				cTempPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

				if(*cPlayerPos > cTempPlayerPos)
				{
					*cPlayerPos = cTempPlayerPos;
				}
			}
		}
		else if(m_tKnowledge.m_cOpponents[CurOpponent].m_bTarget == true)
		{
			if(cPlayerPos != NULL)
			{
				*cPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			}
			else
			{
				cTempPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

				if(*cPlayerPos > cTempPlayerPos)
				{
					*cPlayerPos = cTempPlayerPos;
				}
			}
		}
	}

	if(cPlayerPos != NULL)
	{
		m_cTargetPlayerPos = *cPlayerPos;
		m_vTargetPosition = *cPlayerPos;
	}
}

bool CAIComponent::WithinRadar(const D3DXVECTOR3& cPos)
{
	D3DXVECTOR3 cMe;
	cMe.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
	cMe.z = m_pObject->GetTransform()->GetWorldMatrix()._43;
	
	float x = cPos.x - cMe.x;
	float z = cPos.z - cMe.z;

	return ( (x * x) + (z * z) ) < MINIMAP_RADIUS;
}


void CAIComponent::SteerTowardTarget()
{
	D3DXVECTOR3 vObjPos;
	vObjPos.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
	vObjPos.y = m_pObject->GetTransform()->GetWorldMatrix()._42;
	vObjPos.z = m_pObject->GetTransform()->GetWorldMatrix()._43;

	// HACK: Forward isn't y, it's z in final build
	D3DXVECTOR3 vObjHeading;
	vObjHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
	vObjHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
	vObjHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;
	D3DXVec3Normalize(&vObjHeading, &vObjHeading);
	
	D3DXVECTOR3 vTarget;
	if(m_nFramesToAvoidObstacle == 0) // we're going toward target object
	{
		vTarget = m_vTargetPosition;
	}
	else // we're going toward the avoidance target
	{
		vTarget = m_vAvoidanceTargetPosition;
	}


	D3DXVECTOR3 desiredDirection = vTarget - vObjPos;  // from us to target
	float distanceToTarget = D3DXVec3Length(&desiredDirection);
	D3DXVec3Normalize(&desiredDirection, &desiredDirection);

	float fEpsilon = .01f; // tolerance level
	// check if we're already facing toward it
	float fDot = D3DXVec3Dot(&desiredDirection, &vObjHeading);
	if(fDot > 1.0f-fEpsilon)
	{
		return;
	}

	
	D3DXVECTOR3 rightDir;
	D3DXVec3Cross(&rightDir, &D3DXVECTOR3(0.0f,1.0f,0.0f), &vObjHeading);
	// TODO: Find out if this norm is necessary
	D3DXVec3Normalize(&rightDir, &rightDir);

	//the difference between the current velocity and the desired velocity 
	// Positive is turn right. Negative is turn left
	float turnDir = D3DXVec3Dot(&rightDir, &desiredDirection); 

	// TODO: Drifting logic
	// if the distance < someValue && the angle between heading and desiredDirection is high enough to warrant drifting
	//		turn on drifting so we can turn more tightly

	if(turnDir >= 0.0f) // on right side
	{
		// send out simulated input to the agent's object
		// HACK: Sends to everyone
		CInputEvent* pInputEvent = (CInputEvent*)CMemoryManager::GetInstance()
			->Allocate(sizeof(CInputEvent), HEAPID_EVENT);
		(*pInputEvent) = CInputEvent(CIDGen::GetInstance()->GetID(
			"SteerRight"), NULL, m_nControllerNumber, 1.0f);
		CEventManager::GetInstance()->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	else	// on left side
	{
		// send out simulated input to the agent's object
		// HACK: Sends to everyone
		CInputEvent* pInputEvent = (CInputEvent*)CMemoryManager::GetInstance()
			->Allocate(sizeof(CInputEvent), HEAPID_EVENT);
		(*pInputEvent) = CInputEvent(CIDGen::GetInstance()->GetID(
			"SteerLeft"), NULL, m_nControllerNumber, 1.0f);
		CEventManager::GetInstance()->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
}


void CAIComponent::AvoidObstacles()
{
	// Handle figuring out whether to accelerate or decelerate:
	// HACK: Move forward based on distance

	if(m_nFramesToAccel <= 2)
	{
		CInputEvent* pInputEvent = (CInputEvent*)CMemoryManager::GetInstance()
				->Allocate(sizeof(CInputEvent), HEAPID_EVENT);
		(*pInputEvent) = CInputEvent(CIDGen::GetInstance()->GetID(
			"Accelerate"), NULL, m_nControllerNumber, 1.0f);
		CEventManager::GetInstance()->PostEvent(pInputEvent, PRIORITY_INPUT);
	}
	else if(m_nFramesToAccel >= 3)
	{
		CInputEvent* pInputEvent = (CInputEvent*)CMemoryManager::GetInstance()
				->Allocate(sizeof(CInputEvent), HEAPID_EVENT);
		(*pInputEvent) = CInputEvent(CIDGen::GetInstance()->GetID(
				"Decelerate"), NULL, m_nControllerNumber, 1.0f);
		CEventManager::GetInstance()->PostEvent(pInputEvent, PRIORITY_INPUT);
	
	}
	--m_nFramesToAccel;


	// calc how many frames to accel
	D3DXVECTOR3 vObjPos(
			m_pObject->GetTransform()->GetLocalMatrix()._41, 
			m_pObject->GetTransform()->GetLocalMatrix()._42, 
			m_pObject->GetTransform()->GetLocalMatrix()._43);

	D3DXVECTOR3 vToTarget = m_vTargetPosition - vObjPos;
	float lengthToTarget = D3DXVec3Length(&vToTarget);

	if(m_nFramesToAccel <= 0)
	{
		if(lengthToTarget < .3f)
		{
			m_nFramesToAccel = 6;
		}
		else if(lengthToTarget < .6f)
		{
			m_nFramesToAccel = 5;
		}
		else if(lengthToTarget < 2.0f)
		{
			m_nFramesToAccel = 4;
		}
		else if(lengthToTarget < 5.0f)
		{
			m_nFramesToAccel = 3;
		}
		else
		{
			m_nFramesToAccel = 0;
		}
	}
	

	// Check heading so we can slow down to try to turn around
	D3DXVECTOR3 vObjHeading;
	vObjHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
	vObjHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
	vObjHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;
	D3DXVec3Normalize(&vObjHeading, &vObjHeading);

	D3DXVECTOR3 desiredDirection = m_vTargetPosition - vObjPos;  // from us to target
	float distanceToTarget = D3DXVec3Length(&desiredDirection);
	D3DXVec3Normalize(&desiredDirection, &desiredDirection);

	float fEpsilon = .7f; // tolerance level (45deg on each side)
	// check if we're already facing toward it
	float fDot = D3DXVec3Dot(&desiredDirection, &vObjHeading);
	if(fDot < -1.0f+fEpsilon) // behind 
	{
		m_nFramesToAccel = 1; // temporarily disable next frame's accel
	}


	// ENDHACK


	// check if we should try to avoid obstacles this frame (frame skip)

	// Update the feelers based on current heading (stay normalized)
	// Forward feeler
	// DE-HACKED: Make proper facing for xz plane instead of xy plane
	m_pvReactionFeelers[0].x = m_pObject->GetTransform()->GetLocalMatrix()._31;
	m_pvReactionFeelers[0].y = m_pObject->GetTransform()->GetLocalMatrix()._32;
	m_pvReactionFeelers[0].z = m_pObject->GetTransform()->GetLocalMatrix()._33;
	D3DXVec3Normalize(&m_pvReactionFeelers[0], &m_pvReactionFeelers[0]);

	// generate a right vector
	D3DXVECTOR3 vRightDirection;
	D3DXVec3Cross(&vRightDirection, &D3DXVECTOR3(0.0f, -1.0f, 0.0f), 
		&m_pvReactionFeelers[0]); // [0] is fwd heading
	D3DXVec3Normalize(&vRightDirection, &vRightDirection);


	// Side feelers
	m_pvReactionFeelers[1] = m_pvReactionFeelers[0] + vRightDirection;
	m_pvReactionFeelers[2] = m_pvReactionFeelers[0] - vRightDirection;
	D3DXVec3Normalize(&m_pvReactionFeelers[1], &m_pvReactionFeelers[1]);
	D3DXVec3Normalize(&m_pvReactionFeelers[2], &m_pvReactionFeelers[2]);

	// check feelers against nearby geometry
		// if(no collision) consider distance too if ray collision
			// continue
		// scale feelers
		// grab collision normal
		// set avoidance target to our pos + col normal
		// m_nAvoidingObstacle = how long we wanna turn away from it
}

void CAIComponent::PathFind()
{
	CAIManager* pcAIManager = CAIManager::GetInstance();
}