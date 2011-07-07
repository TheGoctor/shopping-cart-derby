#include "CPickupItemComponent.h"

#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\Managers\Component Managers\CPickupItemManager.h"

CPickupItemComponent::CPickupItemComponent(CObject* pObj) : m_pObject(pObj), m_bSpawned(false)
{
}

// Lua stuff
// CreatePickupItemComponent(pObj, flyDir.x, flyDir.y, flyDir.z)
int CPickupItemComponent::CreatePickupItemComponent(lua_State* pLua)
{
	CObject* pObj = (CObject*)lua_topointer(pLua, -4);
	D3DXVECTOR3 vDir;
	vDir.x = (float)lua_tonumber(pLua, -3);
	vDir.y = (float)lua_tonumber(pLua, -2);
	vDir.z = (float)lua_tonumber(pLua, -1);
	
	/*CPickupItemComponent* pPickupItemComponent =*/ CreatePickupItemComponent(pObj, vDir);


	lua_pop(pLua, 4);
	return 0;
}

CPickupItemComponent* CPickupItemComponent::CreatePickupItemComponent(CObject* pObj, D3DXVECTOR3 vDirection)
{
	CPickupItemComponent* comp = MMNEW(CPickupItemComponent(pObj));
	
	comp->Init();
	comp->m_vDirectionToMove = vDirection;

	D3DXVec3Normalize(&comp->m_vDirectionToMove, &comp->m_vDirectionToMove);

	pObj->AddComponent(comp);
	CPickupItemManager::GetInstance()->AddComponent(comp);

	return comp;
}
//	//Lua Stuff

void CPickupItemComponent::EventInit()
{
	// listens for these events
	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);

	szEvent = "Update";
	szEvent += PAUSE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	szEvent = "Update";
	szEvent += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	szEvent = "Update";
	szEvent += PAUSE_OPTIONS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);
	szEvent = "Update";
	szEvent += PAUSE_KEYBINDS_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	CEventManager::GetInstance()->RegisterEvent("PickupItemCollision", this, PickupItemCollisionCallback);
}

void CPickupItemComponent::Init()
{
	m_fLifetime = 15.0f;
	m_fTimeLeft = 15.0f;
	m_fFlyOutSpeed = 8.0f;
	m_fTimeToFly = 1.0f;
	m_bSpawned = true;
	m_bRendering = true;
	//m_nItemType = -1;
	m_nItemType = NO_ITEM;
}

///////////////////////////
//
//	Callback Functions
//
///////////////////////////
void CPickupItemComponent::Update(IEvent* cEvent, IComponent* cCenter)
{
	CPickupItemComponent* comp = (CPickupItemComponent*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;

	// if we're not spawned
	if(!comp->m_bSpawned)
	{
		// dont do anything
		return;
	}

	comp->m_fTimeLeft -= fDt;
	if(comp->m_fTimeLeft <= 0.0f)
	{
		//TODO: destroy me sire!
		DestroyPickupItem(comp);
		return;
	}
	else if(!comp->IsReadyToCollide()) // if we're still flying
	{
		comp->m_pObject->GetTransform()->GetLocalMatrix()._41 += comp->m_vDirectionToMove.x*fDt*comp->m_fFlyOutSpeed;
		comp->m_pObject->GetTransform()->GetLocalMatrix()._43 += comp->m_vDirectionToMove.z*fDt*comp->m_fFlyOutSpeed;

	}

	// oscillate on the Y axis (bounce up and down)
	comp->m_pObject->GetTransform()->GetLocalMatrix()._42 = 1.0f + .5f*sin(comp->m_fTimeLeft);

	// flash code
	if(comp->m_bRendering)
	{
		// HACK: BVH will handle this once implemented
		SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_NORMAL);
	}
	if(comp->m_fTimeLeft < 5.0f)
	{
		comp->m_bRendering = !comp->m_bRendering;
	}

}



void CPickupItemComponent::DestroyPickupItem(CPickupItemComponent* toDestroy)
{
	CPickupItemManager::GetInstance()->DespawnPickupItem(toDestroy);
}


void CPickupItemComponent::PickupItemCollisionCallback(IEvent* cEvent, IComponent* cCenter)
{
	CPickupItemComponent* pComp = (CPickupItemComponent*)cCenter;
	TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)cEvent->GetData();

	// if we were the ones collected
	if(tEvent->m_pcGoalItem == pComp->m_pObject && pComp->IsReadyToCollide())
	{
		// tell the collector they collected a goal item
		SendPickupItemCollectedEvent("PickupItemCollected", pComp, pComp->GetObject(), tEvent->m_pcCollector, (EGoalItemType)pComp->m_nItemType);

		// destroy us
		DestroyPickupItem(pComp);
	}
}


void CPickupItemComponent::PauseUpdateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CPickupItemComponent* comp = (CPickupItemComponent*)pComp;

	if(comp->m_bSpawned)
	{
		SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_UPDATE);
	}	
}


