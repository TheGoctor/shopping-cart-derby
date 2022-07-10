#include "CMovementManager.h"
#include "..\..\Components\Movement\CMovement.h"
#include "..\..\CObject.h"

#include "..\Global Managers\Event Manager\EventStructs.h"
#include "..\Component Managers\CMovementManager.h"
using namespace EventStructs;

CMovementManager::CMovementManager()
{
	CEventManager::GetInstance()->RegisterEvent("Shutdown", (IComponent*)GetInstance(), Shutdown);
}

void CMovementManager::Shutdown(IEvent*, IComponent*)
{
	CMovementManager* pM = GetInstance();
	std::map<unsigned int, CMovement*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CMovement*>>>::iterator pIter;
	pIter = pM->m_cMovements.begin();
	while(pIter != pM->m_cMovements.end())
	{
		MMDEL(pIter->second);
		++pIter;
	}
	pM->m_cMovements.clear();
}
bool CMovementManager::PlayerIsInvincible(CObject* pPlayer)
	{
		std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
			CMovement*>>>::iterator pPlayerIter = m_cMovements.find(pPlayer->GetID());
		return (*pPlayerIter).second->GetPlayerInvulnerable();
	}
float CMovementManager::GetPlayerSpeed(CObject* pPlayer)
{
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
		CMovement*>>>::iterator pPlayerIter = m_cMovements.find(pPlayer->GetID());
	return (*pPlayerIter).second->GetPlayerSpeed();
}

int CMovementManager::CreateMovementComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 1);
	CreateMovementComponent(pObj);
	return 0;
}


int CMovementManager::SetCartWeight(lua_State* pLua)
{
	int nPlayerNum = lua_tointeger(pLua, -2);
	int nWeight = lua_tointeger(pLua, -1);

	std::map<unsigned int, CMovement*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CMovement*>>>::iterator iter;
	iter = GetInstance()->m_cMovements.begin();

	// loop through our movement comps
	while(iter != GetInstance()->m_cMovements.end())
	{
		// if we found the player we want to set
		if(iter->second->GetPlayerNumber() == nPlayerNum)
		{
			// use that object* in the event
			SendWeightClassEvent("SetWeight", (IComponent*)GetInstance(), iter->second->GetObject(), (ECartWeight)nWeight);
			break;
		}

		iter++;
	}


	lua_pop(pLua, 2);
	return 0;
}

CMovement* CMovementManager::CreateMovementComponent(CObject* pObj)
{
	static int nNumCreated = 0;

	CMovement* comp = MMNEW(CMovement);
	
	comp->SetObject(pObj);
	comp->SetPlayerNumber(nNumCreated);
	comp->Init();
	pObj->AddComponent(comp);

	// Let anyone who wants to know that a player exists (since only players have movement)
	SendObjectEvent("PlayerCreated", (IComponent*)GetInstance(), pObj);

	if(nNumCreated == 0)
	{
		SendObjectEvent("Player1ObjectCreation", (IComponent*)GetInstance(), pObj);
	}


	CMovementManager::GetInstance()->m_cMovements.insert(make_pair((unsigned int)
		pObj->GetID(), comp));

	// if this is the first one we've made, attach the camera
	if(nNumCreated == 0)
	{
		SendObjectEvent("AttachToCamera", (IComponent*)GetInstance(), pObj,
			PRIORITY_IMMEDIATE);
	}
	nNumCreated++;

	return comp;
}
 

CMovement* CMovementManager::GetMovementComponent(CObject* pOwner)
{
	CMovementManager* mgr = GetInstance();

	std::map<unsigned int, CMovement*, less<unsigned int>, 
		CAllocator<pair<unsigned int, CMovement*>>>::iterator iter;
	iter = mgr->m_cMovements.begin();

	// iterate through the list of movement comps
	while(iter != mgr->m_cMovements.end())
	{
		// if the current comp'sobject is the param's object
		if(pOwner == iter->second->GetObject())
		{
			return iter->second;
		}

		iter++;
	}

	// didn't find in the list
	return NULL;
}
