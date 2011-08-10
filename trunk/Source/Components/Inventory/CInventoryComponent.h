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
#include "..\\Level\\CGoalItemComponent.h"
#include "..\\..\\Enums.h"
#include "..\\..\\IComponent.h"

// Foward Declares
class CObject;
class CGoalItems;
class IEvent;

class CInventoryComponent : public IComponent
{
private:

	CObject*	m_pParent;			// The parent object
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
	CInventoryComponent(CObject* pParent, int nPlayer);
	
	// Collect Item
	static void PickupItemCollectedCallback(IEvent* e, IComponent* comp);
	static void GoalItemCollectedCallback(IEvent* e, IComponent* comp);
	static void GoalItemSpawnedCallback(IEvent* e, IComponent* comp);
	static void GoalItemInitCallback(IEvent* e, IComponent* comp);
	static void HandleInit(IEvent* e, IComponent* comp);
	static void Update(IEvent*, IComponent*);

	static void UseHeldItem1(IEvent*, IComponent*); 
	static void UseHeldItem2(IEvent*, IComponent*);
	static void HeldItemCollected(IEvent*, IComponent*);
	static void HeldItemSteal(IEvent*, IComponent*);

	void UseHeldItem(CInventoryComponent* pComp, int nIndex, float fAmount);
	
	// Rammed
	static void Rammed(IEvent* e, IComponent* comp);

	static void checkout(IEvent* e, IComponent* comp);
	static void StunCallback(IEvent* e, IComponent* comp);

	// Accessors
	inline CObject* GetParent()
	{
		return m_pParent;
	}

	inline EHeldItemType GetHeldItem(int nIndex)
	{
		return m_eHeldItem[nIndex];
	}
	//Sound Functions
	void PlayCollectionSound(CObject* player);
	void PlayAllItemCollectionSound(CObject* player);
	void PlayItemSpawnSound(int nitem);

};

#endif