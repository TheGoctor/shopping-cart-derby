/***********************************************
 * Filename:  		CAIComponent.cpp
 * Date:      		04/04/2011
 * Mod. Date: 		05/12/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This is the AI's main class
			which is responsable for changing
			the AI's state based on weights.
					The AI will have three main
			states: Defensive, Collective, and 
			Aggressive. Each state will dertermine
			how the AI behaves in each situation.
 ************************************************/

//#include <string>
//#include <iostream>

#include "CAIComponent.h"
#include "..\..\CObject.h"
#include "..\..\Managers\Component Managers\CAIManager.h"
#include "..\..\Managers\Component Managers\CSpawningManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Input Manager\CInputManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\Level\CGoalItemComponent.h"
#include "..\..\Managers\Global Managers\Memory Manager\CMemoryManager.h"
#include "..\..\Managers\Component Managers\CCollisionManager.h"
using namespace EventStructs;

CAIComponent::CAIComponent(CObject* pObj) : m_pObject(pObj)
{
}

CAIComponent::~CAIComponent()
{
	while(m_cPath.empty() == false)
	{
		list<D3DXVECTOR3, CAllocator<D3DXVECTOR3>>::iterator pIter = 
			m_cPath.begin();

		MyDelete<D3DXVECTOR3>(&(*pIter), HEAPID_GENERAL);
		m_cPath.erase(m_cPath.begin());
	}

	while(m_tKnowledge.m_cGoalItemsInLevel.empty() == false)
	{
		list<TGoalItem, CAllocator<TGoalItem>>::iterator pIter =
			m_tKnowledge.m_cGoalItemsInLevel.begin();

		MyDelete<TGoalItem>(&(*pIter), HEAPID_GENERAL);
		
		m_tKnowledge.m_cGoalItemsInLevel.erase(
			m_tKnowledge.m_cGoalItemsInLevel.begin());
	}

	while(m_tKnowledge.m_cNeededGoalItems.empty() == false)
	{
		list<unsigned int, CAllocator<unsigned int>>::iterator pIter =
			m_tKnowledge.m_cNeededGoalItems.begin();

		MyDelete<unsigned int>(&(*pIter), HEAPID_GENERAL);

		m_tKnowledge.m_cNeededGoalItems.erase(
			m_tKnowledge.m_cNeededGoalItems.begin());
	}

	for(unsigned nIndex = 0; nIndex < 3; ++nIndex)
	{
		while(m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.empty() == false)
		{
			list<unsigned int, CAllocator<unsigned int>>::iterator pIter =
				m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin();

			MyDelete<unsigned int>(&(*pIter), HEAPID_GENERAL);

			m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.erase(
				m_tKnowledge.m_cOpponents[nIndex].m_cGoalItems.begin());
		}
	}
}

void CAIComponent::Init()
{
	//CAIManager* pcAIM = CAIManager::GetInstance();
	m_pfWeight[COLLECTIVE] = 0;
	m_pfWeight[AGGRESSIVE] = 0;
	m_pfWeight[DEFENSIVE] = 0;

	m_eCurrentState = NOT_INIT;

	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent(szEvent, this, Update);
	pEM->RegisterEvent("GoalItemInit", this, GoalItemInit);
	pEM->RegisterEvent("GoalItemCollected", this, GoalItemCollected);
	pEM->RegisterEvent("PlayerAttacked", this, PlayerAttacked);
	pEM->RegisterEvent("GoalItemSpawned", this, GoalItemSpawned);
	pEM->RegisterEvent("GoalItemDespawned", this, GoalItemDespawned);
	pEM->RegisterEvent("PlayerCreated", this, SetupOpponents);
	pEM->RegisterEvent("ItemDropped", this, ItemDropped);
	//pEM->RegisterEvent("Shutdown", this, Shutdown);

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

	m_fReactionDistance = 0.1f;
	m_fSideReactionDistance = 0.1f;

	m_bPathFound = false;
	//m_cPath.push_back(m_pObject->GetTransform()->GetWorldPosition());
	//m_vTargetPosition = m_pObject->GetTransform()->GetWorldPosition();
}

