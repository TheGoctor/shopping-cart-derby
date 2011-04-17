#include "CObjectManager.h"

CObjectManager::CObjectManager(void)
{
}

CObjectManager::~CObjectManager(void)
{
}

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
void CObjectManager::EnableStateObjects(EGameState eGameState)
{
}

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
void CObjectManager::DisableStateObjects(EGameState eGameState)
{
}

int CObjectManager::CreateObject(lua_State* pL)
{
	// Get the Obj Name from LUA
	string szName = lua_tostring(pL, 1);
	float fPosX = (float)lua_tonumber(pL, 2);
	float fPosY = (float)lua_tonumber(pL, 3);
	float fPosZ = (float)lua_tonumber(pL, 4);
	
	// Take the Data off the LUA Stack
	lua_pop(pL, 4);


	// Create the Obj
	CObject* pObj = CreateObject(szName, fPosX, fPosY, fPosZ);

	// Push the Obj onto the LUA Stack
	lua_pushlightuserdata(pL, pObj);

	// 1 Return Value(s)
	return 1;
}

CObject* CObjectManager::CreateObject(string szName, float fPosX, float fPosY, float fPosZ)
{
	// Create the Object
	CObject* pObj = MMNEW(CObject) CObject(szName);
	pObj->GetTransform()->TranslateFrame(D3DXVECTOR3(fPosX, fPosY, fPosZ));
	GetInstance()->m_cObjects.push_back(pObj);
	
	// Return the Obj
	return pObj;
}