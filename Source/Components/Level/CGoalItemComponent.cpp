/*******************************************************************************
 * Filename:  		CGoalItemComponent.cpp
 * Date:      		05/01/2011
 * Mod. Date: 		07/26/2011
 * Mod. Initials:	JS
 * Author:    		Jesse A. Stanciu
 * Purpose:   		This will represent the Goal Item in the world.
					When created, Init should be called and a EGoalItemType
					should be given. Goal Items will register for an update
					event and will despawn after a set period of time. This
					time is set in the init function.
 ******************************************************************************/
#include "CGoalItemComponent.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\Enums.h"
using namespace EventStructs;

CGoalItems::CGoalItems(CObject* pObj) : m_pObject(pObj)
{
	std::string szEvent = "Update";
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

	szEvent = "Update";
	szEvent += QUIT_CONFIRMATION_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	szEvent = "Update";
	szEvent += IN_GAME_HOW_TO_PLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);
	szEvent = "Update";
	szEvent += IN_GAME_HOW_TO_PLAY_CONTROLLER_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, PauseUpdateCallback);

	szEvent = "ShutdownObjects";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, ShutdownGoalItem);
}

CGoalItems::~CGoalItems()
{
}

void CGoalItems::Init(const EGoalItemType eType)
{	
	m_bBlink = false;
	m_eType = eType;
	m_fDespawnTimer = 15.0f;
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

	pcGoalItem->m_pObject->GetTransform()->GetLocalMatrix()._42 = sin(pcGoalItem->m_fDespawnTimer * 2.5f)*.2f + 1.3f;
	pcGoalItem->m_pObject->GetTransform()->RotateFrame(D3DXVECTOR3(0,1,0), fDT* 1.5f);

	if(pcGoalItem->m_bCollidable == true)
	{
		if(pcGoalItem->m_fDespawnTimer <= 5.0f)
		{
			// Make us blink if we're despawning soon
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


