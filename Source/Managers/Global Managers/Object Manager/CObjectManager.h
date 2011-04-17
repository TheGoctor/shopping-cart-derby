////////////////////////////////////////////////////////////////////////////////
//	File			:	CObjectManager.h
//	Date			:	4/4/11
//	Mod. Date		:	4/4/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	This class is primarily used to load game objects upon 
//						entering a game state, and disabling game objects upon
//						exiting a state.
////////////////////////////////////////////////////////////////////////////////

#ifndef _COBJECTMANAGER_H_
#define _COBJECTMANAGER_H_

#include <map>
#include <list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\\..\\..\\CObject.h"
#include "..\\State Manager\\CStateManager.h"
#include "..\\Memory Manager\\CAllocator.h"

class CObjectManager
{
	//std::multimap<unsigned int, CObject> m_cObjects; // A list of all the game objects
													 // currently in the world and an 
													 // integer id relating to the game
													 // state they belong to
	std::list<CObject*, CAllocator<CObject*>> m_cObjects;

	// Constructors, Destructors
	CObjectManager(void);
	~CObjectManager(void);
	CObjectManager(const CObjectManager&);
	CObjectManager& operator=(const CObjectManager&);

	////////////////////////////////////////////////////////////////////////////////
	// EnableStateObjects()	:	Enables the Objects in the passed in states
	//
	// Ins		:	EGameStates	- The state to enable objects in
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void EnableStateObjects(EGameState eGameState);

	////////////////////////////////////////////////////////////////////////////////
	// DisableStateObjects()	:	Disables the Objects in the passed in states
	//
	// Ins		:	EGameStates	- The state to disable objects from
	//
	// Returns	:	void
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	void DisableStateObjects(EGameState eGameState);

public:

	////////////////////////////////////////////////////////////////////////////////
	// GetInstance()	:	Returns the Singleton
	//
	// Returns	:	static CObjectManager*
	//
	// Mod. Date		:	4/4/11
	// Mod. Initials	:	JL
	////////////////////////////////////////////////////////////////////////////////
	static CObjectManager* GetInstance(void) 
	{ 
		static CObjectManager instance;
		return &instance; 
	} 

	// Game Object Factory Funcs
	static int CreateObject(lua_State* pL);
	static CObject* CreateObject(string szName,  float fPosX = 0.0f, float fPosY = 0.0f, float fPosZ = 0.0);
	
	CObject* GetObject()
	{
		return (*m_cObjects.begin());
	}
};

#endif // _COBJECTMANAGER_H_