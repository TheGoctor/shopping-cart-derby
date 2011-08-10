///////////////////////////////////////////////////////////////////////////////
//  File			:	"CInventoryManager.h"
//
//  Author			:	Joseph Leybovich (JL)
//
//  Date Created	:	04/17/11
//
//	Last Changed	:	05/17/11
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
#include "..\\..\\CObject.h"

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
	
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: Init
	//
	// Return:  
	//		void
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		Initialized all the data members in this class.
	////////////////////////////////////////////////////////////////////////////////
	void Init(void);

	////////////////////////////////////////////////////////////////////////////////
	// Function: GetInstance
	//
	// Return:  
	//		CInventoryManager* - the instance of this singleton
	//
	// Parameters: 
	//		void
	//
	// Purpose:  
	//		First call creates this beautiful creature. Every call gives it to you.
	////////////////////////////////////////////////////////////////////////////////
	static CInventoryManager *GetInstance(void);

	// Component Functions
	////////////////////////////////////////////////////////////////////////////////
	// Function: CreateInventoryComp
	//
	// Return:  
	//		int - lua stack size
	//
	// Parameters: 
	//		lua_State* pLua - lua pointer sent from lua
	//
	// Purpose:  
	//		Lua function to create an inventory component
	////////////////////////////////////////////////////////////////////////////////
	static int CreateInventoryComp(lua_State* pLua);

	////////////////////////////////////////////////////////////////////////////////
	// Function: CreateInventoryComp
	//
	// Return:  
	//		CInventoryComponent* - The component created
	//
	// Parameters: 
	//		CObject* pParent - the object this component is attached to
	//
	// Purpose:  
	//		Creates and inits some data for an inventory component
	////////////////////////////////////////////////////////////////////////////////
	static CInventoryComponent* CreateInventoryComp(CObject* pParent);

	////////////////////////////////////////////////////////////////////////////////
	// Function: GetInventoryComponent
	//
	// Return:  
	//		CInventoryComponent* - The component created
	//
	// Parameters: 
	//		CObject* pObj - the object the desired component is attached to
	//
	// Purpose:  
	//		Finds the first component (if it exists) that is attached to the object
	////////////////////////////////////////////////////////////////////////////////
	CInventoryComponent* GetInventoryComponent(CObject* pObj);
	
};

#endif