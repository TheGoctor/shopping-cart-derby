///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryComponent.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	04/17/11
//
//  Purpose			:	A component that allows a game object to have a list of
//						goal and held items. Also alows for collecting and losing
//						said items
///////////////////////////////////////////////////////////////////////////////

// Includes
#include "CInventoryComponent.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\PickupItem\CPickupItemComponent.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Renderer.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Component Managers\CCollisionManager.h"
#include "..\..\Managers\Component Managers\CSpawningManager.h"
using namespace EventStructs;

#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

// Constructor
CInventoryComponent::CInventoryComponent(CObject* pParent, int nPlayer)
	: m_pParent(pParent), m_nPlayerNumber(nPlayer), m_nNumItemsCollected(0)
{
	for(int item = 0; item < 8; ++item)
	{
		m_bGoalItems[item] = false;
	}

	CEventManager::GetInstance()->RegisterEvent("GoalItemCollected", this,
		GoalItemCollectedCallback);
	CEventManager::GetInstance()->RegisterEvent("CheckoutCollision", this,
		checkout);

	
	string szEventName = "InitObjects";
	szEventName += (char)GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, HandleInit);	


}

// Collect Item
void CInventoryComponent::GoalItemCollectedCallback(IEvent* e, IComponent* comp)
{
	// Get the Values from the Event
	TGoalItemCollectedEvent* pcObjEvent = static_cast<TGoalItemCollectedEvent*>
		(e->GetData());
	int eType = CSpawningManager::GetInstance()->GetGoalItemType(pcObjEvent->
		m_pcGoalItem);
	CObject* pCollector = pcObjEvent->m_pcCollector;

	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	if(pComp->GetParent() == pCollector)
	{
		// if we collected it for the first time
		if(pComp->m_bGoalItems[eType] == false)
		{
			// let the rankings know, sending the object (player) that collected
			SendObjectEvent("IncrementRankScore", pComp, pComp->GetParent());
		}
		pComp->m_bGoalItems[eType] = true;
		pComp->m_nNumItemsCollected++;

		CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_PICK_UP, GLOBAL_ID);
	}
}

// Rammed
void CInventoryComponent::Rammed(CObject* pRammerObject, CObject* pRammedObject)
{
	for(int item = 0; item < 8; ++item)
	{
		if(m_bGoalItems[item])
		{
			m_bGoalItems[item] = false;

			// Spawn Item
			D3DXVECTOR3 vObjPos =  m_pParent->GetTransform()->GetWorldPosition();
			D3DXVECTOR3 vRamDir(
				pRammedObject->GetTransform()->GetLocalMatrix()._31,
				pRammedObject->GetTransform()->GetLocalMatrix()._32, 
				pRammedObject->GetTransform()->GetLocalMatrix()._33);
			vRamDir *= -1.0f;


			string szObjName = "PickupItem";
			static int nNumCreated = 1; // Is there a better way to append a unique number?

			// Instead of doing this
			//char szStrBuff[6];
			//itoa(nNumCreated, szStrBuff, 10);
			//szObjName += szStrBuff;

			// Do this
			szObjName += (nNumCreated + '0');

			nNumCreated++;
			CObject* pPickupItem = CObjectManager::GetInstance()->CreateObject(szObjName, vObjPos.x, vObjPos.y, vObjPos.z);


			CPickupItemComponent::CreatePickupItemComponent(pPickupItem, vRamDir);
			Renderer::GetInstance()->CreateRenderComp(pPickupItem, 23, 0, 1);
			// Add a collision component to pick it up
			CCollideable* pCol = CCollisionManager::GetInstance()->CreateCollideableComponent(pPickupItem, true, false, OBJPITEM);
			TSphere tsphere;
			tsphere.cPosition = vObjPos;
			tsphere.fRadius = 1.0f;
			pCol->SetBVType(BSPHERE);
			pCol->SetSphere(tsphere);
/**/
			// Play a sound
			//Sound Hack
			CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_DROP, GLOBAL_ID);

			//TODO: Let the hud know that the item was removed
			// Send goal item event "ItemDropped" 
			SendGoalItemCollectedEvent("ItemDropped", this, (CObject*)item, m_pParent);
		}
	}
}

void CInventoryComponent::checkout(IEvent* e, IComponent* comp)
{
	CInventoryComponent* pComp = (CInventoryComponent*)comp;
	TObjectEvent* objEvent = (TObjectEvent*)e->GetData();

	// if it's us
	if(objEvent->m_pcObj == pComp->GetParent())
	{
		// check for full list
		for(int i=0; i< 8; i++) // HACK: Hardcoded 8
		{
			if(pComp->m_nNumItemsCollected >= 8)
			{
				break; // we have 8 items, means we can checkout
			}

			if(pComp->m_bGoalItems[i] == false) // if one of them isn't collected
			{
				return; // no need to checkout
			}
		}

		// got here because all of m_bGoalItems is true. Someone won.
		
		// are we the human player?
		if(pComp->m_nPlayerNumber == 1)
		{
			// send win state because the player won yay!
			SendStateEvent("StateChange", pComp, WIN_STATE, PRIORITY_NORMAL);
		}
		else // we're an AI
		{
			// send lose state because someone else won, not the player (HACK: end game shouldn't be this way)
			SendStateEvent("StateChange", pComp, WIN_STATE, PRIORITY_NORMAL);
		}
		
	}
}

void CInventoryComponent::HandleInit(IEvent* e, IComponent* comp)
{
	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	pComp->m_nNumItemsCollected = 0;
	
	for(int item = 0; item < 8; item++)
	{
		pComp->m_bGoalItems[item] = false;
	}
}