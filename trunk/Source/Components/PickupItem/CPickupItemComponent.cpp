#include "CPickupItemComponent.h"

#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;

#include "..\..\CObject.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

CPickupItemComponent::CPickupItemComponent(CObject* pObj) : m_pObject(pObj)
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
	
	CPickupItemComponent* pSOR = CreatePickupItemComponent(pObj, vDir);

	lua_pop(pLua, 4);
	return 0;
}

CPickupItemComponent* CPickupItemComponent::CreatePickupItemComponent(CObject* pObj, D3DXVECTOR3 vDirection)
{
	CPickupItemComponent* comp = MMNEW(CPickupItemComponent(pObj));
	
	comp->Init();
	comp->m_vDirectionToMove = vDirection;
	if(D3DXVec3LengthSq(&comp->m_vDirectionToMove) == 0.0f)
	{
		comp->m_vDirectionToMove.x = pObj->GetTransform()->GetLocalMatrix()._31;
		comp->m_vDirectionToMove.y = pObj->GetTransform()->GetLocalMatrix()._32;
		comp->m_vDirectionToMove.z = pObj->GetTransform()->GetLocalMatrix()._33;

		comp->m_vDirectionToMove *= -1.0f;
	}
	else // it has a value
	{
		D3DXVec3Normalize(&comp->m_vDirectionToMove, &comp->m_vDirectionToMove);
	}

	pObj->AddComponent(comp);

	return comp;
}
//	//Lua Stuff

void CPickupItemComponent::Init()
{
	// listens for these events
	string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);
	CEventManager::GetInstance()->RegisterEvent("GoalItemCollected", this, GoalItemCollectionCallback);

	m_fLifetime = 10.0f;
	m_fTimeLeft = 10.0f;
	m_fFlyOutSpeed = 8.0f;
	m_fTimeToFly = 1.0f;
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

	// HACK: BVH will handle this once implemented
	SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_NORMAL);	
}



void CPickupItemComponent::DestroyPickupItem(CPickupItemComponent* toDestroy)
{
	//toDestroy->m_pObject->RemoveComponent
}


void CPickupItemComponent::GoalItemCollectionCallback(IEvent* cEvent, IComponent* cCenter)
{
	CPickupItemComponent* pComp = (CPickupItemComponent*)cCenter;
	TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)cEvent->GetData();

	// if we were the ones collected
	if(tEvent->m_pcGoalItem == pComp->m_pObject || tEvent->m_pcCollector == pComp->m_pObject)
	{
		// destroy us
		DestroyPickupItem(pComp);
	}
}