// Events Start
void CAIComponent::ItemDropped(IEvent* piEvent, IComponent* piComponent)
{
	// TODO : FIX - FUNCTION BREAKS
	return;
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemCollectedEvent* pcGoalItem = (TGoalItemCollectedEvent*)piEvent->GetData();

	if(pcAI->m_pObject->GetID() == pcGoalItem->m_pcCollector->GetID())
	{
		list<unsigned int, CAllocator<unsigned int>>::iterator ItemIter;
		ItemIter = pcAI->m_tKnowledge.m_cNeededGoalItems.begin();

		while(ItemIter != pcAI->m_tKnowledge.m_cNeededGoalItems.end())
		{
			if(*ItemIter == unsigned(pcGoalItem->m_pcGoalItem))
			{
				pcAI->m_tKnowledge.m_cNeededGoalItems.remove(*ItemIter);
				break;
			}

			++ItemIter;
		}
	}
	else
	{
		//for(int i = 0; i < 3; ++i)
		//{
		//	pcAI->m_tKnowledge.m_cOpponents[i].m_nPlayer;
		//}

		pcAI->m_tKnowledge.m_cOpponents[pcGoalItem->m_pcCollector->GetID()].m_chAmtGoalItems--;

		if(pcAI->WithinRadar(pcGoalItem->m_pcCollector->GetTransform()->GetWorldPosition()))
		{
			list<unsigned int, CAllocator<unsigned int>>::iterator ItemIter;
			ItemIter = pcAI->m_tKnowledge.m_cOpponents[pcGoalItem->
				m_pcCollector->GetID()].m_cGoalItems.begin();

			while(ItemIter != pcAI->m_tKnowledge.m_cOpponents[pcGoalItem->
				m_pcCollector->GetID()].m_cGoalItems.end())
			{
				if(*ItemIter == unsigned(pcGoalItem->m_pcGoalItem))
				{
					pcAI->m_tKnowledge.m_cOpponents[pcGoalItem->
						m_pcCollector->GetID()].m_cGoalItems.remove(*ItemIter);
					break;
				}

				++ItemIter;
			}
		}
	}
}

void CAIComponent::SetupOpponents(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TObjectEvent* pcObject = (TObjectEvent*)piEvent->GetData();

	// Previously in Manager
	std::list<CObject*,	CAllocator<CObject*>>::iterator pcPlayerIter = 
		CAIManager::GetInstance()->m_cPlayers.begin();
	bool bAlreadyAdded = false;

	while(pcPlayerIter != CAIManager::GetInstance()->m_cPlayers.end())
	{
		if(*pcPlayerIter == pcObject->m_pcObj)
		{
			bAlreadyAdded = true;
			break;
		}
		pcPlayerIter++;
	}

	if(bAlreadyAdded == false)
	{
		CAIManager::GetInstance()->m_cPlayers.push_back(pcObject->m_pcObj);
	}

	static int nPlayerID = 0;

	// Component part
	if(pcAI->m_pObject->GetID() != pcObject->m_pcObj->GetID())
	{
		pcAI->m_tKnowledge.m_cOpponents[pcAI->m_tKnowledge.m_nOpponentID].m_nPlayer =
			pcObject->m_pcObj->GetID();
		++pcAI->m_tKnowledge.m_nOpponentID;
	}

	++nPlayerID;
}

void CAIComponent::Update(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TUpdateStateEvent* pcUpdate = (TUpdateStateEvent*)piEvent->GetData();

	SendRenderEvent("AddToSet", pcAI, pcAI->m_pObject);

	pcAI->m_fTimer += pcUpdate->m_fDeltaTime;

	if(pcAI->GetDistance(pcAI->m_vTargetPosition,
		pcAI->m_pObject->GetTransform()->GetWorldPosition()) < 10.0f)
	{
		if(pcAI->m_cPath.size() >= 1)
		{
			pcAI->m_cPath.pop_front();
		}

		if(pcAI->m_cPath.size() >= 1)
		{
			pcAI->m_vTargetPosition = *(pcAI->m_cPath.begin());
		}
		else
		{
			pcAI->PathPlan(0.0f);
		}
	}

	if(pcAI->m_bPathFound == true)
	{
		pcAI->m_vTargetPosition = *(pcAI->m_cPath.begin());
		pcAI->m_bPathFound = false;
	}

	// AI Steering stuff
	// if we're in gameplay state (i.e. input isn't disabled)
	if(CInputManager::GetInstance()->GetState() == GAMEPLAY_STATE)
	{
		pcAI->SteerTowardTarget();
		pcAI->AvoidObstacles();
	}

	return;

	// Go to checkout behavior
	if(pcAI->m_tKnowledge.m_cNeededGoalItems.size() == 0)
	{
		pcAI->m_vTargetPosition = D3DXVECTOR3(-10, 0, -28);
		return;
	}

	// Don't do the rest of the logic every frame
	//if(pcAI->m_fTimer < 0.1f)
	//{
	//	return;
	//}

	pcAI->m_fTimer = 0;
	
	pcAI->EvaluateStateWeights();
	pcAI->Update();
}

