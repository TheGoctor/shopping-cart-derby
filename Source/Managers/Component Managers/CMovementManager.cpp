#include "CMovementManager.h"
#include "..\..\Components\Movement\CMovement.h"
#include "..\..\CObject.h"

#include "..\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

CMovementManager::CMovementManager()
{
}

int CMovementManager::CreateMovementComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -1);
	lua_pop(pLua, 1);
	CreateMovementComponent(pObj);
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
