#include "CBanana.h"
#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\..\CObject.h"
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"
#include "..\..\..\Managers\Component Managers\CCollisionManager.h"
#include "..\..\..\Managers\Global Managers\Rendering Managers\CEffectManager.h"
#include "..\..\..\Managers\Global Managers\Rendering Managers\Texture Managers\CHUDManager.h"
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
	CEventManager::GetInstance()->RegisterEvent("BananaDestroyedByItem", this, ItemCollision);

	
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


void CBanana::ItemCollision(IEvent* cEvent, IComponent* cCenter)
{
	CBanana* pComp = (CBanana*)cCenter;
	TImpactEvent* tEvent = (TImpactEvent*)cEvent->GetData();
	if(pComp->m_pObject == tEvent->m_pcCollider && pComp->GetIsSpawned())
	{	
		//effect stuff
		D3DXVECTOR3 p1, p2, trans;
		p2 = pComp->m_pObject->GetTransform()->GetWorldPosition();
		p1 = CCollisionManager::GetInstance()->GetBananaDestroyObject()->GetTransform()->GetWorldPosition();
		trans = p2 - p1;//vector to the turkeys position
		CCollisionManager::GetInstance()->GetBananaDestroyObject()->GetTransform()->TranslateFrame(trans);
		CEffectComponent* pEC = CEffectManager::GetInstance()->CreateSlipComponent(CCollisionManager::GetInstance()->GetBananaDestroyObject());
		pEC->SetDeadTimer(EC_TYPE_BANANA_SLIP, 0.0f);
		pEC->SwitchOnOffEmitters(EC_TYPE_BANANA_SLIP, true);

		pComp->Despawn();
	}
}