void CAIComponent::Update()
{
	// This horrible thing is how you
	// call a member function pointer
	// that calls a member function
	//PathPlan(0.0f);
	//((*this).*(m_pfUpdateState))();
}

void CAIComponent::UpdateCollective()
{
	//EvaluateOpponentWeights();
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
				cPlayerPos = &GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);
			}
			else
			{
				cTempPlayerPos = GetOpponentPos(
					m_tKnowledge.m_cOpponents[CurOpponent].m_nPlayer);

				if(GetDistance(*cPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()) >
					GetDistance(cTempPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()))
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

				if(GetDistance(*cPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()) >
					GetDistance(cTempPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()))
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

D3DXVECTOR3 CAIComponent::GetOpponentPos(int nPlayerID)
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
	//return *OpponentIter;
}

void CAIComponent::GoalItemInit(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	pcAI->m_tKnowledge.m_cNeededGoalItems.push_back(pcGoalItem->m_eGoalItemType);
}

void CAIComponent::GoalItemCollected(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemCollectedEvent* pcCollected = 
		(TGoalItemCollectedEvent*)piEvent->GetData();

	list<TGoalItem, CAllocator<TGoalItem>>::iterator cGoalItemIter;
		cGoalItemIter = pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin();

	EGoalItemType eType = CSpawningManager::GetInstance()->
		GetGoalItemType(pcCollected->m_pcGoalItem);

	// Look for which player found it
	if(pcAI->m_pObject->GetID() != pcCollected->m_pcCollector->GetID())
	{
		for(unsigned nPlayer = 0; nPlayer < 3; ++nPlayer)
		{
			if(pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_nPlayer ==
				pcCollected->m_pcCollector->GetID())
			{
				bool bDupe = false;
				list<unsigned, CAllocator<unsigned>>::iterator ItemIter = 
					pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.begin();

				while(ItemIter != 
					pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.end())
				{
					if((*ItemIter) == static_cast<unsigned>(eType))
					{
						bDupe = true;
						break;
					}
					ItemIter++;
				}

				if(bDupe == false)
				{
					++pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_chAmtGoalItems;

					D3DXVECTOR3 cOpponentPos;
					cOpponentPos.x = pcCollected->m_pcCollector->
						GetTransform()->GetWorldMatrix()._41;
					cOpponentPos.z = pcCollected->m_pcCollector->
						GetTransform()->GetWorldMatrix()._43;

					// Look to see if it was in radar
					//if(WithinRadar(cOpponentPos))
					{
						pcAI->m_tKnowledge.m_cOpponents[nPlayer].m_cGoalItems.
							push_back(eType);
					}

					break;
				}
			}
		}
	}
	else // This AI collected the Goal Item
	{
		list<unsigned, CAllocator<unsigned>>::iterator cNeededIter = 
			pcAI->m_tKnowledge.m_cNeededGoalItems.begin();

		while(cNeededIter != pcAI->m_tKnowledge.m_cNeededGoalItems.end())
		{
			if((*cNeededIter) == static_cast<unsigned>(eType))
			{
				pcAI->m_tKnowledge.m_cNeededGoalItems.erase(cNeededIter);
				break;
			}

			cNeededIter++;
		}
	}

	// Remove Goal Item from list
	while(cGoalItemIter != pcAI->m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		if(cGoalItemIter->m_nGoalItem == static_cast<unsigned>(eType))
		{
			pcAI->m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
			return;
		}

		cGoalItemIter++;
	}
}

void CAIComponent::PlayerAttacked(IEvent*, IComponent*)
{
}

void CAIComponent::GoalItemSpawned(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	TGoalItem tGoalItem;
	tGoalItem.m_nGoalItem = pcGoalItem->m_pcGoalItem->m_eType;
	tGoalItem.m_pfWeight = 0;
	tGoalItem.m_cPos.x = pcGoalItem->m_pcGoalItem->GetParent()->GetTransform()->GetWorldMatrix()._41;
	tGoalItem.m_cPos.y = pcGoalItem->m_pcGoalItem->GetParent()->GetTransform()->GetWorldMatrix()._42;
	tGoalItem.m_cPos.z = pcGoalItem->m_pcGoalItem->GetParent()->GetTransform()->GetWorldMatrix()._43;
	pcAI->m_tKnowledge.m_cGoalItemsInLevel.push_back(tGoalItem);
}

