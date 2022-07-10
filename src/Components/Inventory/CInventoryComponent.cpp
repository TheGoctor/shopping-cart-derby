///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryComponent.cpp"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	05/17/11
//
//  Purpose			:	A component that allows a game object to have a list of
//						goal and held items. Also alows for collecting and losing
//						said items
///////////////////////////////////////////////////////////////////////////////

// Includes
#include <string>
#include "CInventoryComponent.h"
#include "..\..\Managers\Global Managers\Object Manager\CObjectManager.h"
#include "..\PickupItem\CPickupItemComponent.h"
#include "..\..\Managers\Global Managers\Rendering Managers\Renderer.h"
#include "..\..\Managers\Global Managers\Event Manager\EventStructs.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Managers\Component Managers\CCollisionManager.h"
#include "..\..\Managers\Component Managers\CSpawningManager.h"
#include "..\..\Managers\Component Managers\CInventoryManager.h"
#include "..\..\Managers\Component Managers\CMovementManager.h"
using namespace EventStructs;

#include "..\..\Managers\Global Managers\Sound Manager\CWwiseSoundManager.h"

#include "..\../Managers/Global Managers/Rendering Managers/Texture Managers/CHUDManager.h"



// Constructor
CInventoryComponent::CInventoryComponent(CObject* pParent, int nPlayer)
: m_pParent(pParent), m_nPlayerNumber(nPlayer), m_nInitIndices(0),
m_bSoundPlayed(false)
{
	for(int item = 0; item < NUM_ITEMS_IN_LEVEL; ++item)
	{
		m_bGoalItemCollectionState[item] = false;
	}
	//m_fSoundTimer = 0.0f;
	CEventManager::GetInstance()->RegisterEvent("GoalItemCollected", this,
		GoalItemCollectedCallback);
	CEventManager::GetInstance()->RegisterEvent("GoalItemSpawned", this,
		GoalItemSpawnedCallback);
	CEventManager::GetInstance()->RegisterEvent("PickupItemCollected", this,
		PickupItemCollectedCallback);
	CEventManager::GetInstance()->RegisterEvent("CheckoutCollision", this,
		checkout);
	CEventManager::GetInstance()->RegisterEvent("GoalItemInit", this,
		GoalItemInitCallback);
	CEventManager::GetInstance()->RegisterEvent("HeldItemCollected", this,
		HeldItemCollected);
	CEventManager::GetInstance()->RegisterEvent("UseHeldItem1", this, UseHeldItem1);
	CEventManager::GetInstance()->RegisterEvent("UseHeldItem2", this, UseHeldItem2);
	CEventManager::GetInstance()->RegisterEvent("Stun", this, StunCallback);

	CEventManager::GetInstance()->RegisterEvent("StealItem", this, HeldItemSteal);

	CEventManager::GetInstance()->RegisterEvent("PlayerRammed", this, Rammed);

	std::string szEvent = "Update";
	szEvent += GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEvent, this, Update);

	string szEventName = "InitObjects";
	szEventName += (char)GAMEPLAY_STATE;
	CEventManager::GetInstance()->RegisterEvent(szEventName, this, HandleInit);	

	for(int i=0; i<MAX_GOAL_ITEMS; i++)
	{
		m_nGoalItemIndices[i] = -1;
	}
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
		int nIndex = pComp->m_nGoalItemIndices[eType];
		// if we collected it for the first time
		if(pComp->m_bGoalItemCollectionState[nIndex] == false)
		{
			// let the rankings know, sending the object (player) that collected
			SendObjectEvent("IncrementRankScore", pComp, pComp->GetParent());
		}
		pComp->m_bGoalItemCollectionState[nIndex] = true;

		bool bHave8 = true; // sets to false once it hits an item that isn't collected
		for(int i=0; i<NUM_ITEMS_IN_LEVEL; i++)
		{
			// if one of our items isn't collected
			if(pComp->m_bGoalItemCollectionState[i] == false)
			{
				// say we don't have all of them
				bHave8 = false;
				// and stop checking
				break;
			}
		}

		// if we have all of the goal items
		if(bHave8)
		{
			SendObjectEvent("Has8", pComp, pComp->GetParent());
			pComp->PlayAllItemCollectionSound(pCollector);
		}
		else
		{	
			pComp->PlayCollectionSound(pCollector);
		}

	}
}

