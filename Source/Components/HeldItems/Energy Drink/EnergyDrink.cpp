#include "EnergyDrink.h"
#include "..\..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\..\Managers\Component Managers\CHeldItemManager.h"
using namespace EventStructs;


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
	SendStatusEffectEvent("Boost", pComp, eEvent->m_pObject, pComp->m_fDuration);
	pComp->SetIsSpawned(true);
}

void CEnergyDrink::Despawn()
{
	m_bIsSpawned = false;
}