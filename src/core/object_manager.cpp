#include "CObjectManager.h"
#include "..\..\..\CObject.h"
#include "..\Event Manager\EventStructs.h"
using namespace EventStructs;

CObjectManager::CObjectManager(void)
{
}

CObjectManager::~CObjectManager(void)
{
	list<CObject*, scd::allocator<CObject*>>::iterator pIter;
	pIter = m_cObjects.begin();
	while(pIter != m_cObjects.end())
	{
		// Use ID to make sure the Object wasn't already deleted
		if((*pIter)->GetID() != 0)
		{
			MMDEL(*pIter);
		}
		++pIter;
	}
	m_cObjects.clear();
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
	
	// Add Obj to List
	GetInstance()->m_cObjects.push_back(pObj);
	
	// Return the Obj
	return pObj;
}

void CObjectManager::DestroyObject(CObject* pObj)
{
	// Send an event to notify all component managers to destroy the components
	// associated with this object
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
	// Get the name from Lua
	string szObjName = lua_tostring(pLua, -1);
	lua_pop(pLua, 1);

	CObject* pObj = GetInstance()->GetObjectByName(szObjName);

	// Pass the object pointer back to Lua
	lua_pushlightuserdata(pLua, (void*)pObj);

	return 1;
}

CObject* CObjectManager::GetObjectByName(string szName)
{
	// use the passed in name to find the object with the same name
	list<CObject*, scd::allocator<CObject*>>::iterator pIter;
	pIter = m_cObjects.begin();
	unsigned int nNameID = CIDGen::GetInstance()->GetID(szName);
	while(pIter != m_cObjects.end())
	{
		if((*pIter)->GetID() == nNameID)
			return *pIter;

		++pIter;
	}

	// Didn't find object with specified name
	return NULL;
}