void CInventoryComponent::PickupItemCollectedCallback(IEvent* e, IComponent* comp)
{
	// Get the Values from the Event
	TPickupItemCollectedEvent* pcObjEvent = static_cast<TPickupItemCollectedEvent*>
		(e->GetData());
	int eType = pcObjEvent->m_eItemType;
	CObject* pCollector = pcObjEvent->m_pcCollector;

	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	// if the collector is this inventory's parent
	if(pComp->GetParent() == pCollector)
	{
		// get the index in our 8 list that holds the value for the item id
		int nIndex = pComp->m_nGoalItemIndices[eType];
		// if we collected it for the first time
		if(pComp->m_bGoalItemCollectionState[nIndex] == false)
		{
			// let the rankings know, sending the object (player) that collected
			SendObjectEvent("IncrementRankScore", pComp, pComp->GetParent());
		}
		pComp->m_bGoalItemCollectionState[nIndex] = true;

		bool bHave8 = true; // sets to false once it hits an item that isn't collected
		for(int i=0; i<NUM_ITEMS_IN_LEVEL; i++)
		{
			// if one of our items isn't collected
			if(pComp->m_bGoalItemCollectionState[i] == false)
			{
				// say we don't have all of them
				bHave8 = false;
				// and stop checking
				break;
			}
		}

		// if we have all of the goal items
		if(bHave8)
		{
			SendObjectEvent("Has8", pComp, pComp->GetParent());
			pComp->PlayAllItemCollectionSound(pCollector);

		}
		else
		{	
			pComp->PlayCollectionSound(pCollector);
		}
	}
}

void CInventoryComponent::HeldItemCollected(IEvent* iEvent, IComponent* pComponent)
{
	THeldItemCollected* tEvent = (THeldItemCollected*)iEvent->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)pComponent;

	CObject* pHeldItem = tEvent->m_pcHeldItem;
	CObject* pCollector = tEvent->m_pcCollector;

	// Is it us?
	if(pCollector->GetID() == pComp->GetParent()->GetID())
	{
		// Yes!
		if (CHUDManager::GetInstance()->GetPlayerNum(pCollector) == 0)
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_PICK_UP, GLOBAL_ID);
		}
		if(pComp->m_eHeldItem[0] == NO_HELD_ITEM)
		{
			pComp->m_eHeldItem[0] = 
				CSpawningManager::GetInstance()->GetHeldItemType(pHeldItem);
		}
		else if(pComp->m_eHeldItem[1] == NO_HELD_ITEM)
		{
			pComp->m_eHeldItem[1] = 
				CSpawningManager::GetInstance()->GetHeldItemType(pHeldItem);
		}
	}
	// No :(
}

void CInventoryComponent::UseHeldItem1(IEvent* pEvent, IComponent* pComponent)
{
	TInputEvent* pInputEvent = (TInputEvent*)pEvent->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)pComponent;

	// HACK : infinte boost!!!!!
	//pComp->m_eHeldItem[0] = ENERGY_DRINK;

	// if we're stunned, dont let them use an item
	if(pComp->m_fStunDuration > 0.0f)
	{
		return;
	}

	if((pInputEvent->m_pPlayer == pComp->m_pParent) &&
		(pComp->m_eHeldItem[0] != NO_HELD_ITEM))
	{
		pComp->UseHeldItem(pComp, 0, pInputEvent->m_fAmount);
		pComp->m_eHeldItem[0] = NO_HELD_ITEM;
		SendInputEvent("UseHeldItem1Hud", pComp, pInputEvent->m_pPlayer, pInputEvent->m_fAmount);
	}
}

