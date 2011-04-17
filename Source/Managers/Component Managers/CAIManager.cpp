#include <iostream>

#include "CAIManager.h"
#include "..\..\CObject.h"
#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\Global Managers\Event Manager\CUpdateStateEvent.h"
#include "..\Global Managers\Event Manager\CGoalItemCollected.h"
#include "..\Global Managers\Event Manager\CObjectEvent.h"
#include "..\Global Managers\State Manager\CStateManager.h"
#include "..\Global Managers\Event Manager\CGoalItemEvent.h"
#include "..\..\Components\AI\CAIComponent.h"
#include "..\..\Components\Level\CGoalItemComponent.h"

CAIManager::CAIManager()
{
}

void CAIManager::Init()
{
	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;

	CEventManager* pEM = CEventManager::GetInstance();
	pEM->RegisterEvent("GoalItemInit", NULL, GoalItemInit);
	pEM->RegisterEvent("GoalItemCollected", NULL, GoalItemCollected);
	pEM->RegisterEvent("PlayerAttacked", NULL, PlayerAttacked);
	pEM->RegisterEvent("GoalItemSpawned", NULL, GoalItemSpawned);
	pEM->RegisterEvent("GoalItemDespawned", NULL, GoalItemDespawned);
	pEM->RegisterEvent(szEvent, NULL, Update);
	//pEM->RegisterEvent("UpdateState", NULL, Update);
	pEM->RegisterEvent("Shutdown", NULL, Shutdown);
	pEM->RegisterEvent("PlayerCreated", NULL, PlayerCreated);
}

int CAIManager::CreateAIComponent(lua_State* pLua)
{
	// Function is only called by LUA.
	// DON'T EVER CALL THIS FUNCTION YOURSELF!!!
	// EVER!!!!
	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	lua_pop(pLua, 1);
	CAIComponent* pAI = CreateAIComponent(pObj);
	
	return 0;
	// DON'T DO IT!
	// IT'S NOT WORTH IT!
}

CAIComponent* CAIManager::CreateAIComponent(CObject* pObj)
{
	// vv Pseudo-code vv
	// CAIComponent* comp = new CAIComponent(pObj);
	// ^^ Pseudo-code ^^
	CAIComponent* comp = MMNEW(CAIComponent)CAIComponent(pObj);

	static int nNumCreated = 1;
	
	comp->Init();
	comp->SetControllerNumber(nNumCreated); // starts at 0, but that's player
	pObj->AddComponent(comp);

	CAIManager::GetInstance()->m_cAIComponents.insert(make_pair((unsigned int)
		pObj->GetID(), comp));

	nNumCreated++;

	return comp;
}

// Events Start
void CAIManager::Update(IEvent* iEvent, IComponent*)
{
	std::map<unsigned int, CAIComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator iter;
	iter = GetInstance()->m_cAIComponents.begin();

	CUpdateStateEvent* cUpdateEvent = (CUpdateStateEvent*)iEvent;

	while(iter != GetInstance()->m_cAIComponents.end())
	{
		if(iter->second)
		{
			iter->second->Update(cUpdateEvent->GetDeltaTime());
		}
		iter++;
	}
}

void CAIManager::GoalItemInit(IEvent* iEvent, IComponent*)
{
	std::map<unsigned int, CAIComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator iter;
	iter = GetInstance()->m_cAIComponents.begin();

	CGoalItemEvent* cGoalItemEvent = (CGoalItemEvent*)iEvent;

	while(iter != GetInstance()->m_cAIComponents.end())
	{
		if(iter->second)
		{
			iter->second->GoalItemInit(cGoalItemEvent->GetGoalItem()->m_pObject);
		}
		iter++;
	}
}

void CAIManager::PlayerCreated(IEvent* iEvent, IComponent*)
{
	CObjectEvent* cObjectEvent = (CObjectEvent*)iEvent;

	CAIManager::GetInstance()->m_cPlayers.push_back(cObjectEvent->GetObject());

	std::map<unsigned int, CAIComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator iter;
	iter = GetInstance()->m_cAIComponents.begin();

	static int nPlayerID = 0;

	while(iter != GetInstance()->m_cAIComponents.end())
	{
		if(iter->second)
		{
			iter->second->SetupOpponents(cObjectEvent->GetObject());
		}
		iter++;
	}

	++nPlayerID;
}

void CAIManager::GoalItemCollected(IEvent* iEvent, IComponent*)
{
	std::map<unsigned int, CAIComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator iter;
	iter = GetInstance()->m_cAIComponents.begin();

	CGoalItemCollectedEvent* cGoalItemCollected = (CGoalItemCollectedEvent*)iEvent;

	//cout << "Player " << cGoalItemCollected->GetCollectorType() << " collected " << cGoalItemCollected->GetGoalItem() << '\n';

	while(iter != GetInstance()->m_cAIComponents.end())
	{
		if(iter->second)
		{
			iter->second->GoalItemCollected(cGoalItemCollected->GetGoalItem(), cGoalItemCollected->GetCollector());
		}
		iter++;
	}
}

void CAIManager::PlayerAttacked(IEvent*, IComponent*)
{
}

void CAIManager::GoalItemSpawned(IEvent* iEvent, IComponent*)
{
	std::map<unsigned int, CAIComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator iter;
	iter = GetInstance()->m_cAIComponents.begin();

	CGoalItemEvent* cGoalItemEvent = (CGoalItemEvent*)iEvent;

	while(iter != GetInstance()->m_cAIComponents.end())
	{
		if(iter->second)
		{
			iter->second->GoalItemSpawned(cGoalItemEvent->GetGoalItem()->m_pObject);
		}
		iter++;
	}
}

void CAIManager::GoalItemDespawned(IEvent* iEvent, IComponent*)
{
	std::map<unsigned int, CAIComponent*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CAIComponent*>>>::iterator iter;
	iter = GetInstance()->m_cAIComponents.begin();

	CGoalItemEvent* cGoalItemEvent = (CGoalItemEvent*)iEvent;

	while(iter != GetInstance()->m_cAIComponents.end())
	{
		if(iter->second)
		{
			iter->second->GoalItemDespawned(cGoalItemEvent->GetGoalItem()->m_pObject);
		}
		iter++;
	}
}

void CAIManager::Shutdown(IEvent*, IComponent*)
{
}
// Events End