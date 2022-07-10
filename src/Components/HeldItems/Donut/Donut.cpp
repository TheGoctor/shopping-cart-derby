#include "Donut.h"
#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"


CDonut* CDonut::CreateDonutComponent(CObject* pObj)
{
	CDonut* pComp = MMNEW(CDonut(pObj));
	pComp->DONUT_ID = -1;
	pComp->FirstInit();
	pObj->AddComponent(pComp);
	return pComp;
}
void CDonut::FirstInit(void)
{
	m_fDuration = DONUTTIME;
	m_fTimeRemaining = m_fDuration;
	DONUT_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	string szEventName = "Update";
	szEventName += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, Update);
	CEventManager::GetInstance()->RegisterEvent("CartCollision", this, PlayerCollision);
}
void CDonut::ReInit()
{
	SetIsSpawned(true);
	DONUT_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	SetTimeRemaining(GetDuration());

	// m_pAttachedObject is already filled out before this reinit is called inside HeldItemMgr
	SendStatusEffectEvent("DonutEffect", this, m_pAttachedObject, m_fDuration);
	// goober 2
	//SendStatusEffectEvent("DonutDespawn", this, m_pAttachedObject, 0.0f);
	CWwiseSoundManager::GetInstance()->SetObjectPosition(DONUT_ID, m_pAttachedObject->GetTransform()->GetWorldPosition(), 0.3f);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_DOUGHNUT_USE, DONUT_ID);
}

void CDonut::Despawn()
{
	m_fTimeRemaining = 0.0f;
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_DOUGHNUT_AURA_STOP, DONUT_ID);

	m_bIsSpawned = false;
	m_pParent->GetTransform()->GetLocalMatrix()._41 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._42 = 300.0f;
	m_pParent->GetTransform()->GetLocalMatrix()._43 = 300.0f;

	CWwiseSoundManager::GetInstance()->UnregisterObject(DONUT_ID);
	// goober 1
}

//call backs
void CDonut::Update(IEvent* cEvent, IComponent* cCenter)
{
	CDonut* pComp = (CDonut*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDt = eEvent->m_fDeltaTime;
	
	if(!pComp->GetIsSpawned())
	{
		return;
	}
	
	pComp->SetTimeRemaining(pComp->GetTimeRemaining()-fDt);
	if(pComp->GetTimeRemaining()<=0.0f)
	{
		pComp->Despawn();
		return;
	}
	
	pComp->SetPosition(pComp->m_pAttachedObject->GetTransform()->GetWorldPosition());
	pComp->m_pParent->GetTransform()->GetLocalMatrix()._42 = .5f;

	//SendRenderEvent("AddToSet", pComp, pComp->m_pParent, PRIORITY_IMMEDIATE);
	
	pComp->SetPosition(pComp->GetParent()->GetTransform()->GetWorldPosition());
	
}
void CDonut::PlayerCollision(IEvent* cEvent, IComponent* cCenter)
{
	CDonut* pComp = (CDonut*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();

	if(pComp->GetIsSpawned() && tEvent->m_pcCollider == pComp->m_pAttachedObject)
	{
		// send the collider as the one who gets the item
		CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_DOUGHNUT_IMPACT, pComp->DONUT_ID);
		SendImpactEvent("Steal", pComp, tEvent->m_pcCollider, tEvent->m_pCollidedWith, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SendStatusEffectEvent("DonutDespawn", pComp, pComp->m_pAttachedObject, 0.0f);
		pComp->Despawn();
	}

}