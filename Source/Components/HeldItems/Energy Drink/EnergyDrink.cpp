#include "EnergyDrink.h"
#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
using namespace EventStructs;
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"
#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"



CEnergyDrink* CEnergyDrink::CreateEnergyDrinkComponent(CObject* pObj)
{
	//	CEnergyDrink* pComp = MMNEW(CObject(pObj));
	CEnergyDrink* pComp = MMNEW(CEnergyDrink(pObj));
	pComp->FirstInit();
	pObj->AddComponent(pComp);
	return pComp;
}
void CEnergyDrink::FirstInit()
{
	m_fDuration = 3.0f;
	m_fBoostAmount = 2.0f;		//mult with speed for boost
	CEventManager::GetInstance()->RegisterEvent("UseBoost", this, ReInit);
}
//call backs
void CEnergyDrink::Update(IEvent* cEvent, IComponent* cCenter)
{
	CEnergyDrink* pComp = (CEnergyDrink*)cCenter;
	TUpdateStateEvent* eEvent = (TUpdateStateEvent*)cEvent->GetData();
	float fDT = eEvent->m_fDeltaTime;

	// if we're not spawned, dont do anything
	if(!pComp->GetIsSpawned())
	{
		return;
	}

	// decrement time
	pComp->SetTimeLeft(pComp->GetTimeLeft()-fDT);

	CWwiseSoundManager::GetInstance()->SetObjectPosition(pComp->ENERGY_DRINK_ID, pComp->GetParent()->GetTransform()->GetWorldPosition(), 0.25f);
	if(pComp->GetTimeLeft() <= 0.0f)
	{
		pComp->Despawn();
		return;
	}

}
void CEnergyDrink::ReInit(IEvent* cEvent, IComponent* cCenter)
{
	CEnergyDrink* pComp = (CEnergyDrink*)cCenter;
	TStatusEffectEvent* eEvent = (TStatusEffectEvent*)cEvent->GetData();
	pComp->ENERGY_DRINK_ID = CWwiseSoundManager::GetInstance()->RegisterHeldObject();
	SendStatusEffectEvent("Boost", pComp, eEvent->m_pObject, pComp->m_fDuration);
	CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_ENERGYDRINK_USE, pComp->ENERGY_DRINK_ID);
	pComp->SetIsSpawned(true);
}

void CEnergyDrink::Despawn()
{
	CWwiseSoundManager::GetInstance()->UnregisterObject(ENERGY_DRINK_ID);
	m_bIsSpawned = false;
}