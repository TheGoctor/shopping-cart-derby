///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryComponent.h"
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
#ifndef _CINVENTORYCOMPONENT_H_
#define _CINVENTORYCOMPONENT_H_

// Includes
#include "..\\Level\\CGoalItemComponent.h"
#include"..\\..\\IComponent.h"

// Foward Declares
class CObject;
class CGoalItems;
class IEvent;

class CInventoryComponent : public IComponent
{
private:

	CObject*	m_pParent;			// The parent object
	bool		m_bGoalItems[8];	// A list of current goal items
	int			m_nPlayerNumber;

	int			m_nNumItemsCollected;
	
public:

	// Constructor
	CInventoryComponent(CObject* pParent, int nPlayer);
	
	// Collect Item
	static void GoalItemCollectedCallback(IEvent* e, IComponent* comp);
	static void HandleInit(IEvent* e, IComponent* comp);

	// Rammed
	void Rammed(CObject* pRammerObject, CObject* pRammedObject);

	static void checkout(IEvent* e, IComponent* comp);


	// Accessors
	inline CObject* GetParent()
	{
		return m_pParent;
	}
};

#endif