void CInventoryComponent::UseHeldItem2(IEvent* pEvent, IComponent* pComponent)
{
	TInputEvent* pInputEvent = (TInputEvent*)pEvent->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)pComponent;

	// HACK : infinte boost!!!!!
	//pComp->m_eHeldItem[1] = ENERGY_DRINK;


	// if we're stunned, dont let them use an item
	if(pComp->m_fStunDuration > 0.0f)
	{
		return;
	}

	if((pInputEvent->m_pPlayer == pComp->m_pParent) &&
		(pComp->m_eHeldItem[1] != NO_HELD_ITEM))
	{
		pComp->UseHeldItem(pComp, 1, pInputEvent->m_fAmount);
		pComp->m_eHeldItem[1] = NO_HELD_ITEM;
		SendInputEvent("UseHeldItem2Hud", pComp, pInputEvent->m_pPlayer, pInputEvent->m_fAmount);
	}
}

void CInventoryComponent::UseHeldItem(CInventoryComponent* pComp, int nIndex, float fAmount)
{
	switch(pComp->m_eHeldItem[nIndex])
	{
	case TURKEY:
		{
			if(fAmount > 0.0f)
			{
				SendStatusEffectEvent("UseTurkeyForward", pComp, pComp->GetParent(), fAmount);
			}
			else
			{
				SendStatusEffectEvent("UseTurkeyBehind", pComp, pComp->GetParent(), fAmount);
			}
			break;
		}
	case BANANA:
		{
			if(fAmount > 0.0f)
			{
				SendStatusEffectEvent("UseBananaForward", pComp, pComp->GetParent(), fAmount);
			}
			else
			{
				SendStatusEffectEvent("UseBananaBehind", pComp, pComp->GetParent(), fAmount);
			}
			break;
		}
	case PEANUT_BUTTER:
		{
			if(fAmount > 0.0f)
			{
				SendStatusEffectEvent("UsePeanutButterForward", pComp, pComp->GetParent(), fAmount);
			}
			else
			{
				SendStatusEffectEvent("UsePeanutButterBehind", pComp, pComp->GetParent(), fAmount);
			}
			break;
		}
	case PIE:
		{
			if(fAmount > 0.0f)
			{
				SendStatusEffectEvent("UsePieForward", pComp, pComp->GetParent(), fAmount);	
			}
			else
			{
				SendStatusEffectEvent("UsePieBehind", pComp, pComp->GetParent(), fAmount);	
			}
			break;
		}
	case ENERGY_DRINK:
		{
			SendStatusEffectEvent("UseBoost", pComp, pComp->GetParent(), fAmount);
			break;
		}
	case SOUP:
		{
			SendStatusEffectEvent("UseSoup", pComp, pComp->GetParent(), fAmount);
			break;
		}
	case DONUT:
		{
			SendStatusEffectEvent("UseDonut", pComp, pComp->GetParent(), fAmount);
			break;
		}
	case JAM:
		{
			SendStatusEffectEvent("UseJam", pComp, pComp->GetParent(), fAmount);
			CWwiseSoundManager::GetInstance()->PlayTheSound(ITEM_JAM_USE, GLOBAL_ID);
			break;
		}
	}
}

