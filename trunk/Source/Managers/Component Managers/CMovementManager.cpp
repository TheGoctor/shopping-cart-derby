#include "CMovementManager.h"
#include "..\..\CObject.h"

#include "..\..\Components\Movement\CMovement.h"

#include "..\Global Managers\Memory Manager\CMemoryManager.h"
#include "CAIManager.h"

#include "..\..\Managers\Global Managers\Event Manager\CMoveCameraEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CInputEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CHeadingEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CWeightClassEvent.h"
#include "..\..\Managers\Global Managers\Event Manager\CObjectEvent.h"
#include "..\Global Managers\Event Manager\IEvent.h"

CMovementManager::CMovementManager()
{
	//void (*funcptr)(IEvent*, IComponent*) = &CMovement::Update;

	CEventManager::GetInstance()->RegisterEvent("UpdateState", NULL, Update);
	//CEventManager::GetInstance()->RegisterEvent("Accel", NULL, funcmoo);
	//CEventManager::GetInstance()->RegisterEvent("Decel", NULL, funcmoo);
	
	// HACK: Not correct input values
	CEventManager::GetInstance()->RegisterEvent("Accelerate", NULL, HandleInputAccelerate);
	CEventManager::GetInstance()->RegisterEvent("Decelerate", NULL, HandleInputBrake);
	CEventManager::GetInstance()->RegisterEvent("SteerLeft", NULL, HandleInputLeft);
	CEventManager::GetInstance()->RegisterEvent("SteerRight", NULL, HandleInputRight);
	CEventManager::GetInstance()->RegisterEvent("Boost", NULL, HandleBoost);
	// END



	//CEventManager::GetInstance()->RegisterEvent("SendWeightClass", NULL, funcmoo);

	//szEventName = "RegisterPlayer";
	////szEventName += idstr;
	//CEventManager::GetInstance()->RegisterEvent(szEventName.c_str(), NULL, funcmoo);

	//szEventName = "SendSteeringHeading";
	////szEventName += idstr;
	//CEventManager::GetInstance()->RegisterEvent(szEventName.c_str(), NULL, funcmoo);
}

int CMovementManager::CreateMovementComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, 1);
	lua_pop(pLua, 1);
	CMovement* pMove = CreateMovementComponent(pObj);
	return 0;
}

CMovement* CMovementManager::CreateMovementComponent(CObject* pObj)
{
	static int nNumCreated = 0;

	CMovement* comp = MMNEW(CMovement) CMovement();
	
	comp->SetObject(pObj);
	comp->SetPlayerNumber(nNumCreated);
	comp->Init();
	pObj->AddComponent(comp);


	// Let anyone who wants to know that a player exists (since only players have movement)
	CObjectEvent* pObjectEvent = (CObjectEvent*)CMemoryManager::GetInstance()
			->Allocate(sizeof(CObjectEvent), HEAPID_EVENT);
		(*pObjectEvent) = CObjectEvent(CIDGen::GetInstance()->GetID(
			"PlayerCreated"), NULL, pObj);
		// only sending pos here since that's all they need
	CEventManager::GetInstance()->PostEvent(pObjectEvent, PRIORITY_NORMAL);

	CMovementManager::GetInstance()->m_cMovements.insert(make_pair((unsigned int)
		pObj->GetID(), comp));


	// if this is the first one we've made, attach the camera
	if(nNumCreated == 0)
	{
		D3DXVECTOR3 vObjPos;
		vObjPos.x = pObj->GetTransform()->GetWorldMatrix()._41;
		vObjPos.y = pObj->GetTransform()->GetWorldMatrix()._42;
		vObjPos.z = pObj->GetTransform()->GetWorldMatrix()._43;

		// Send pertinant data to Camera through event
		// Vel, pos, turn speed
		CMoveCameraEvent* pMoveCameraEvent = (CMoveCameraEvent*)CMemoryManager::GetInstance()
			->Allocate(sizeof(CMoveCameraEvent), HEAPID_EVENT);
		(*pMoveCameraEvent) = CMoveCameraEvent(CIDGen::GetInstance()->GetID(
			"AttachToCamera"), NULL, vObjPos, 0.0f, 0.0f);
		// only sending pos here since that's all they need
		CEventManager::GetInstance()->PostEvent(pMoveCameraEvent, PRIORITY_IMMEDIATE);

	}
	nNumCreated++;

	return comp;
}

