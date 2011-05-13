/*******************************************************************************
 * Filename:  		CGoalItemComponent.cpp
 * Date:      		05/01/2011
 * Mod. Date: 		05/09/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Goal Item in the world.
					When created, Init should be called and a EGoalItemType
					should be given. Goal Items will register for an update
					event and will despawn after a set period of time. This
					time is set in the init function.
 ******************************************************************************/

#include <string>
#include "CGoalItemComponent.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\Managers\Component Managers\CSpawningManager.h"
using namespace EventStructs;

CGoalItems::CGoalItems(CObject* pObj) : m_pObject(pObj)
{
}

CGoalItems::~CGoalItems()
{
	//CEventManager::GetInstance()->UnregisterEventAll(this);

	//CObjectManager::GetInstance()->DestroyObject(this->GetParent());
}

void CGoalItems::Init(EGoalItemType eType)
{
	std::string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	m_eEventID = CEventManager::GetInstance()->RegisterEvent(szEvent, this, 
		Update);
	//m_eEventID = CEventManager::GetInstance()->RegisterEvent("GoalItemCollected",
	//	this, GoalItemCollectedCallback);
	m_bBlink = false;

	m_eType = eType;
	m_fDespawnTimer = 10.0f;
	m_bDespawning = true;
	m_bSpawned = true;
}

void CGoalItems::SetDespawnTimer(const float fDespawnTime)
{
	m_fDespawnTimer = fDespawnTime;
	//m_bDespawning = true;
}

void CGoalItems::Update(IEvent* piEvent, IComponent* piComponent)
{
	CGoalItems* pcGoalItem = (CGoalItems*)piComponent;
	float fDT = ((TUpdateStateEvent*)piEvent->GetData())->m_fDeltaTime;
	
	if(pcGoalItem->m_bSpawned == false)
		return;

	pcGoalItem->m_fDespawnTimer -= fDT;

	if(pcGoalItem->m_fDespawnTimer <= 4.0f)
	{
		pcGoalItem->m_bBlink = !pcGoalItem->m_bBlink;
	}

	//NOTE: I'm commenting this out for now, it's getting culled in the 
	//			collision manager, we shall see about blinking later   -Ray
	if(false == pcGoalItem->m_bBlink)
	{
		SendRenderEvent("AddToSet", NULL, pcGoalItem->GetParent());
	}

	if(pcGoalItem->m_fDespawnTimer <= 0.0f)
	{
		pcGoalItem->m_bSpawned = false;
		SendGoalItemEvent("Despawned", pcGoalItem, pcGoalItem->m_eType, pcGoalItem);
		SendGoalItemEvent("GoalItemDespawned", pcGoalItem, pcGoalItem->m_eType, pcGoalItem);
		pcGoalItem->Despawn();
	}
}

void CGoalItems::Despawn()
{
	m_bSpawned = false;
	m_pObject->GetTransform()->TranslateFrame(D3DXVECTOR3(-65535,-65535,-65535));
	//MMDEL(CGoalItems, this);
}

//void CGoalItems::GoalItemCollectedCallback(IEvent* piEvent, IComponent* piComponent)
//{
//	CGoalItems* pComp = (CGoalItems*)piComponent;
//	TGoalItemCollectedEvent* tEvent = (TGoalItemCollectedEvent*)piEvent->GetData();
//
//	if(pComp->GetParent() == tEvent->m_pcGoalItem)
//	{
//		SendGoalItemCollectedEvent("GoalItemCollectedMoo", pComp, pComp->GetParent(), tEvent->m_pcCollector, pComp->m_eType, PRIORITY_NORMAL);
//	}
//}