// Rammed
void CInventoryComponent::Rammed(IEvent* e, IComponent* comp)
{
	// This should be moved to the inventorycomponent instead of the manager, no?
	TRamEvent* pEvent = (TRamEvent*)e->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	CObject* pRammerObject = pEvent->m_pcRammer;
	CObject* pRammedObject = pEvent->m_pcRammee;


	// if we were the ones rammed
	if(pComp->m_pParent == pRammedObject)
	{
		
		// make sure we have at least one item to drop
		int nItemsInInventory = 0;
		for(int item = 0; item < NUM_ITEMS_IN_LEVEL; ++item)
		{
			// if they have an item in this slot
			if(pComp->m_bGoalItemCollectionState[item])
			{
				// let the number know
				nItemsInInventory++;
			}
		}

		// if they didn't have any items or are invincible
		bool bPlayerIsSouperGood = CMovementManager::GetInstance()->PlayerIsInvincible(pComp->GetParent());
		if(nItemsInInventory == 0 || bPlayerIsSouperGood)
		{
			// cant drop one, leave.
			return;
		}
		//play hurt sound
		CWwiseSoundManager::GetInstance()->PlayerHurtSound(CHUDManager::GetInstance()->GetPlayerNum(pComp->m_pParent));
		// get a random index to remove
		int nItemToRemove = rand()% NUM_ITEMS_IN_LEVEL;
		// for each item in the inventory
		for(int item = 0; item < NUM_ITEMS_IN_LEVEL; ++item)
		{
			// find the collected one (collectionstate[item] == true) that is also our random item to drop
			if(item == nItemToRemove && pComp->m_bGoalItemCollectionState[item])
			{
				// lose it in the inventory
				pComp->m_bGoalItemCollectionState[item] = false;

				// Get the item that was knocked out
				int nItemID = -1;

				// for all possible items
				for(int i=0; i<MAX_GOAL_ITEMS; i++)
				{
					// if the corresponding index in our 9 list is the item index
					if(pComp->m_nGoalItemIndices[i] == item)
					{
						// store that value
						nItemID = i;
					}
				}

				// TODO: Send decrement scoreboard and lose item event to the hud
				// Send goal item event "ItemDropped" 
				SendGoalItemCollectedEvent("ItemDropped", pComp, (CObject*)nItemID, pComp->m_pParent);
				CWwiseSoundManager::GetInstance()->PlayTheSound(GOAL_ITEM_DROP, GLOBAL_ID);

				// at rammed obj location (that's who we're in right now)
				D3DXVECTOR3 vObjPos =  pComp->m_pParent->GetTransform()->GetWorldPosition();

				// object fly direction should be the rammERs heading. That's the way they're going innit?
				D3DXVECTOR3 vRamDir(
					pRammerObject->GetTransform()->GetLocalMatrix()._31,
					pRammerObject->GetTransform()->GetLocalMatrix()._32, 
					pRammerObject->GetTransform()->GetLocalMatrix()._33);

				// Spawn the item
				SendSpawnPickupItemEvent("SpawnPickupItem", pComp, vObjPos, vRamDir, nItemID);

				break;
			}
			else if(nItemToRemove == item) // else if we're on the rnd index and it's not in the inventory
			{
				// reset the stuff and try again
				nItemToRemove = rand()% NUM_ITEMS_IN_LEVEL;
				item = -1; // -1 so that item++ at the end of the for() will make it start at 0
			}
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
		for(int i=0; i< NUM_ITEMS_IN_LEVEL; i++) 
		{
			if(pComp->m_bGoalItemCollectionState[i] == false) // if one of them isn't collected
			{
				/*// HACK: don't actually return so we can win.
				if(GetAsyncKeyState('O') && pComp->m_nPlayerNumber == 0)
				{
				continue;
				}/**/

				return; // no need to checkout
			}
		}

		// got here because all of m_bGoalItems is true. Someone won.

		// are we the human player?
		SendInputEvent("GameWon", pComp, pComp->m_pParent, 1.0f);
	}
}

void CInventoryComponent::HandleInit(IEvent*, IComponent* comp)
{
	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	// reset the state of all items
	for(int item = 0; item < NUM_ITEMS_IN_LEVEL; item++)
	{
		pComp->m_bGoalItemCollectionState[item] = false;
	}

	pComp->m_eHeldItem[0] = NO_HELD_ITEM;
	pComp->m_eHeldItem[1] = NO_HELD_ITEM;
}

// Collect Item
void CInventoryComponent::GoalItemInitCallback(IEvent* e, IComponent* comp)
{
	// Get the Values from the Event
	TGoalItemEvent* pcObjEvent = (TGoalItemEvent*)e->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	int nSpawnedItemID = pcObjEvent->m_eGoalItemType;

	// if this index is out of bounds
	if(pComp->m_nInitIndices > MAX_GOAL_ITEMS || pComp->m_nInitIndices < 0)
	{
		return;
	}

	pComp->m_nGoalItemIndices[nSpawnedItemID] = pComp->m_nInitIndices;

	pComp->m_nInitIndices++;	
}

void CInventoryComponent::Update(IEvent* e, IComponent* iComponent)
{
	CInventoryComponent* pInventoryComponent = (CInventoryComponent*)iComponent;
	TUpdateStateEvent* tEvent = (TUpdateStateEvent*)e->GetData();

	// update our stun timer
	pInventoryComponent->m_fStunDuration -= tEvent->m_fDeltaTime;
	pInventoryComponent->m_fSoundtimer	+= tEvent->m_fDeltaTime;	
}


void CInventoryComponent::StunCallback(IEvent* e, IComponent* comp)
{
	TStatusEffectEvent* tEvent = (TStatusEffectEvent*)e->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	if(pComp->m_pParent == tEvent->m_pObject)
	{
		pComp->m_fStunDuration = tEvent->m_fDuration;
	}
}

void CInventoryComponent::HeldItemSteal(IEvent* e, IComponent* comp)
{
	// Get the Values from the Event
	TImpactEvent* tEvent = (TImpactEvent*)e->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)comp;

	// recieve the collider as the one who gets the item

	if(pComp->m_pParent == tEvent->m_pcCollider)
	{
		// take a powerup from the collidedwith
		CInventoryComponent* pCompToStealFrom = CInventoryManager::GetInstance()->GetInventoryComponent(tEvent->m_pCollidedWith);
		int nIndexStoleFrom = -1;
		EHeldItemType nStolenItem;

		// grab the item and find the index it got from
		if(pCompToStealFrom->m_eHeldItem[0] != NO_HELD_ITEM)
		{
			nStolenItem = pCompToStealFrom->m_eHeldItem[0];
			nIndexStoleFrom = 0;
		}
		else
		{
			nStolenItem = pCompToStealFrom->m_eHeldItem[1];
			nIndexStoleFrom = 1;
		}

		// if their inventory is empty (it hit "else" and STILL got a no held item)
		if(nStolenItem == NO_HELD_ITEM)
		{
			// cant do anything with it, get out
			return;
		}

		pCompToStealFrom->m_eHeldItem[nIndexStoleFrom] = NO_HELD_ITEM;

		// take it out of the hud if they use it on us (if the player who lost is the human)
		if(CHUDManager::GetInstance()->GetPlayerNum(tEvent->m_pCollidedWith) == 0)
		{
			if(nIndexStoleFrom == 0)
			{
				SendInputEvent("UseHeldItem1Hud", pComp,tEvent->m_pCollidedWith, 1.0f);
				CHUDManager::GetInstance()->SendStolenVictimItemEvent(0);
			}
			else // we stole item 2
			{
				SendInputEvent("UseHeldItem2Hud", pComp, tEvent->m_pCollidedWith, 1.0f);
				CHUDManager::GetInstance()->SendStolenVictimItemEvent(1);
			}
		}/**/

		// Get the human object*
		CObject* pHumanObj = CObjectManager::GetInstance()->GetObjectByName("Player0");


		// if we have room in our inventory, add that to our inventory (if no room, can't do anything but it still stole
		CHUDManager* pHUD = CHUDManager::GetInstance();
		if(pComp->m_eHeldItem[0] == NO_HELD_ITEM)
		{
			pComp->m_eHeldItem[0] = nStolenItem;
			// tell the hud to show an item ONLY IF ITS THE HUMAN PLAYER CAUSE THEY HAVE THE HUD!!!
			if(pHumanObj == pComp->m_pParent)
			{
				pHUD->HeldItemIDCollected(nStolenItem);
				pHUD->SendStolenItemEvent(0);
			}
		}
		else if(pComp->m_eHeldItem[1] == NO_HELD_ITEM)
		{
			pComp->m_eHeldItem[1] = nStolenItem;
			// tell the hud to show an item ONLY IF ITS THE HUMAN PLAYER CAUSE THEY HAVE THE HUD!!!
			if(pHumanObj == pComp->m_pParent)
			{
				pHUD->HeldItemIDCollected(nStolenItem);
				pHUD->SendStolenItemEvent(1);
			}
		}

		// if neither ifs got hit, just don't care about the item. we got it out of them and if we wanted it, we'dve made room in ours
	}
}

