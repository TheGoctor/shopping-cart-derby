///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryManager.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	04/17/11
//
//  Purpose			:	A manager for inventory components
///////////////////////////////////////////////////////////////////////////////
#ifndef _CINVENTORYMANAGER_H_
#define _CINVENTORYMANAGER_H_

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
#include "..\\..\\Components\\Inventory\\CInventoryComponent.h"

// Foward Declares
class IEvent;

class CInventoryManager
{
private:
	// Components
	list<CInventoryComponent*, 
		CAllocator<CInventoryComponent*>> m_cInvComps;	// A list of all the Inventory Components

	// Trilogy of Evil
	CInventoryManager(void)
	{
	}
	CInventoryManager(CInventoryManager &ref);
	CInventoryManager &operator=(CInventoryManager &ref);

public:
	// Destructor
	~CInventoryManager(void)
	{
	}
	
	// Init
	void Init(void);

	static CInventoryManager *GetInstance(void);

	// Component Functions
	static int CreateInventoryComp(lua_State* pLua);
	static CInventoryComponent* CreateInventoryComp(CObject* pParent);

	// Callbacks
	static void GoalItemCollectedCallback(IEvent* e, IComponent* comp);
	static void PlayerRammedCallback(IEvent* e, IComponent* comp);
};

#endif