////////////////////////////////////////////////////////////////////////////////
//	File			:	CObjectManager.h
//	Date			:	4/4/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	This class is primarily used to load game objects upon 
//						entering a game state, and disabling game objects upon
//						exiting a state.
////////////////////////////////////////////////////////////////////////////////

#ifndef _COBJECTMANAGER_H_
#define _COBJECTMANAGER_H_

#include <list>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "..\..\..\Enums.h"
#include "..\Event Manager\CIDGen.h"
#include "..\Memory Manager\CAllocator.h"

class CObject;
class CFrame;

class CObjectManager
{
	// A list of all the game objects currently in the world
	std::list<CObject*, CAllocator<CObject*>> m_cObjects;

	// Constructor
	CObjectManager(void);

	// Trilogy of Evil
	~CObjectManager(void);
	CObjectManager(const CObjectManager&);
	CObjectManager& operator=(const CObjectManager&);

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
		static CObjectManager cObjectManager;
		return &cObjectManager; 
	} 

	////////////////////////////////////////////////////////////////////////////////
	// Function: “CreateObject”
	//
	// Return:  CObject*	- Pointer to the newly created object
	//
	// Parameters:	string szName	-	The name of the object. Must be unique.
	//				float fPosX		-	X position of the object's starting location
	//				float fPosY		-	Y position of the object's starting location
	//				float fPosZ		-	Z position of the object's starting location
	//				float fRot		-	The object's starting rotation around Y axis
	//				CObject* pcFramesParent	-	The object's frame's parent.
	//
	// Purpose:  This function is a factory function used to create objects. All
	//		created objects must have a unique name and must be created using this
	//		function.
	////////////////////////////////////////////////////////////////////////////////
	static int CreateObject(lua_State* pL);
	static CObject* CreateObject(string szName,
		float fPosX = 0.0f, float fPosY = 0.0f, float fPosZ = 0.0,
		float fRot = 0.0f, CObject* pcFramesParent = NULL);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “DestroyObject”
	//
	// Return:  void
	//
	// Parameters:	CObject* pObj	-	The pointer to the object to be destroyed
	//
	// Purpose:  This function tells the component managers to destroy their
	//		components attached to this object, unregisters the object for all
	//		events, and deletes the object.
	////////////////////////////////////////////////////////////////////////////////
	static void DestroyObject(CObject* pObj);

	////////////////////////////////////////////////////////////////////////////////
	// Function: “BindObjects”
	//
	// Return:  void
	//
	// Parameters:	CObject* pParent	-	Pointer to the parent object
	//				CObject* pChild		-	Pointer to the child object
	//
	// Purpose:  This function will bind the frames of the two passed in objects
	//		so the child object and parent objects are bound appropriately in their
	//		frame hierarchies.
	////////////////////////////////////////////////////////////////////////////////
	static int BindObjects(lua_State* pL);
	void BindObjects(CObject* pParent, CObject* pChild);
	
	////////////////////////////////////////////////////////////////////////////////
	// Function: “GetObjectByName”
	//
	// Return:  CObject*	-	Pointer to the object with the passed in name.
	//							NULL if object wasn't found.
	//
	// Parameters:	string szName	-	The name of the object you're looking for
	//
	// Purpose:  This function takes in a string and returns the object pointer of
	//		the object that shares a name with the passed in string. If the object
	//		hasn't been found, the function returns NULL.
	////////////////////////////////////////////////////////////////////////////////
	static int GetObjectByName(lua_State* pLua);
	CObject* GetObjectByName(string szName);

};

#endif // _COBJECTMANAGER_H_