void CInventoryComponent::PlayCollectionSound(CObject *player)
{
	switch(CHUDManager::GetInstance()->GetPlayerCharacter(CHUDManager::GetInstance()->GetPlayerNum(player)))
	{
	case BIKER_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(BULLDOG_COLLECTION, BIKER_ID);
			break;
		}
	case BANDITOS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(BANDITOS_COLLECTION, BANDITOS_ID);
			break;
		}
	case LARPERS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LARPER_COLLECTION, LARPER_ID);
			break;
		}
	case SCIENTIST_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(SCIENTIST_COLLECTION, SCIENTIST_ID);
			break;
		}
	case SASHA_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(SASHA_PICKUP, SASHA_ID);
			break;
		}
	case CRYGAME_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(CRYGAME_COLLECTION, CRYGAME_ID);
			break;
		}
	case STORYTIME_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(STORYTIME_COLLECTION, STORYTIME_ID);
			break;
		}
	}
}

void CInventoryComponent::PlayAllItemCollectionSound(CObject *player)
{

	switch(CHUDManager::GetInstance()->GetPlayerCharacter(CHUDManager::GetInstance()->GetPlayerNum(player)))
	{
	case BIKER_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(BULLDOG_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	case BANDITOS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(BANDITOS_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	case LARPERS_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(LARPER_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	case SCIENTIST_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(SCIENTIST_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	case SASHA_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(SASHA_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	case CRYGAME_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(CRYGAME_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	case STORYTIME_CHARACTER:
		{
			CWwiseSoundManager::GetInstance()->PlayTheSound(STORYTIME_ALLGOALITEMS, GLOBAL_ID);
			break;
		}
	}
}


void CInventoryComponent::GoalItemSpawnedCallback(IEvent* pEvent, IComponent* iComp)
{
	TGoalItemEvent* tEvent = (TGoalItemEvent*)pEvent->GetData();
	CInventoryComponent* pComp = (CInventoryComponent*)iComp;

	int nSpawnedItemID = tEvent->m_eGoalItemType;

	// all inventories receive this. Go through this pcomp's inventory and see if it needs the item spawned
	int nNumItemsCollected = 0;
	int nListIndexNotCollected = -1;
	for(int i=0; i<NUM_ITEMS_IN_LEVEL; i++)
	{
		if(pComp->m_bGoalItemCollectionState[i])
		{
			nNumItemsCollected++;
		}
		else
		{
			nListIndexNotCollected = i;
		}
	}

	int nNotCollectedItemType = -1;

	// if we set the nlistindexnotcollected (aka there's at least 1 item not collected)
	if(nListIndexNotCollected != -1)
	{
		// loop through the array of the indices into goalitemcollectionstate so we can get the goal item index of the noncollected index
		for(int i=0; i<MAX_GOAL_ITEMS; i++)
		{
			if(pComp->m_nGoalItemIndices[i] == nListIndexNotCollected)
			{
				nNotCollectedItemType = i;
			}
		}
	}

	if(nNotCollectedItemType == nSpawnedItemID && nNumItemsCollected == 7)
	{
		pComp->m_bSoundPlayed = false;
		pComp->PlayItemSpawnSound(nSpawnedItemID);
	}
}

void CInventoryComponent::PlayItemSpawnSound(int nitem)
{
	if (!m_bSoundPlayed)
	{
		//CWwiseSoundManager::GetInstance()->PlayTheSound(SALE_ON, GLOBAL_ID);
		switch(nitem)
		{
		case GORILLA_MILK:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(GORILLAMILD_SPAWN, GLOBAL_ID);
				break;
			}
		case CAPTAIN_FLAKEY:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(CAPTAINFLAKYS_SPAWN, GLOBAL_ID);
				break;
			}
		case PIZZA:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(BLUEBOMBER_SPAWN, GLOBAL_ID);
				break;
			}
		case CAKE:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(BIRTHDAYCAKE_SPAWN, GLOBAL_ID);
				break;
			}
		case CANNED_BEARS:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(BEAR_IN_CAN_SPAWN_SOUND, GLOBAL_ID);
				break;
			}
		case CARROTS:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(CARROTS_SPAWN, GLOBAL_ID);
				break;
			}
		case SAMMY_SANDWICH:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(SAMMYTHESANDWICH_SPAWN, GLOBAL_ID);
				break;
			}
		case SQUID:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(PIRATESQUID_SPAWN, GLOBAL_ID);
				break;
			}
		case AGENT_ORANGE:
			{
				CWwiseSoundManager::GetInstance()->PlayTheSound(ORANGE_SPAWN_SOUND, GLOBAL_ID);
				break;
			}
		}
		m_bSoundPlayed = true;
	}
}