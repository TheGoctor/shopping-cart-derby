#include "CChickenSoup.h"
#include "Managers\Global Managers\Event Manager\CEventManager.h"
#include "Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "Managers\Global Managers\Object Manager\scd::objectManager.h"
#include "Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "Managers\Component Managers\CHeldItemManager.h"


CChickenSoupComponent* CChickenSoupComponent::CreateChickenSoupComponent(scd::object* pObj)
{
	CChickenSoupComponent* pComp = MMNEW(CChickenSoupComponent(pObj));
	pComp->FirstInit();
	pComp->CHICKENSOUP_ID = -1;
	pObj->AddComponent(pComp);

	return pComp;
}
void CChickenSoupComponent::FirstInit(void)
{
	m_fDuration = 10.0f;
	m_fTimeRemaining = m_fDuration;
	CHICKENSOUP_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	//(event)registration forms!
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);

}
void CChickenSoupComponent::ReInit()
{
	SetIsSpawned(true);
	SetTimeRemaining(GetDuration());
	CHICKENSOUP_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	// tell movement that it's invulnerable (since it has invulnerable checks in all its status effects)
	SendStatusEffectEvent("Invulnerable", this, m_pAttachedObject, m_fDuration);
	CWwiseSoundManager::GetInstance()->SetObjectPosition(CHICKENSOUP_ID, m_pAttachedObject->GetTransform()->GetWorldPosition(), 0.3f);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_CHICKENSOUP_USE, CHICKENSOUP_ID);
}
//call backs
void CChickenSoupComponent::Update(IEvent* cEvent, scd::base_component* cCenter)
{
	CChickenSoupComponent* pComp = (CChickenSoupComponent*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;
	
	if(!pComp->GetIsSpawned())
	{
		//not being used/spawned
		return;
	}

	pComp->SetTimeRemaining(pComp->GetTimeRemaining()-fDt);
	if(pComp->GetTimeRemaining() <= 0.0f)
	{
		pComp->Despawn();
		return;
	}
	pComp->SetPosition(pComp->m_pAttachedObject->GetTransform()->GetWorldPosition());
	pComp->m_pParent->GetTransform()->GetLocalMatrix()._42 = 0.5f;
	CWwiseSoundManager::GetInstance()->SetObjectPosition(pComp->CHICKENSOUP_ID, pComp->GetParent()->GetTransform()->GetWorldPosition(), 0.30f);

	//SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);
}
void CChickenSoupComponent::PlayerCollision(IEvent* /*cEvent*/, scd::base_component* /*cCenter*/)
{
	//CChickenSoupComponent* pComp = (CChickenSoupComponent*) cCenter;

}
void CChickenSoupComponent::ItemCollision(IEvent* /*cEvent*/, scd::base_component* /*cCenter*/)
{

}
void CChickenSoupComponent::Despawn()
{
	SendObjectEvent("InvulnerableEnd", this, m_pAttachedObject);

	m_fTimeRemaining = 0.0f;
	m_bIsSpawned = false;
	CWwiseSoundManager::GetInstance()->PlayTheSound(CHICKENSOUP_STOP, CHICKENSOUP_ID);
	CWwiseSoundManager::GetInstance()->UnregisterObject(CHICKENSOUP_ID);
	m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;
	
}