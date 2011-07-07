#include "CBanana.h"
#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\..\CObject.h"
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"
#include "..\..\..\Managers\Component Managers\CCollisionManager.h"
using namespace EventStructs;
CWwiseSoundManager* m_pSound = CWwiseSoundManager::GetInstance();

CBanana* CBanana::CreateBananaComponent(CObject* pObj) 
{
	CBanana* pComp = MMNEW(CBanana(pObj));
	pComp->FirstInit();
	pObj->AddComponent(pComp);
	return pComp;
}
void CBanana::FirstInit()
{
	std::string szEventName = "Update";
	BANANA_ID = 0;
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
//	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldCollision", this, environment)
	CEventManager::GetInstance()->RegisterEvent("HeldItemInWorldPlayerCollision", this, PlayerCollision);

	
	szEventName = "Update";
	szEventName += PAUSE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
	szEventName = "Update";
	szEventName += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);

	szEventName = "Update";
	szEventName += PAUSE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
	szEventName = "Update";
	szEventName += CONSOLE_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, PauseUpdateCallback);
}
void CBanana::ReInit()
{
	m_bIsSpawned = true;
	BANANA_ID = m_pSound->RegisterHeldObject();
	TSphere sphere;
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CREAMPIE_USE, BANANA_ID);
}
void CBanana::Update(IEvent* cEvent, IComponent* cCenter)
{
	CBanana* pComp = (CBanana*)cCenter;
	/*TUpdateStateEvent* eEvent = */
	(TUpdateStateEvent*)cEvent->GetData();
	if(!pComp->m_bIsSpawned)
	{
		return;
	}
	m_pSound->SetObjectPosition(pComp->BANANA_ID,	
		pComp->m_pObject->GetTransform()->GetWorldPosition(), 0.25f);
	SendRenderEvent("AddToSet", pComp, pComp->m_pObject, PRIORITY_IMMEDIATE);
}
void CBanana::Despawn()
{
	m_bIsSpawned = false;
	m_pObject->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	m_pObject->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	m_pObject->GetTransform()->GetLocalMatrix()._43 = 300.0f;
	m_pSound->UnregisterObject(BANANA_ID);
}
void CBanana::PlayerCollision(IEvent* cEvent, IComponent* cCenter)
{
	CBanana* pComp = (CBanana*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
	if(pComp->m_pObject == tEvent->m_pCollidedWith && pComp->m_bIsSpawned)
	{
		SendStatusEffectEvent("Slip", pComp, tEvent->m_pcCollider, SLOW_TIME);
		m_pSound->PlayTheSound(BULLDOG_BIKE_SLIP, pComp->BANANA_ID);
		pComp->Despawn();
	}
}


 

void CBanana::PauseUpdateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CBanana* comp = (CBanana*)pComp;
	
	if(comp->m_bIsSpawned)
	{
		SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_UPDATE);
	}
	
}