void CMovementManager::AccelerateCart()
{
	
}

void CMovementManager::BrakeCart()
{
	
}

void CMovementManager::GoRight()
{
	
}

void CMovementManager::GoLeft()
{
	
}


void CMovementManager::SendUpdates(IEvent* cEvent)
{
	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int,
		CMovement*>>>::iterator iter = m_cMovements.begin();

	while(iter != m_cMovements.end())
	{
		(*iter).second->Update(cEvent, NULL);
		++iter;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Callback Functions
//
///////////////////////////////////////////////////////////////////////////////
void CMovementManager::HandleInputBrake(IEvent* cEvent, IComponent* cCenter)
{
	CInputEvent* eEvent = (CInputEvent*)cEvent;

	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int,
	CMovement*>>>::iterator iter = GetInstance()->m_cMovements.begin();

	while(iter != GetInstance()->m_cMovements.end())
	{
		if((*iter).second->GetPlayerNumber() == eEvent->GetPlayer())
		{
			(*iter).second->HandleInput(BRAKE);
		}
		++iter;
	}
}

void CMovementManager::HandleInputAccelerate(IEvent* cEvent, IComponent* cCenter)
{
	CInputEvent* eEvent = (CInputEvent*)cEvent;

	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
		CMovement*>>>::iterator iter = GetInstance()->m_cMovements.begin();

	while(iter != GetInstance()->m_cMovements.end())
	{
		if((*iter).second->GetPlayerNumber() == eEvent->GetPlayer())
		{
			(*iter).second->HandleInput(ACCELERATE);
		}
		++iter;

	}
}

void CMovementManager::Update(IEvent* cEvent, IComponent* cCenter)
{
	CMovementManager::GetInstance()->SendUpdates(cEvent);
}

void CMovementManager::HandleInputLeft(IEvent* cEvent, IComponent* cCenter)
{
	CInputEvent* eEvent = (CInputEvent*)cEvent;

	if(eEvent->GetPlayer() == 1)
	{
		int x = 5;
	}

	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
		CMovement*>>>::iterator iter = GetInstance()->m_cMovements.begin();

	while(iter != GetInstance()->m_cMovements.end())
	{
		if((*iter).second->GetPlayerNumber() == eEvent->GetPlayer())
		{
			(*iter).second->HandleInput(GOLEFT);
		}
		++iter;
	}
}

void CMovementManager::HandleInputRight(IEvent* cEvent, IComponent* cCenter)
{
		CInputEvent* eEvent = (CInputEvent*)cEvent;

	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
		CMovement*>>>::iterator iter = GetInstance()->m_cMovements.begin();

	while(iter != GetInstance()->m_cMovements.end())
	{
		if((*iter).second->GetPlayerNumber() == eEvent->GetPlayer())
		{
			(*iter).second->HandleInput(GORIGHT);
		}
	
		++iter;
	}

}


void CMovementManager::HandleBoost(IEvent* cEvent, IComponent* cCenter)
{
	CInputEvent* eEvent = (CInputEvent*)cEvent;

	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
		CMovement*>>>::iterator iter = GetInstance()->m_cMovements.begin();

	while(iter != GetInstance()->m_cMovements.end())
	{
		if((*iter).second->GetPlayerNumber() == eEvent->GetPlayer())
		{
			(*iter).second->Boost();;
		}
		++iter;

	}
}


void CMovementManager::HandleCollision(IEvent* cEvent, IComponent* cCenter)
{
	// TODO: Make Collision event
	//CInputEvent* eEvent = (CCollisionEvent*)cEvent;

	// HACK
	std::map<unsigned int, CMovement*, less<unsigned int>, CAllocator<pair<unsigned int, 
		CMovement*>>>::iterator iter = GetInstance()->m_cMovements.begin();

	while(iter != GetInstance()->m_cMovements.end())
	{
		//if((*iter).second->GetPlayerNumber() == eEvent->GetPlayer())
		{
			(*iter).second->HandleCollision(cEvent, cCenter);
		}
	
		++iter;
	}

}


