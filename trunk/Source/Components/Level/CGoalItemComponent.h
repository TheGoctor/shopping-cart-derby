/*******************************************************************************
 * Filename:  		CGoalItemComponent.h
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

#ifndef _CGOALITEMS_H_
#define _CGOALITEMS_H_

#include "..\..\IComponent.h"
#include "..\..\Enums.h"
#include "..\..\Managers\Global Managers\Event Manager\CEventManager.h"

#define NULL 0
class CObject;

class CGoalItems : public IComponent
{
public:
	// HACK: make private with mutators
	EGoalItemType m_eType;
	CObject* m_pObject;
	bool m_bSpawned;
	bool m_bDespawning;
	bool m_bCollidable;
	float m_fDespawnTimer;
	EventID m_eEventID;

	bool m_bBlink;

	// Constructor
	CGoalItems(CObject* pObj);
	CGoalItems(){}

	// Destructor
	~CGoalItems();

	void Init(EGoalItemType);

	static void Update(IEvent*, IComponent*);
	static void ShutdownGoalItem(IEvent*, IComponent*);
	void Spawn();
	void Despawn();
	void SetDespawnTimer(const float fDT);

	inline CObject* GetParent(void)
	{
		return m_pObject;
	}

	static void PauseUpdateCallback(IEvent*, IComponent* pComp);

};

#endif