void CAIComponent::GoalItemDespawned(IEvent* piEvent, IComponent* piComponent)
{
	CAIComponent* pcAI = (CAIComponent*)piComponent;
	TGoalItemEvent* pcGoalItem = (TGoalItemEvent*) piEvent->GetData();

	list<TGoalItem, CAllocator<TGoalItem>>::iterator cGoalItemIter;
	cGoalItemIter = pcAI->m_tKnowledge.m_cGoalItemsInLevel.begin();

	while(cGoalItemIter != pcAI->m_tKnowledge.m_cGoalItemsInLevel.end())
	{
		if(cGoalItemIter->m_nGoalItem == static_cast<unsigned>(pcGoalItem->m_pcGoalItem->m_eType))
		{
			pcAI->m_tKnowledge.m_cGoalItemsInLevel.erase(cGoalItemIter);
			return;
		}

		cGoalItemIter++;
	}
}
// Events End

void CAIComponent::EvaluateStateWeights()
{
	//CAIManager* pcAIM = CAIManager::GetInstance();
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
	//CAIManager* pcAIM = CAIManager::GetInstance();
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
				ItemIter->m_pfWeight = (MyWeight * 3.0f);

				// This will loop through each opponent in your minimap
				// and find out their weight and then will subtract
				// from each of the item's weights
				list<CObject*,	CAllocator<CObject*>>::iterator iter;
				iter = CAIManager::GetInstance()->m_cPlayers.begin();

				D3DXVECTOR3 OpponentPos;
				D3DXVECTOR3 OpponentHeading;

				for(unsigned nCurPlayer = 0; nCurPlayer < 2; ++nCurPlayer)
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

								ItemIter->m_pfWeight -= (OpponentWeight);// / 3);

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

				if(GetDistance(*cPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()) >
					GetDistance(cTempPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()))
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

				if(GetDistance(*cPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()) >
					GetDistance(cTempPlayerPos, 
					m_pObject->GetTransform()->GetWorldPosition()))
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

bool CAIComponent::WithinRadar(const D3DXVECTOR3& cPos) const
{
	D3DXVECTOR3 cMe;
	cMe.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
	cMe.z = m_pObject->GetTransform()->GetWorldMatrix()._43;
	
	float x = cPos.x - cMe.x;
	float z = cPos.z - cMe.z;

	return ( (x * x) + (z * z) ) < MINIMAP_RADIUS;
}

void CAIComponent::PathPlan(const float fTimeSlice)
{
	//float fTime = fTimeSlice;
	//m_bPathFound = false;
	//
	////DWORD dwStartTime = 0;
	////DWORD dwCurrentTime = 0;
	////DWORD dwPreviousTime = 0;

	//// Open set
	//set<CVertex*, TNodeCmp, CAllocator<CVertex*>>::iterator tCurrentTile;
	//
	//// HACKING
	//m_vTargetPosition = D3DXVECTOR3(50, 1, 46);

	//// Put nearest node into open list.
	//// Add that node to created list.

	//m_cOpen.insert(*(CAIManager::GetInstance()->m_cWaypointNodes.begin()));

	//while(m_cOpen.empty() == false)
	//{
	//	//dwStartTime = GetTickCount();
	//	//dwCurrentTime = (dwStartTime - dwPreviousTime) / 10;
	//	//if(dwCurrentTime < 1)
	//	//	dwCurrentTime = 1;
	//	//dwPreviousTime = dwStartTime;
	//	//fTime -= dwCurrentTime;

	//	tCurrentTile = m_cOpen.begin();
	//	CVertex* pcVertex = (*tCurrentTile);

	//	m_cOpen.erase(*tCurrentTile);

	//	// Close enough
	//	float dist = GetDistance(pcVertex->m_cLocation, m_vTargetPosition);
	//	if(dist <= 100)
	//	{
	//		// We're there!!
	//		// Push nodes' location's into the list
	//		while(pcVertex->m_pcParent != NULL)
	//		{
	//			m_cPath.push_front(pcVertex->m_cLocation);
	//			pcVertex = pcVertex->m_pcParent;
	//		}
	//		m_cPath.push_front(pcVertex->m_cLocation);

	//		m_bPathFound = true;

	//		while(m_cOpen.empty() == false)
	//		{
	//			m_cOpen.erase(m_cOpen.begin());
	//		}
	//		while(m_cCreated.empty() == false)
	//		{
	//			m_cCreated.erase(m_cCreated.begin());
	//		}

	//		return;
	//	}

	//	if(fTime < 0)
	//	{
	//		return;
	//	}

	//	// Add all of the current tile's neighbors to the open list
	//	map<CVertex*, CVertex*, less<CEdge*>, CAllocator<
	//		pair<CVertex*, CVertex*>>>::iterator pcIter = 
	//		pcVertex->m_pcConnections.begin();

	//	map<CEdge*, CVertex*, less<CEdge*>, CAllocator<
	//		pair<CEdge*, CVertex*>>>::iterator pcCreatedIter;

	//	while(pcEdgeIter != pcVertex->m_pcConnections.end())
	//	{
	//		pcCreatedIter = m_cCreated.find((*pcEdgeIter));

	//		(*pcEdgeIter)->m_dFinalCost = pcVertex->m_dGivenCost + 
	//			sqrtf(GetDistance((*pcEdgeIter)->m_pcEndpoint->m_cLocation, 
	//			m_vTargetPosition)) * 1.2f;
	//		
	//		if(pcCreatedIter != m_cCreated.end())
	//		{
	//			// It's in the created list.
	//			// However we should check cost
	//			// to see if the new one is better

	//			//if(iter->second->m_Cost > cost)
	//			//{
	//			//	m_Open.remove(iter->second);
	//			//	iter->second->m_Cost = cost;
	//			//	iter->second->m_Parent = node;
	//			//	iter->second->m_FinalCost = cost + iter->second->m_Heuristic;
	//			//	m_Open.push(iter->second);
	//			//}
	//		}
	//		else
	//		{
	//			// It's not in the list
	//			// so lets add it.
	//			(*pcEdgeIter)->m_pcEndpoint->m_dGivenCost = (*pcEdgeIter)->m_dFinalCost;
	//			(*pcEdgeIter)->m_pcEndpoint->m_pcParent = pcVertex;
	//			
	//			//m_cCreated[(*pcEdgeIter)] = (*pcEdgeIter)->m_pcEndpoint;
	//			m_cCreated.insert(make_pair((*pcEdgeIter), (*pcEdgeIter)->m_pcEndpoint));

	//			m_cOpen.insert(((*pcEdgeIter)->m_pcEndpoint));
	//		}

	//		pcEdgeIter++;
	//	}
	//}
}

CVertex* CAIComponent::GetClosestWaypoint(D3DXVECTOR3& cPos)
{
	std::list<CVertex*, CAllocator<CVertex*>>::iterator pcIter =
		CAIManager::GetInstance()->m_cWaypointNodes.begin();

	while(pcIter != CAIManager::GetInstance()->m_cWaypointNodes.end())
	{
		++pcIter;	
	}

	return *(CAIManager::GetInstance()->m_cWaypointNodes.begin());
}

float CAIComponent::GetDistance(const D3DXVECTOR3 &cPos1, const D3DXVECTOR3 &cPos2) const
{
	float x = cPos2.x - cPos1.x;
	float z = cPos2.z - cPos1.z;
	return ( (x * x) + (z * z) );
	
}

void CAIComponent::SteerTowardTarget()
{
	D3DXVECTOR3 vObjPos;
	vObjPos.x = m_pObject->GetTransform()->GetWorldMatrix()._41;
	vObjPos.y = m_pObject->GetTransform()->GetWorldMatrix()._42;
	vObjPos.z = m_pObject->GetTransform()->GetWorldMatrix()._43;

	D3DXVECTOR3 vObjHeading;
	vObjHeading.x = m_pObject->GetTransform()->GetWorldMatrix()._31;
	vObjHeading.y = m_pObject->GetTransform()->GetWorldMatrix()._32;
	vObjHeading.z = m_pObject->GetTransform()->GetWorldMatrix()._33;
	D3DXVec3Normalize(&vObjHeading, &vObjHeading);
	
	D3DXVECTOR3 vTarget;
	if(m_nFramesToAvoidObstacle <= 0) // we're going toward target object
	{
		vTarget = m_vTargetPosition;
	}
	else // we're going toward the avoidance target
	{
		m_vTargetPosition = m_vAvoidanceTargetPosition;
	}
	m_nFramesToAvoidObstacle--;

	D3DXVECTOR3 desiredDirection = vTarget - vObjPos;  // from us to target
	//float distanceToTarget = D3DXVec3Length(&desiredDirection);
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
	//D3DXVec3Normalize(&rightDir, &rightDir);

	//the difference between the current velocity and the desired velocity 
	// Positive is turn right. Negative is turn left
	float turnDir = D3DXVec3Dot(&rightDir, &desiredDirection); 

	// TODO: Drifting logic
	// if the distance < someValue && the angle between heading and desiredDirection is high enough to warrant drifting
	//		turn on drifting so we can turn more tightly

	if(turnDir >= 0.0f) // on right side
	{
		// send out simulated input to the agent's object
		SendInputEvent("SteerRight", this, m_nControllerNumber, 1.0f);
	}
	else	// on left side
	{
		// send out simulated input to the agent's object
		SendInputEvent("SteerLeft", this, m_nControllerNumber, 1.0f);
	}
}

void CAIComponent::AvoidObstacles()
{
	// Handle figuring out whether to accelerate or decelerate:
	// HACK: Move forward based on distance

	if(m_nFramesToAccel <= 2)
	{
		SendInputEvent("Accelerate", this, m_nControllerNumber, 1.0f);
	}
	else if(m_nFramesToAccel >= 3)
	{
		SendInputEvent("Decelerate", this, m_nControllerNumber, 1.0f);
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
	//float distanceToTarget = D3DXVec3Length(&desiredDirection);
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
	m_pvReactionFeelers[0].x = m_pObject->GetTransform()->GetLocalMatrix()._31;
	m_pvReactionFeelers[0].y = m_pObject->GetTransform()->GetLocalMatrix()._32;
	m_pvReactionFeelers[0].z = m_pObject->GetTransform()->GetLocalMatrix()._33;
	D3DXVec3Normalize(&m_pvReactionFeelers[0], &m_pvReactionFeelers[0]);

	// generate a right vector
	D3DXVECTOR3 vRightDirection;
	D3DXVec3Cross(&vRightDirection, &D3DXVECTOR3(0.0f, -1.0f, 0.0f), 
		&m_pvReactionFeelers[0]); // [0] is fwd heading
	D3DXVec3Normalize(&vRightDirection, &vRightDirection);
	// scale the front feeler
	D3DXVec3Scale(&m_pvReactionFeelers[0], &m_pvReactionFeelers[0], m_fReactionDistance);

	// Side feelers
	m_pvReactionFeelers[1] = m_pvReactionFeelers[0] + vRightDirection;
	m_pvReactionFeelers[2] = m_pvReactionFeelers[0] - vRightDirection;
	D3DXVec3Normalize(&m_pvReactionFeelers[1], &m_pvReactionFeelers[1]);
	D3DXVec3Normalize(&m_pvReactionFeelers[2], &m_pvReactionFeelers[2]);

	// scale side feelers
	D3DXVec3Scale(&m_pvReactionFeelers[1], &m_pvReactionFeelers[1], m_fSideReactionDistance);
	D3DXVec3Scale(&m_pvReactionFeelers[2], &m_pvReactionFeelers[2], m_fSideReactionDistance);

	m_vAvoidanceTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	/*
	for(int curFeeler=0; curFeeler<3; curFeeler++)
	{
		// check feelers against nearby geometry
		TLine tLine;
		tLine.cLineStart = m_pObject->GetTransform()->GetWorldPosition();
		tLine.cLineEnd = tLine.cLineStart + m_pvReactionFeelers[curFeeler];
		D3DXVECTOR3 vClosestPt;
		bool bCollided = true;//CCollisionManager::GetInstance()->LineToWayPoint(tLine, vClosestPt);

		if(!bCollided)
		{
			// grab collision normal (emulated with from col point to us)
			D3DXVECTOR3 vColNormal = m_pObject->GetTransform()->GetWorldPosition() - vClosestPt;
			// set avoidance target to our pos + col normal
			m_vAvoidanceTargetPosition += m_pObject->GetTransform()->GetWorldPosition() + vColNormal;
			// m_nAvoidingObstacle = how long we wanna turn away from it
			m_nFramesToAvoidObstacle = 1;
		}
		// if(no collision) consider distance too if ray collision
				// continue
	}/**/
}