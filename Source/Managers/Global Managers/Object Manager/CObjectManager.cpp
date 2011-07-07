#include "CObjectManager.h"
#include "..\..\..\CObject.h"
#include "..\Event Manager\EventStructs.h"
using namespace EventStructs;

CObjectManager::CObjectManager(void)
{
}

CObjectManager::~CObjectManager(void)
{
	list<CObject*, CAllocator<CObject*>>::iterator pIter;
	pIter = m_cObjects.begin();
	while(pIter != m_cObjects.end())
	{
		if((*pIter)->GetID() != 0)
		{
			MMDEL(*pIter);
		}
		++pIter;
	}
	m_cObjects.clear();
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
void CObjectManager::EnableStateObjects(EGameState)
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
void CObjectManager::DisableStateObjects(EGameState)
{
}

int CObjectManager::CreateObject(lua_State* pL)
{
	// Get the Obj Name from LUA
	string szName = lua_tostring(pL, -5);
	float fPosX = (float)lua_tonumber(pL, -4);
	float fPosY = (float)lua_tonumber(pL, -3);
	float fPosZ = (float)lua_tonumber(pL, -2);
	float fRot  = (float)lua_tonumber(pL, -1);
	
	// Take the Data off the LUA Stack
	lua_pop(pL, 5);

	// Create the Obj
	CObject* pObj = CreateObject(szName, fPosX, fPosY, fPosZ, fRot);

	// Push the Obj onto the LUA Stack
	lua_pushlightuserdata(pL, pObj);

	// 1 Return Value(s)
	return 1;
}

CObject* CObjectManager::CreateObject(string szName,
									  float fPosX, float fPosY, float fPosZ, float fRot,
									  CObject* pcFramesParent)
{
	// Create the Object
	CObject* pObj = MMNEW(CObject(szName));
	
	pObj->GetTransform()->TranslateFrame(D3DXVECTOR3(fPosX, fPosY, fPosZ));
	pObj->GetTransform()->RotateFrame(D3DXVECTOR3(0,1,0), fRot);
	// Set Transform Frame
	if(pcFramesParent != NULL)
	{
		GetInstance()->BindObjects(pcFramesParent, pObj);
	}
	
	// Add Obg to List
	GetInstance()->m_cObjects.push_back(pObj);
	
	// Return the Obj
	return pObj;
}

int CObjectManager::DestroyObject(lua_State*)
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

int CObjectManager::BindObjects(lua_State* pL)
{
	CObject* pParent = (CObject*)lua_topointer(pL, -2);
	CObject* pChild = (CObject*)lua_topointer(pL, -1);

	GetInstance()->BindObjects(pParent, pChild);

	lua_pop(pL, 2);

	return 0;
}

void CObjectManager::BindObjects(CObject* pParent, CObject* pChild)
{
	pParent->GetTransform()->AddChildFrame(pChild->GetTransform());
}

int CObjectManager::GetObjectByName(lua_State* pLua)
{
	string szObjName = lua_tostring(pLua, -1);
	lua_pop(pLua, 1);

	CObject* pObj = GetInstance()->GetObjectByName(szObjName);

	lua_pushlightuserdata(pLua, (void*)pObj);

	return 1;
}

CObject* CObjectManager::GetObjectByName(string szName)
{
	list<CObject*, CAllocator<CObject*>>::iterator pIter;
	pIter = m_cObjects.begin();
	unsigned int nNameID = CIDGen::GetInstance()->GetID(szName);
	while(pIter != m_cObjects.end())
	{
		if((*pIter)->GetID() == nNameID)
			return *pIter;

		++pIter;
	}
	return NULL;
}