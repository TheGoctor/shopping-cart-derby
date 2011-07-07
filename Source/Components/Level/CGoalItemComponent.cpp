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
	std::string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	m_eEventID = CEventManager::GetInstance()->RegisterEvent(szEvent, this, 
		Update);
		
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


	szEvent = "ShutdownObjects";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, 
		ShutdownGoalItem);
}

CGoalItems::~CGoalItems()
{
	//CEventManager::GetInstance()->UnregisterEventAll(this);
}

void CGoalItems::Init(EGoalItemType eType)
{	
	m_bBlink = false;
	m_eType = eType;
	m_fDespawnTimer = CSpawningManager::GetInstance()->m_fGoalItemWaveTime;
	m_bDespawning = true;
	m_bSpawned = false;
	m_bCollidable = false;
}

void CGoalItems::Spawn()
{
	m_bSpawned = true;
	m_bCollidable = true;
	SendGoalItemEvent("GoalItemSpawned", this, m_eType, this);
}

void CGoalItems::SetDespawnTimer(const float fDespawnTime)
{
	m_fDespawnTimer = fDespawnTime;
}

void CGoalItems::Update(IEvent* piEvent, IComponent* piComponent)
{
	// Get component
	CGoalItems* pcGoalItem = (CGoalItems*)piComponent;

	// If not spawned don't update
	if(pcGoalItem->m_bSpawned == false)
		return;

	// Get the fDT
	float fDT = ((TUpdateStateEvent*)piEvent->GetData())->m_fDeltaTime;

	// Update the despawn timer
	pcGoalItem->m_fDespawnTimer -= fDT;

	// Show blinking if despawn timer is less then 1/3
	// of its total despawn timer
	if(pcGoalItem->m_bCollidable == true)
	{
		if(pcGoalItem->m_fDespawnTimer <= 
			(CSpawningManager::GetInstance()->m_fGoalItemWaveTime / 3))
		{
			pcGoalItem->m_bBlink = !pcGoalItem->m_bBlink;
			SendGoalItemEvent("GoalItemBlinking", pcGoalItem, pcGoalItem->m_eType, pcGoalItem);
		}

		// AddToSet
		if(pcGoalItem->m_bBlink == false)
		{
			SendRenderEvent("AddToSet", pcGoalItem, pcGoalItem->GetParent(), PRIORITY_IMMEDIATE);
		}
	}

	// Goal Item is despawning so tell everyone about it
	if((pcGoalItem->m_fDespawnTimer <= 0.0f) && 
		(pcGoalItem->m_bDespawning == true))
	{
		pcGoalItem->m_bDespawning = false;
		SendGoalItemEvent("GoalItemDespawned", pcGoalItem, pcGoalItem->m_eType, pcGoalItem,
			PRIORITY_IMMEDIATE);
		pcGoalItem->Init(pcGoalItem->m_eType);
	}
}

void CGoalItems::Despawn()
{
	m_bCollidable = false;
}

//void CGoalItems::Collected()
//{
//	// Reinit everything but the timer and bspawned 
//	//(so that "GoalItemDespawned" can happen after the timer finishes)
//	
//}


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


void CGoalItems::ShutdownGoalItem(IEvent* /*piEvent*/, IComponent* piComponent)
{
	// Get component
	CGoalItems* pcGoalItem = (CGoalItems*)piComponent;

	pcGoalItem->m_fDespawnTimer = 0.0f; // so we catch the despawn in update next update
}

void CGoalItems::PauseUpdateCallback(IEvent*, IComponent* pComp)
{
	// Get the Effect Comp
	CGoalItems* comp = (CGoalItems*)pComp;
	
	if(comp->m_bSpawned)
	{
		SendRenderEvent("AddToSet", comp, comp->m_pObject, PRIORITY_UPDATE);
	}	
}


