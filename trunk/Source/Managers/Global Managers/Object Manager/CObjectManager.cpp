#include "CObjectManager.h"
#include "..\..\..\CObject.h"
#include "..\Event Manager\EventStructs.h"
using namespace EventStructs;

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

static void stackDump(lua_State* l)
{
	int i;
	int top = lua_gettop(l);
	for(i=1; i<=top; i++)
	{
		int t = lua_type(l, i);
		switch(t)
		{
		case LUA_TSTRING:
			{
				printf("%s", lua_tostring(l, i));
				break;
			}
		case LUA_TBOOLEAN:
			{
				printf(lua_toboolean(l, i) ? "true" : "false");
				break;
			}
		case LUA_TNUMBER:
			{
				printf("%g", lua_tonumber(l, i));
				break;
			}
		default:
			{
				printf("%s", lua_typename(l, t));
				break;
			}
		};
		printf(" ");
	}
	printf("\n");
}


int CObjectManager::CreateObject(lua_State* pL)
{
	stackDump(pL);

	// Get the Obj Name from LUA
	string szName = lua_tostring(pL, -4);
	float fPosX = (float)lua_tonumber(pL, -3);
	float fPosY = (float)lua_tonumber(pL, -2);
	float fPosZ = (float)lua_tonumber(pL, -1);
	
	// Take the Data off the LUA Stack
	lua_pop(pL, 4);

	// Create the Obj
	CObject* pObj = CreateObject(szName, fPosX, fPosY, fPosZ);

	// Push the Obj onto the LUA Stack
	lua_pushlightuserdata(pL, pObj);

	// 1 Return Value(s)
	return 1;
}

CObject* CObjectManager::CreateObject(string szName,
									  float fPosX, float fPosY, float fPosZ,
									  CFrame* pcFramesParent)
{
	// Create the Object
	CObject* pObj = MMNEW(CObject(szName));
	
	// Set Transform Frame
	if(pcFramesParent != NULL)
	{
		pcFramesParent->AddChildFrame(pObj->GetTransform());
	}
	pObj->GetTransform()->TranslateFrame(D3DXVECTOR3(fPosX, fPosY, fPosZ));
	
	// Add Obg to List
	GetInstance()->m_cObjects.push_back(pObj);
	
	// Return the Obj
	return pObj;
}

int CObjectManager::DestroyObject(lua_State* pL)
{
	// Not currently used, in here for future reference
	return 0;
}

void CObjectManager::DestroyObject(CObject* pObj)
{
	SendObjectEvent("DestroyObject", (IComponent*)GetInstance(), pObj, PRIORITY_IMMEDIATE);
	GetInstance()->m_cObjects.remove(pObj);
	MMDEL(pObj);
}