///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryComponent.h"
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
#ifndef _CINVENTORYCOMPONENT_H_
#define _CINVENTORYCOMPONENT_H_

// Includes
#include "Level\\CGoalItemComponent.h"
#include "Enums.h"
#include "scd::base_component.h"

// Foward Declares
class scd::object;
class CGoalItems;
class IEvent;

class CInventoryComponent : public scd::base_component
{
private:

	scd::object*	m_pParent;			// The parent object
	int			m_nPlayerNumber;
	EHeldItemType m_eHeldItem[2];

	int			m_nGoalItemIndices[MAX_GOAL_ITEMS]; // indicies into the m_bGoalItemCollectionState so [itemID] in this will yeild the correct [] for that
	bool		m_bGoalItemCollectionState[NUM_ITEMS_IN_LEVEL];	// A list of current goal items
	int			m_nInitIndices;
	float		m_fStunDuration;
	float		m_fSoundtimer;
	bool		m_bSoundPlayed;
public:
	
	// Constructor
	CInventoryComponent(scd::object* pParent, int nPlayer);
	
	// Collect Item
	static void PickupItemCollectedCallback(IEvent* e, scd::base_component* comp);
	static void GoalItemCollectedCallback(IEvent* e, scd::base_component* comp);
	static void GoalItemSpawnedCallback(IEvent* e, scd::base_component* comp);
	static void GoalItemInitCallback(IEvent* e, scd::base_component* comp);
	static void HandleInit(IEvent* e, scd::base_component* comp);
	static void Update(IEvent*, scd::base_component*);

	static void UseHeldItem1(IEvent*, scd::base_component*); 
	static void UseHeldItem2(IEvent*, scd::base_component*);
	static void HeldItemCollected(IEvent*, scd::base_component*);
	static void HeldItemSteal(IEvent*, scd::base_component*);

	void UseHeldItem(CInventoryComponent* pComp, int nIndex, float fAmount);
	
	// Rammed
	static void Rammed(IEvent* e, scd::base_component* comp);

	static void checkout(IEvent* e, scd::base_component* comp);
	static void StunCallback(IEvent* e, scd::base_component* comp);

	// Accessors
	inline scd::object* GetParent()
	{
		return m_pParent;
	}

	inline EHeldItemType GetHeldItem(int nIndex)
	{
		return m_eHeldItem[nIndex];
	}
	//Sound Functions
	void PlayCollectionSound(scd::object* player);
	void PlayAllItemCollectionSound(scd::object* player);
	void PlayItemSpawnSound(int nitem);

};

#endif