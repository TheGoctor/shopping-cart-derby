///////////////////////////////////////////////////////////////////////////////
//  File			:	"CPickupItemManager.h"
//
//  Author			:	Mac Smith (MS)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	05/18/11
//
//  Purpose			:	A manager for PickupItem components
///////////////////////////////////////////////////////////////////////////////
#ifndef _CPICKUPITEMMANAGER_H_
#define _CPICKUPITEMMANAGER_H_

// Includes
#include <list>
using std::list;

// LUA
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// Includes
#include "..\\Global Managers\\Memory Manager\\CAllocator.h"
#include "..\\..\\Components\\PickupItem\\CPickupItemComponent.h"
#include "..\Global Managers\Event Manager\CEventManager.h"
#include "..\..\Enums.h"

// Foward Declares
class IEvent;

class CPickupItemManager
{
private:
	// Components
	list<CPickupItemComponent*, 
		CAllocator<CPickupItemComponent*>> m_cPickupItemComps;	// A list of all the PickupItem Components

	unsigned int m_nRenderContexts[MAX_GOAL_ITEMS];

	// Trilogy of Evil
	CPickupItemManager(void)
	{
	}
	CPickupItemManager(CPickupItemManager &ref);

public:
	// Destructor
	~CPickupItemManager(void)
	{
	}
	
	// Init
	void Init(void);

	inline static CPickupItemManager *GetInstance(void)
	{
		static CPickupItemManager cPickupItemManager;
		return &cPickupItemManager;
	}

	CPickupItemComponent* GetAPickupItem(EGoalItemType eType);
	int GetModelNumber(EGoalItemType eType);

	void AddComponent(CPickupItemComponent* pComp);
	void RemoveComponent(CObject* pObj);

	CPickupItemComponent* GetPickupComp(EGoalItemType eType);

	static void SpawnPickupItem(IEvent* cEvent, IComponent* cCenter);
	static void Shutdown(IEvent*, IComponent*);

	void DespawnPickupItem(CPickupItemComponent* pComp);
};

#endif